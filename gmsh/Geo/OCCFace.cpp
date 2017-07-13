// Gmsh - Copyright (C) 1997-2016 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@onelab.info>.

#include "GmshConfig.h"
#include "GmshMessage.h"
#include "GModel.h"
#include "GEdgeLoop.h"
#include "OCCVertex.h"
#include "OCCEdge.h"
#include "OCCFace.h"
#include "Numeric.h"
#include "Context.h"

#if defined(HAVE_OCC)
#include "GModelIO_OCC.h"
#include <Standard_Version.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_BSplineSurface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_BezierSurface.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <BRepMesh_FastDiscret.hxx>

#if ((OCC_VERSION_MAJOR == 6) && (OCC_VERSION_MINOR >= 6)) || (OCC_VERSION_MAJOR >= 7)
#  if ((OCC_VERSION_MAJOR == 6) && (OCC_VERSION_MINOR < 8))
#  include <BOPInt_Context.hxx>
#  else
#  include <IntTools_Context.hxx>
#  endif
#include <BOPTools_AlgoTools2D.hxx>
#include <BOPTools_AlgoTools.hxx>
#else
#include <IntTools_Context.hxx>
#include <BOPTools_Tools2D.hxx>
#include <BOPTools_Tools3D.hxx>
#endif

OCCFace::OCCFace(GModel *m, TopoDS_Face _s, int num)
  : GFace(m, num), s(TopoDS::Face(_s.Oriented(TopAbs_FORWARD)))
{
  setup();
  model()->getOCCInternals()->bind(s, num);
}

OCCFace::~OCCFace()
{
  model()->getOCCInternals()->unbind(s);
  model()->getOCCInternals()->unbind(_replaced);
}

void OCCFace::setup()
{
  edgeLoops.clear();
  l_edges.clear();
  l_dirs.clear();
  TopExp_Explorer exp2, exp3;
  for(exp2.Init(s, TopAbs_WIRE); exp2.More(); exp2.Next()){
    TopoDS_Wire wire = TopoDS::Wire(exp2.Current());
    Msg::Debug("OCC Face %d - New Wire", tag());
    std::list<GEdge*> l_wire;
    for(exp3.Init(wire, TopAbs_EDGE); exp3.More(); exp3.Next()){
      TopoDS_Edge edge = TopoDS::Edge(exp3.Current());
      GEdge *e = model()->getOCCInternals()->getOCCEdgeByNativePtr(model(), edge);
      if(!e){
	Msg::Error("Unknown edge in face %d", tag());
      }
      else if(edge.Orientation() == TopAbs_INTERNAL){
        Msg::Info("Adding embedded edge %d", e->tag());
        embedded_edges.push_back(e);
        OCCEdge *occe = (OCCEdge*)e;
        occe->setTrimmed(this);
      }
      else{
        l_wire.push_back(e);
        Msg::Debug("Edge %d (%d --> %d) ori %d", e->tag(),
                   e->getBeginVertex()->tag(), e->getEndVertex()->tag(),
                   edge.Orientation());
        e->addFace(this);
        if(!e->is3D()){
          OCCEdge *occe = (OCCEdge*)e;
          occe->setTrimmed(this);
        }
      }
    }

    GEdgeLoop el(l_wire);
    // printf("l_wire of size %d %d\n",l_wire.size(),el.count());
    for(GEdgeLoop::citer it = el.begin(); it != el.end(); ++it){
      l_edges.push_back(it->ge);
      l_dirs.push_back(it->_sign);
      if (el.count() == 2){
        it->ge->meshAttributes.minimumMeshSegments =
          std::max(it->ge->meshAttributes.minimumMeshSegments,2);
      }
      if (el.count() == 1){
        it->ge->meshAttributes.minimumMeshSegments =
          std::max(it->ge->meshAttributes.minimumMeshSegments,3);
      }
    }
    edgeLoops.push_back(el);
  }

  BRepAdaptor_Surface surface(s);
  _periodic[0] = surface.IsUPeriodic();
  _periodic[1] = surface.IsVPeriodic();

  ShapeAnalysis::GetFaceUVBounds(s, umin, umax, vmin, vmax);
  Msg::Debug("OCC Face %d with %d parameter bounds (%g,%g)(%g,%g)",
             tag(), l_edges.size(), umin, umax, vmin, vmax);
  // we do that for the projections to converge on the borders of the
  // surface
  const double du = umax - umin;
  const double dv = vmax - vmin;
  umin -= fabs(du) / 100.0;
  vmin -= fabs(dv) / 100.0;
  umax += fabs(du) / 100.0;
  vmax += fabs(dv) / 100.0;
  occface = BRep_Tool::Surface(s);

  // std::list<GEdge*>::const_iterator it = l_edges.begin();
  // for (; it != l_edges.end(); ++it){
  //   printf("edge %d : %d %d iseam %d \n", (*it)->tag(),
  //          (*it)->getBeginVertex()->tag(), (*it)->getEndVertex()->tag(),
  //          (*it)->isSeam(this));
  // }

  /*
  for(exp2.Init(s, TopAbs_VERTEX); exp2.More(); exp2.Next()){
    TopoDS_Vertex vertex = TopoDS::Vertex(exp2.Current());
    GVertex *v = model()->getOCCInternals()->getOCCVertexByNativePtr(model(), vertex);
    if(!v){
      Msg::Error("Unknown vertex in face %d", tag());
    }
    else if(vertex.Orientation() == TopAbs_INTERNAL){
      Msg::Info("Adding embedded vertex %d", v->tag());
      embedded_vertices.push_back(v);
    }
  }
  */
}

SBoundingBox3d OCCFace::bounds() const
{
  Bnd_Box b;
  BRepBndLib::Add(s, b);
  double xmin, ymin, zmin, xmax, ymax, zmax;
  b.Get(xmin, ymin, zmin, xmax, ymax, zmax);
  SBoundingBox3d bbox(xmin, ymin, zmin, xmax, ymax, zmax);
  return bbox;
}

Range<double> OCCFace::parBounds(int i) const
{
  double umin2, umax2, vmin2, vmax2;

  ShapeAnalysis::GetFaceUVBounds(s, umin2, umax2, vmin2, vmax2);
  if(i == 0)
    return Range<double>(umin2, umax2);
  return Range<double>(vmin2, vmax2);
}

SVector3 OCCFace::normal(const SPoint2 &param) const
{
  gp_Pnt pnt;
  gp_Vec du, dv;

  occface->D1(param.x(), param.y(), pnt, du, dv);

  SVector3 t1(du.X(), du.Y(), du.Z());
  SVector3 t2(dv.X(), dv.Y(), dv.Z());
  SVector3 n(crossprod(t1, t2));
  n.normalize();
  if (s.Orientation() == TopAbs_REVERSED) return n * (-1.);
  return n;
}

Pair<SVector3,SVector3> OCCFace::firstDer(const SPoint2 &param) const
{
  gp_Pnt pnt;
  gp_Vec du, dv;
  occface->D1(param.x(), param.y(), pnt, du, dv);
  return Pair<SVector3,SVector3>(SVector3(du.X(), du.Y(), du.Z()),
                                 SVector3(dv.X(), dv.Y(), dv.Z()));
}

void OCCFace::secondDer(const SPoint2 &param,
                        SVector3 *dudu, SVector3 *dvdv, SVector3 *dudv) const
{
  gp_Pnt pnt;
  gp_Vec du, dv, duu, dvv, duv;
  occface->D2(param.x(), param.y(), pnt, du, dv, duu, dvv, duv);

  *dudu = SVector3(duu.X(), duu.Y(), duu.Z());
  *dvdv = SVector3(dvv.X(), dvv.Y(), dvv.Z());
  *dudv = SVector3(duv.X(), duv.Y(), duv.Z());
}

GPoint OCCFace::point(double par1, double par2) const
{
  double pp[2] = {par1, par2};

#if 1
  gp_Pnt val = occface->Value(par1, par2);
  return GPoint(val.X(), val.Y(), val.Z(), this, pp);
#else // this is horribly slow!
  double umin2, umax2, vmin2, vmax2;

  ShapeAnalysis::GetFaceUVBounds(s, umin2, umax2, vmin2, vmax2);

  double du = umax2 - umin2;
  double dv = vmax2 - vmin2;

  if (par1 > (umax2+.1*du) || par1 < (umin2-.1*du) ||
      par2 > (vmax2+.1*dv) || par2 < (vmin2-.1*dv)){
    GPoint p(0,0,0, this, pp);
    p.setNoSuccess();
    return p;
  }

  try{
    gp_Pnt val;
    val = occface->Value(par1, par2);
    return GPoint(val.X(), val.Y(), val.Z(), this, pp);
  }
  catch(Standard_OutOfRange){
    GPoint p(0,0,0, this, pp);
    p.setNoSuccess();
    return p;
  }
#endif
}

GPoint OCCFace::closestPoint(const SPoint3 &qp, const double initialGuess[2]) const
{
  gp_Pnt pnt(qp.x(), qp.y(), qp.z());
  GeomAPI_ProjectPointOnSurf proj(pnt, occface, umin, umax, vmin, vmax);

  if(!proj.NbPoints()){
    Msg::Warning("OCC Project Point on Surface FAIL");
    GPoint gp(0, 0);
    gp.setNoSuccess();
    return gp;
  }

  double pp[2] = {initialGuess[0],initialGuess[1]};
  proj.LowerDistanceParameters(pp[0], pp[1]);

  // Msg::Info("projection lower distance parameters %g %g",pp[0],pp[1]);

  if((pp[0] < umin || umax < pp[0]) || (pp[1]<vmin || vmax<pp[1])){
    Msg::Warning("Point projection is out of face bounds");
    GPoint gp(0, 0);
    gp.setNoSuccess();
    return gp;
  }

  pnt = proj.NearestPoint();
  return GPoint(pnt.X(), pnt.Y(), pnt.Z(), this, pp);
}

SPoint2 OCCFace::parFromPoint(const SPoint3 &qp, bool onSurface) const
{
  gp_Pnt pnt(qp.x(), qp.y(), qp.z());
  GeomAPI_ProjectPointOnSurf proj(pnt, occface, umin, umax, vmin, vmax);
  if(!proj.NbPoints()){
    Msg::Error("OCC Project Point on Surface FAIL");
    return GFace::parFromPoint(qp);
  }
  double U, V;
  proj.LowerDistanceParameters(U, V);
  return SPoint2(U, V);
}

GEntity::GeomType OCCFace::geomType() const
{
  if (occface->DynamicType() == STANDARD_TYPE(Geom_Plane))
    return Plane;
  else if (occface->DynamicType() == STANDARD_TYPE(Geom_ToroidalSurface))
    return Torus;
  else if (occface->DynamicType() == STANDARD_TYPE(Geom_BezierSurface))
    return BezierSurface;
  else if (occface->DynamicType() == STANDARD_TYPE(Geom_CylindricalSurface))
    return Cylinder;
  else if (occface->DynamicType() == STANDARD_TYPE(Geom_ConicalSurface))
    return Cone;
  else if (occface->DynamicType() == STANDARD_TYPE(Geom_SurfaceOfRevolution))
    return SurfaceOfRevolution;
  else if (occface->DynamicType() == STANDARD_TYPE(Geom_SphericalSurface))
    return Sphere;
  else if (occface->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface))
    return BSplineSurface;
  return Unknown;
}

double OCCFace::curvatureMax(const SPoint2 &param) const
{
  const double eps = 1.e-12;
  BRepAdaptor_Surface sf(s, Standard_True);
  BRepLProp_SLProps prop(sf, 2, eps);
  prop.SetParameters (param.x(), param.y());

  if(!prop.IsCurvatureDefined()){
    return eps;
  }

  return std::max(fabs(prop.MinCurvature()), fabs(prop.MaxCurvature()));
}

double OCCFace::curvatures(const SPoint2 &param,
                           SVector3 *dirMax,
                           SVector3 *dirMin,
                           double *curvMax,
                           double *curvMin) const
{
  const double eps = 1.e-12;
  BRepAdaptor_Surface sf(s, Standard_True);
  BRepLProp_SLProps prop(sf, 2, eps);
  prop.SetParameters (param.x(),param.y());

  if (!prop.IsCurvatureDefined()){
    return -1.;
  }

  *curvMax = prop.MaxCurvature();
  *curvMin = prop.MinCurvature();

  gp_Dir dMax = gp_Dir();
  gp_Dir dMin = gp_Dir();
  prop.CurvatureDirections(dMax,dMin);

  (*dirMax)[0] = dMax.X();
  (*dirMax)[1] = dMax.Y();
  (*dirMax)[2] = dMax.Z();
  (*dirMin)[0] = dMin.X();
  (*dirMin)[1] = dMin.Y();
  (*dirMin)[2] = dMin.Z();

  return *curvMax;
}

bool OCCFace::containsPoint(const SPoint3 &pt) const
{
  if(geomType() == Plane){
    gp_Pln pl = Handle(Geom_Plane)::DownCast(occface)->Pln();
    double n[3], c;
    pl.Coefficients(n[0], n[1], n[2], c);
    norme(n);
    double angle = 0.;
    double v[3] = {pt.x(), pt.y(), pt.z()};

    std::list<int>::const_iterator ito = l_dirs.begin();
    for(std::list<GEdge*>::const_iterator it = l_edges.begin(); it != l_edges.end(); it++){
      GEdge *c = *it;
      int ori = 1;
      if(ito != l_dirs.end()){
        ori = *ito;
        ++ito;
      }
      int N = 10;
      Range<double> range = c->parBounds(0);
      for(int j = 0; j < N ; j++) {
        double u1 = (double)j / (double)N;
        double u2 = (double)(j + 1) / (double)N;
        if(ori < 0){
          u1 = 1. - u1;
          u2 = 1. - u2;
        }
        GPoint pp1 = c->point(range.low() + u1 * (range.high() - range.low()));
        GPoint pp2 = c->point(range.low() + u2 * (range.high() - range.low()));
        double v1[3] = {pp1.x(), pp1.y(), pp1.z()};
        double v2[3] = {pp2.x(), pp2.y(), pp2.z()};
        angle += angle_plan(v, v1, v2, n);
      }
    }
    // we're inside if angle equals 2 * pi
    if(fabs(angle) > 2 * M_PI - 0.5 && fabs(angle) < 2 * M_PI + 0.5)
      return true;
    return false;
  }
  else
    Msg::Error("Not Done Yet ...");
  return false;
}

surface_params OCCFace::getSurfaceParams() const
{
  surface_params p;
  switch(geomType()){
  case GEntity::Cylinder:
    p.radius = Handle(Geom_CylindricalSurface)::DownCast(occface)->Radius();
    break;
  case GEntity::Sphere:
    p.radius = Handle(Geom_SphericalSurface)::DownCast(occface)->Radius();
    break;
  default:
    break;
  }
  return p;
}

bool OCCFace::buildSTLTriangulation(bool force)
{
  if(stl_triangles.size()){
    if(force){
      stl_vertices.clear();
      stl_triangles.clear();
    }
    else
      return true;
  }

  Bnd_Box aBox;
  BRepBndLib::Add(s, aBox);

#if (OCC_VERSION_MAJOR >= 7)
  BRepMesh_FastDiscret::Parameters parameters;
  parameters.Deflection = 0.1;
  parameters.Angle = 0.5;
  parameters.Relative = Standard_True;
  BRepMesh_FastDiscret aMesher(aBox, parameters);
#else
  BRepMesh_FastDiscret aMesher(0.1, 0.5, aBox, Standard_False, Standard_False,
                               Standard_True, Standard_False);
#endif
#if (OCC_VERSION_MAJOR == 6) && (OCC_VERSION_MINOR < 5)
  aMesher.Add(s);
#else
  aMesher.Perform(s);
#endif

  TopLoc_Location loc;
  Handle(Poly_Triangulation) triangulation = BRep_Tool::Triangulation(s, loc);

  if(triangulation.IsNull() || !triangulation->HasUVNodes()){
    if(triangulation.IsNull())
      Msg::Warning("OCC STL triangulation of surface %d failed", tag());
    else
      Msg::Warning("OCC STL triangulation of surface %d has no u,v coordinates", tag());
    // add a dummy triangle so that we won't try again
    stl_vertices.push_back(SPoint2(0., 0.));
    stl_triangles.push_back(0);
    stl_triangles.push_back(0);
    stl_triangles.push_back(0);
    return false;
  }

  for(int i = 1; i <= triangulation->NbNodes(); i++){
    gp_Pnt2d p = (triangulation->UVNodes())(i);
    stl_vertices.push_back(SPoint2(p.X(), p.Y()));
  }

  bool reverse = false;
  for(int i = 1; i <= triangulation->NbTriangles(); i++){
    Poly_Triangle triangle = (triangulation->Triangles())(i);
    int p1, p2, p3;
    triangle.Get(p1, p2, p3);

    // orient STL mesh to get normal right
    if(i == 1){
      gp_Pnt2d gp1 = (triangulation->UVNodes())(p1);
      gp_Pnt2d gp2 = (triangulation->UVNodes())(p2);
      gp_Pnt2d gp3 = (triangulation->UVNodes())(p3);
      SPoint2 b = SPoint2(gp1.X(), gp1.Y()) + SPoint2(gp2.X(), gp2.Y()) +
        SPoint2(gp3.X(), gp3.Y());
      b *= 1. / 3.;
      SVector3 nf = normal(b);
      GPoint sp1 = point(gp1.X(), gp1.Y());
      GPoint sp2 = point(gp2.X(), gp2.Y());
      GPoint sp3 = point(gp3.X(), gp3.Y());
      double n[3];
      normal3points(sp1.x(), sp1.y(), sp1.z(),
                    sp2.x(), sp2.y(), sp2.z(),
                    sp3.x(), sp3.y(), sp3.z(), n);
      SVector3 ne(n[0], n[1], n[2]);
      if(dot(ne, nf) < 0){
        Msg::Debug("Reversing orientation of STL mesh in face %d", tag());
        reverse = true;
      }
    }

    if(!reverse){
      stl_triangles.push_back(p1 - 1);
      stl_triangles.push_back(p2 - 1);
      stl_triangles.push_back(p3 - 1);
    }
    else{
      stl_triangles.push_back(p1 - 1);
      stl_triangles.push_back(p3 - 1);
      stl_triangles.push_back(p2 - 1);
    }
  }

  return true;
}

void OCCFace::replaceEdgesInternal(std::list<GEdge*> &new_edges)
{

#if (OCC_VERSION_MAJOR == 6) && (OCC_VERSION_MINOR >= 6) && (OCC_VERSION_MINOR < 8)
  Handle(BOPInt_Context) myContext = new BOPInt_Context;
#elif defined(OCC_VERSION_HEX) && OCC_VERSION_HEX >= 0x060503
  Handle(IntTools_Context) myContext = new IntTools_Context;
#else
  IntTools_Context myContext;
#endif

  // we simply replace old edges by new edges in the structure

  // make a copy of s
  TopoDS_Face copy_of_s_forward = s;
  copy_of_s_forward.Orientation(TopAbs_FORWARD);
  // make a copy of occface
  TopLoc_Location location;
  Handle(Geom_Surface) copy_of_occface = BRep_Tool::Surface(copy_of_s_forward, location);
  // check periodicity
  bool bIsUPeriodic = _periodic[0];
  // get tolerance
  double tolerance = BRep_Tool::Tolerance(copy_of_s_forward);

  BRep_Builder aBB;
  TopoDS_Face newFace;
  aBB.MakeFace(newFace, copy_of_occface, location, tolerance);
  // expolore the face
  TopExp_Explorer aExpW, aExpE;
  aExpW.Init(copy_of_s_forward, TopAbs_WIRE);
  for (; aExpW.More(); aExpW.Next()) {
    TopoDS_Wire newWire;
    aBB.MakeWire(newWire);
    const TopoDS_Wire& aW=TopoDS::Wire(aExpW.Current());
    aExpE.Init(aW, TopAbs_EDGE);
    for (; aExpE.More(); aExpE.Next()) {
      const TopoDS_Edge& aE=TopoDS::Edge(aExpE.Current());
      std::list<GEdge*>::iterator it  = l_edges.begin();
      std::list<GEdge*>::iterator it2 = new_edges.begin();
      TopoDS_Edge aER;
      Msg::Debug("trying to replace %d by %d",(*it)->tag(),(*it2)->tag());
      for ( ; it != l_edges.end(); ++it, ++it2){
	OCCEdge *occEd = dynamic_cast<OCCEdge*>(*it);
	TopoDS_Edge olde = occEd->getTopoDS_Edge();
	if (olde.IsSame(aE)){
	  aER = *((TopoDS_Edge*)(*it2)->getNativePtr());
	}
	else {
	  olde = occEd->getTopoDS_EdgeOld();
	  if (olde.IsSame(aE)){
	    aER = *((TopoDS_Edge*)(*it2)->getNativePtr());
	  }
	}
      }
      if (aER.IsNull()){
	Msg::Error("cannot find an edge for gluing a face");
      }
      aER.Orientation(TopAbs_FORWARD);
      if (!BRep_Tool::Degenerated(aER)) {
	if (bIsUPeriodic) {
	  Standard_Real aT1, aT2, aTx, aUx;
	  BRep_Builder aBB_;
	  Handle(Geom2d_Curve) aC2D =
            BRep_Tool::CurveOnSurface(aER, copy_of_s_forward, aT1, aT2);
	  if (!aC2D.IsNull()) {
	    if (BRep_Tool::IsClosed(aER, copy_of_s_forward)) {
	      continue;
	    }
	    else{
#if ((OCC_VERSION_MAJOR == 6) && (OCC_VERSION_MINOR >= 6)) || (OCC_VERSION_MAJOR >= 7)
	      aTx = BOPTools_AlgoTools2D::IntermediatePoint(aT1, aT2);
#else
	      aTx = BOPTools_Tools2D::IntermediatePoint(aT1, aT2);
#endif
	      gp_Pnt2d aP2D;
	      aC2D->D0(aTx, aP2D);
	      aUx=aP2D.X();
	      if (aUx < umin || aUx > umax) {
		// need to rebuild
		Handle(Geom2d_Curve) aC2Dx;
		aBB_.UpdateEdge(aER, aC2Dx, copy_of_s_forward , BRep_Tool::Tolerance(aE));
	      }
	    }
	  }
	}
#if ((OCC_VERSION_MAJOR == 6) && (OCC_VERSION_MINOR >= 6)) || (OCC_VERSION_MAJOR >= 7)
	BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aER, copy_of_s_forward);
#else
	BOPTools_Tools2D::BuildPCurveForEdgeOnFace(aER, copy_of_s_forward);
#endif
	// orient image
	Standard_Boolean bIsToReverse =
#if ((OCC_VERSION_MAJOR == 6) && (OCC_VERSION_MINOR >= 6)) || (OCC_VERSION_MAJOR >= 7)
          BOPTools_AlgoTools::IsSplitToReverse(aER, aE, myContext);
#else
          BOPTools_Tools3D::IsSplitToReverse1(aER, aE, myContext);
#endif
	if (bIsToReverse) {
	  aER.Reverse();
	}
      }
      else {
	aER.Orientation(aE.Orientation());
      }
      aBB.Add(newWire, aER);
    }
    aBB.Add(newFace, newWire);
  }
  _replaced = s;
  s = newFace;

  setup();
  model()->getOCCInternals()->bind(_replaced, tag());
}

bool OCCFace::isSphere (double &radius, SPoint3 &center) const
{
  switch(geomType()){
  case GEntity::Sphere:
    {
      radius = Handle(Geom_SphericalSurface)::DownCast(occface)->Radius();
      gp_Ax3 pos  = Handle(Geom_SphericalSurface)::DownCast(occface)->Position();
      gp_Ax1 axis = pos.Axis();
      gp_Pnt loc = axis.Location();
      center = SPoint3(loc.X(),loc.Y(),loc.Z());
    }
    return true;
  default:
    return false;
  }
}

#endif
