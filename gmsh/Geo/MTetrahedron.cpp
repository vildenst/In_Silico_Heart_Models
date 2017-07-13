// Gmsh - Copyright (C) 1997-2016 C. Geuzaine, J.-F. Remacle
//
// See the LICENSE.txt file for license information. Please report all
// bugs and problems to the public mailing list <gmsh@onelab.info>.

#include "GmshConfig.h"
#include "MTetrahedron.h"
#include "Numeric.h"
#include "Context.h"
#include "BasisFactory.h"


#if defined(HAVE_MESH)
#include "qualityMeasures.h"
#include "meshGFaceDelaunayInsertion.h"
#include "meshGRegionDelaunayInsertion.h"
#endif

#define SQU(a)      ((a)*(a))

SPoint3 MTetrahedron::circumcenter()
{
#if defined(HAVE_MESH)
  MTet4 t(this, 0);
  double res[3];
  t.circumcenter(res);
  return SPoint3(res[0], res[1], res[2]);
#else
  return SPoint3(0., 0., 0.);
#endif
}

double MTetrahedron::getCircumRadius()
{
#if defined(HAVE_MESH)
  SPoint3 center = circumcenter();
  const double dx = getVertex(0)->x() - center.x();
  const double dy = getVertex(0)->y() - center.y();
  const double dz = getVertex(0)->z() - center.z();
  double circum_radius = sqrt(dx * dx + dy * dy + dz * dz);
  return circum_radius;
#else
  return 0.0;
#endif
}

double MTetrahedron::getInnerRadius()
{
  // radius of inscribed sphere = 3 * Volume / sum(Area_i)
  double dist[3], face_area = 0.;
  double vol = getVolume();
  for(int i = 0; i < 4; i++){
    MFace f = getFace(i);
    for (int j = 0; j < 3; j++){
      MEdge e = f.getEdge(j);
      dist[j] = e.getVertex(0)->distance(e.getVertex(1));
    }
    face_area += 0.25 * sqrt((dist[0] + dist[1] + dist[2]) *
                             (-dist[0] + dist[1] + dist[2]) *
                             (dist[0] - dist[1] + dist[2]) *
                             (dist[0] + dist[1] - dist[2]));
  }
  return 3 * vol / face_area;
}

double MTetrahedron::gammaShapeMeasure()
{
#if defined(HAVE_MESH)
  double vol;
  return qmTetrahedron::qm(this, qmTetrahedron::QMTET_GAMMA, &vol);
#else
  return 0.;
#endif
}

double MTetrahedron::etaShapeMeasure()
{
#if defined(HAVE_MESH)
  double vol;
  return qmTetrahedron::qm(this, qmTetrahedron::QMTET_ETA, &vol);
#else
  return 0.;
#endif
}

double MTetrahedron::getVolume()
{
  double mat[3][3];
  getMat(mat);
  return det3x3(mat) / 6.;
}

void MTetrahedron::xyz2uvw(double xyz[3], double uvw[3]) const
{
  double mat[3][3], b[3], det;
  getMat(mat);
  b[0] = xyz[0] - getVertex(0)->x();
  b[1] = xyz[1] - getVertex(0)->y();
  b[2] = xyz[2] - getVertex(0)->z();
  sys3x3(mat, b, uvw, &det);
}

int MTetrahedron10::getNumEdgesRep(bool curved){
  return curved ? 6 * CTX::instance()->mesh.numSubEdges : 6;
}

int MTetrahedronN::getNumEdgesRep(bool curved){
  return curved ? 6 * CTX::instance()->mesh.numSubEdges : 6;
}

static void _myGetEdgeRep(MTetrahedron *tet, int num, double *x, double *y, double *z,
                          SVector3 *n, int numSubEdges)
{
  static double pp[4][3] = {{0,0,0},{1,0,0},{0,1,0},{0,0,1}};
  static int ed [6][2] = {{0,1},{0,2},{0,3},{1,2},{1,3},{2,3}};
  int iEdge = num / numSubEdges;
  int iSubEdge = num % numSubEdges;

  int iVertex1 = ed [iEdge][0];
  int iVertex2 = ed [iEdge][1];
  double t1 = (double) iSubEdge / (double) numSubEdges;
  double u1 = pp[iVertex1][0] * (1.-t1) + pp[iVertex2][0] * t1;
  double v1 = pp[iVertex1][1] * (1.-t1) + pp[iVertex2][1] * t1;
  double w1 = pp[iVertex1][2] * (1.-t1) + pp[iVertex2][2] * t1;

  double t2 = (double) (iSubEdge+1) / (double) numSubEdges;
  double u2 = pp[iVertex1][0] * (1.-t2) + pp[iVertex2][0] * t2;
  double v2 = pp[iVertex1][1] * (1.-t2) + pp[iVertex2][1] * t2;
  double w2 = pp[iVertex1][2] * (1.-t2) + pp[iVertex2][2] * t2;

  SPoint3 pnt1, pnt2;
  tet->pnt(u1,v1,w1,pnt1);
  tet->pnt(u2,v2,w2,pnt2);
  x[0] = pnt1.x(); x[1] = pnt2.x();
  y[0] = pnt1.y(); y[1] = pnt2.y();
  z[0] = pnt1.z(); z[1] = pnt2.z();

  // not great, but better than nothing
  static const int f[6] = {0, 0, 0, 1, 2, 3};
  n[0] = n[1] = tet->getFace(f[iEdge]).normal();
}

void MTetrahedron10::getEdgeRep(bool curved, int num,
                                double *x, double *y, double *z, SVector3 *n)
{
  if (curved) _myGetEdgeRep(this, num, x, y, z, n, CTX::instance()->mesh.numSubEdges);
  else MTetrahedron::getEdgeRep(false, num, x, y, z, n);
}

void MTetrahedronN::getEdgeRep(bool curved, int num,
                               double *x, double *y, double *z, SVector3 *n)
{
  if (curved) _myGetEdgeRep(this, num, x, y, z, n, CTX::instance()->mesh.numSubEdges);
  else MTetrahedron::getEdgeRep(false, num, x, y, z, n);
}

int MTetrahedronN::getNumFacesRep(bool curved) {
  return curved ? 4 * SQU(CTX::instance()->mesh.numSubEdges) : 4;
}

int MTetrahedron10::getNumFacesRep(bool curved) {
  return curved ? 4 * SQU(CTX::instance()->mesh.numSubEdges) : 4;
}

static void _myGetFaceRep(MTetrahedron *tet, int num, double *x, double *y, double *z,
                          SVector3 *n, int numSubEdges)
{
  static double pp[4][3] = {{0,0,0},{1,0,0},{0,1,0},{0,0,1}};
  static int fak [4][3] = {{0,1,2},{0,1,3},{0,2,3},{1,2,3}};
  int iFace    = num / (numSubEdges * numSubEdges);
  int iSubFace = num % (numSubEdges * numSubEdges);

  int iVertex1 = fak[iFace][0];
  int iVertex2 = fak[iFace][1];
  int iVertex3 = fak[iFace][2];

  /*
    0
    0 1
    0 1 2
    0 1 2 3
    0 1 2 3 4
    0 1 2 3 4 5
  */

  // on the first layer, we have (numSubEdges-1) * 2 + 1 triangles
  // on the second layer, we have (numSubEdges-2) * 2 + 1 triangles
  // on the ith layer, we have (numSubEdges-1-i) * 2 + 1 triangles
  int ix = 0, iy = 0;
  int nbt = 0;
  for (int i = 0; i < numSubEdges; i++){
    int nbl = (numSubEdges - i - 1) * 2 + 1;
    nbt += nbl;
    if (nbt > iSubFace){
      iy = i;
      ix = nbl - (nbt - iSubFace);
      break;
    }
  }

  const double d = 1. / numSubEdges;

  SPoint3 pnt1, pnt2, pnt3;
  double u1, v1, u2, v2, u3, v3;
  if (ix % 2 == 0){
    u1 = ix / 2 * d; v1= iy*d;
    u2 = (ix / 2 + 1) * d ; v2 =  iy * d;
    u3 = ix / 2 * d ; v3 =  (iy+1) * d;
  }
  else{
    u1 = (ix / 2 + 1) * d; v1= iy * d;
    u2 = (ix / 2 + 1) * d; v2= (iy + 1) * d;
    u3 = ix / 2 * d ; v3 =  (iy + 1) * d;
  }

  double U1 = pp[iVertex1][0] * (1.-u1-v1) + pp[iVertex2][0] * u1 + pp[iVertex3][0] * v1;
  double U2 = pp[iVertex1][0] * (1.-u2-v2) + pp[iVertex2][0] * u2 + pp[iVertex3][0] * v2;
  double U3 = pp[iVertex1][0] * (1.-u3-v3) + pp[iVertex2][0] * u3 + pp[iVertex3][0] * v3;

  double V1 = pp[iVertex1][1] * (1.-u1-v1) + pp[iVertex2][1] * u1 + pp[iVertex3][1] * v1;
  double V2 = pp[iVertex1][1] * (1.-u2-v2) + pp[iVertex2][1] * u2 + pp[iVertex3][1] * v2;
  double V3 = pp[iVertex1][1] * (1.-u3-v3) + pp[iVertex2][1] * u3 + pp[iVertex3][1] * v3;

  double W1 = pp[iVertex1][2] * (1.-u1-v1) + pp[iVertex2][2] * u1 + pp[iVertex3][2] * v1;
  double W2 = pp[iVertex1][2] * (1.-u2-v2) + pp[iVertex2][2] * u2 + pp[iVertex3][2] * v2;
  double W3 = pp[iVertex1][2] * (1.-u3-v3) + pp[iVertex2][2] * u3 + pp[iVertex3][2] * v3;

  tet->pnt(U1, V1, W1, pnt1);
  tet->pnt(U2, V2, W2, pnt2);
  tet->pnt(U3, V3, W3, pnt3);

  x[0] = pnt1.x(); x[1] = pnt2.x(); x[2] = pnt3.x();
  y[0] = pnt1.y(); y[1] = pnt2.y(); y[2] = pnt3.y();
  z[0] = pnt1.z(); z[1] = pnt2.z(); z[2] = pnt3.z();

  SVector3 d1(x[1] - x[0], y[1] - y[0], z[1] - z[0]);
  SVector3 d2(x[2] - x[0], y[2] - y[0], z[2] - z[0]);
  n[0] = crossprod(d1, d2);
  n[0].normalize();
  n[1] = n[0];
  n[2] = n[0];
}

void MTetrahedronN::getFaceRep(bool curved, int num,
                               double *x, double *y, double *z, SVector3 *n)
{
  if (curved) _myGetFaceRep(this, num, x, y, z, n, CTX::instance()->mesh.numSubEdges);
  else MTetrahedron::getFaceRep(false, num, x, y, z, n);
}

void MTetrahedron10::getFaceRep(bool curved, int num,
                                double *x, double *y, double *z, SVector3 *n)
{
  if (curved) _myGetFaceRep(this, num, x, y, z, n, CTX::instance()->mesh.numSubEdges);
  else MTetrahedron::getFaceRep(false, num, x, y, z, n);
}

void MTetrahedron::getIntegrationPoints(int pOrder, int *npts, IntPt **pts)
{
  *npts = getNGQTetPts(pOrder);
  *pts = getGQTetPts(pOrder);
}

void MTetrahedron::getFaceInfo(const MFace &face, int &ithFace, int &sign, int &rot) const
{
  for (ithFace = 0; ithFace < 4; ithFace++){
    MVertex *v0 = _v[faces_tetra(ithFace, 0)];
    MVertex *v1 = _v[faces_tetra(ithFace, 1)];
    MVertex *v2 = _v[faces_tetra(ithFace, 2)];

    if (v0 == face.getVertex(0) && v1 == face.getVertex(1) && v2 == face.getVertex(2)){
      sign = 1; rot = 0; return;
    }
    if (v0 == face.getVertex(1) && v1 == face.getVertex(2) && v2 == face.getVertex(0)){
      sign = 1; rot = 1; return;
    }
    if (v0 == face.getVertex(2) && v1 == face.getVertex(0) && v2 == face.getVertex(1)){
      sign = 1; rot = 2; return;
    }
    if (v0 == face.getVertex(0) && v1 == face.getVertex(2) && v2 == face.getVertex(1)){
      sign = -1; rot = 0; return;
    }
    if (v0 == face.getVertex(1) && v1 == face.getVertex(0) && v2 == face.getVertex(2)){
      sign = -1; rot = 1; return;
    }
    if (v0 == face.getVertex(2) && v1 == face.getVertex(1) && v2 == face.getVertex(0)){
      sign = -1; rot = 2; return;
    }
  }
  Msg::Error("Could not get face information for tetrahedron %d", getNum());
}

static std::vector<std::vector<int> > tetReverseIndices(20);

const std::vector<int> &MTetrahedronN::_getReverseIndices (int order)
{
  if(order >= (int)tetReverseIndices.size())
    tetReverseIndices.resize(order + 1);
  std::vector<int> &r = tetReverseIndices[order];
  if (r.size() != 0) return r;
  //
  // not the funniest code ever ... (guaranteed correct only up to order 5)
  //
  int nb = (order+1)*(order+2)*(order+3)/6;
  r.resize(nb);
  int p=0;
  for (int layerOrder = order; layerOrder>=0; layerOrder-=4) {
    //principal vertices
    r[p+0] = p+0;
    if (layerOrder ==0) break;
    r[p+1] = p+2;
    r[p+2] = p+1;
    r[p+3] = p+3;
    p+=4;
    for (int i = 0; i<layerOrder-1; i++) {
      //E2 reversed switches with E0
      r[p+i] = p+3*(layerOrder-1)-(i+1);
      r[p+3*(layerOrder-1)-(i+1)] = p+i;
      //E1 is reversed
      r[p+(layerOrder-1)+i] = p+2*(layerOrder-1)-(i+1);
      //E3 is preserved
      r[p+3*(layerOrder-1)+i] = p+3*(layerOrder-1)+i;
      //E4 switches with E5
      r[p+4*(layerOrder-1)+i] = p+5*(layerOrder-1)+i;
      r[p+5*(layerOrder-1)+i] = p+4*(layerOrder-1)+i;
    }
    p+=6*(layerOrder-1);
    //F0(=012) switches its nodes 1 and 2
    for (int of = layerOrder-3; of >= 0; of -= 3) {
      r[p] = p;
      if (of == 0) {
        p+=1;
        break;
      }
      r[p+1] = p+2;
      r[p+2] = p+1;
      for (int i = 0; i < of-1; i++) {
        //switch edges 0 and 2
        r[p+3+i] = p+3+3*(of-1)-(i+1);
        r[p+3+3*(of-1)-(i+1)] = p+3+i;
        //reverse edge 1
        r[p+3+(of-1)+i] = p+3+2*(of-1)-(i+1);
      }
      p += 3*of;
    }
    //F1 (=013) reversed switches with F2 (=032)
    int nf = (layerOrder-2)*(layerOrder-1)/2;
    for (int of = layerOrder-3; of >= 0; of -= 3) {
      r[p] = p+nf;
      r[p+nf] = p;
      if (of == 0) {
        p += 1;
        break;
      }
      r[p+1] = p+nf+2;
      r[p+nf+2] = p+1;
      r[p+2] = p+nf+1;
      r[p+nf+1] = p+2;
      for (int i = 0; i < of-1; i++) {
        //switch edges 0 and 2
        r[p+3+i] = p+3+3*(of-1)-(i+1)+nf;
        r[p+3+3*(of-1)-(i+1)] = p+3+i+nf;
        r[p+3+i+nf] = p+3+3*(of-1)-(i+1);
        r[p+3+3*(of-1)-(i+1)+nf] = p+3+i;
        //reverse edge 1
        r[p+3+(of-1)+i] = p+3+2*(of-1)-(i+1)+nf;
        r[p+3+(of-1)+i+nf] = p+3+2*(of-1)-(i+1);
      }
      p += 3*of;
    }
    p+=nf;

    //F3(=312) switches its nodes 1 and 2
    for (int of = layerOrder-3; of >= 0; of -= 3) {
      r[p] = p;
      if (of == 0) {
        p += 1;
        break;
      }
      r[p+1] = p+2;
      r[p+2] = p+1;
      for (int i = 0; i < of-1; i++) {
        //switch edges 0 and 2
        r[p+3+i] = p+3+3*(of-1)-(i+1);
        r[p+3+3*(of-1)-(i+1)] = p+3+i;
        //reverse edge 1
        r[p+3+(of-1)+i] = p+3+2*(of-1)-(i+1);
      }
      p += 3*of;
    }
  }
  return r;
}
