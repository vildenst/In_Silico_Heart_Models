"""
Created by Vilde N. Strom, July 2017.
	Simula Research Laboratory
"""

import os
import shutil 
root=os.getcwd()	#path to In_Silico_Heart_Models


"""
PART 1: MATLAB SLICE ALIGNMENT
"""
#running the matlab script alignAll.m
def run_matlab(nr_errors,err_list):
	os.system('sh run_matlab.sh')	
	scar_folder ='{}/Matlab_Process/Data/ScarImages/MetaImages/'.format(root)
	scar_files=os.listdir(scar_folder)
	N2=len(scar_files) #files produces from matlab. 
	if (N-nr_errors)*2 > N2:	#not all .mat files are processed
		remove_error(N2,nr_errors,err_list)

#if an error occured in matlab, this function removes the error file(s) and restarts
def remove_error(N2,nr_errors,err_list): 
		error=(N2+nr_errors*2)/2+1
		err_path='{}Patient_{}.mat'.format(dest,error)
		os.remove(err_path)		
		nr_errors+=1
		err_list.append(error)
		run_matlab(nr_errors,err_list)

#if the error file(s) produced any text files, they're deleted here.
def remove_error_files(fname):
	rm_path='{}/Data/Texts/'.format(os.getcwd())
	for i in ['LVEndo','LVEpi','RVEndo','RVEpi']:
		if os.path.isfile('{}{}-{}-Frame_1.txt'.format(rm_path,fname,i)):
			os.remove('{}{}-{}-Frame_1.txt'.format(rm_path,fname,i))

source ='{}/seg/'.format(root) #.mat files source path
files=os.listdir(source)
N=len(files)	#number of .mat files to be processed
nr_errors=0
err_list=[]
dest='{}/Matlab_Process/Data/Seg/'.format(root) #.mat files dest path
for f in files:
	shutil.copy(source+f,dest+f)	#move all .mat files to dest

os.chdir('Matlab_Process')

run_matlab(nr_errors,err_list)
if err_list:	#errors occured
	for nr in err_list:
		fname='Patient_{}'.format(nr)
		remove_error_files(fname)
		print('{} was removed due to errors. Will continue without it.'.format(fname))

print('#############################')
print('#PART 1 DONE: SLICES ALIGNED#')
print('#############################')


"""
PART 2: SURFACE GENERATION
"""
#moves all files in a directory
def move(src,dst):
	src_files=os.listdir(src)
	for f in src_files:
		shutil.move(src+'/'+f,dst+'/'+f)

#runs a bash program with a given path
def run(path,script):
	os.chdir(path)
	os.system('sh {}'.format(script))


#removes all files in a list of folders
def remove(folder_list,matpath):
	for i in folder_list:
		path=matpath+'/Data/'+i
		files=os.listdir(path)
		for f in files:
			os.remove(path+'/'+f)

#creating new output folder for the surface data
import datetime
now=datetime.datetime.now()
time = now.strftime("%d.%m-%H.%M")
Date='Data-'+time	#name of output folder
print('Creating folder {} for data storage'.format(Date))
newdir=root+'/Surfaces/'+Date
os.mkdir(newdir)

#folder paths
matpath=root+'/Matlab_Process'
Conv_src=matpath+'/Data/Texts'
Scar_src=matpath+'/Data/ScarImages/MetaImages'
Conv_path=root+'/Convertion_Process'
Scar_path=root+'/Scar_Process/Data'

#moving and deleting files before starting surface generation
move(Conv_src,Conv_path)
move(Scar_src,Scar_path)
remove(['Aligned','Seg'],matpath)

#generating and moving heart surfaces
print('Making surfaces')
run(Conv_path,'make_surface_all.sh')
print('Moving files to Surfaces')
for i in ['plyFiles','vtkFiles','txtFiles']:
	os.chdir(Conv_path+'/Data/'+i)
	os.mkdir(newdir+'/'+i)
	move(os.getcwd(),newdir+'/'+i)

#generating and moving scar surfaces
print('Making scar surfaces')
run(root+'/Scar_Process','run.sh')
print('Moving scar files to Surfaces')
src=os.listdir(os.getcwd()+'/Data')
for f in src:
	if f.endswith('.vtk'):
		shutil.move(os.getcwd()+'/Data/'+f,newdir+'/vtkFiles/')
	else:
		os.remove(os.getcwd()+'/Data/'+f)


print('All .vtk files are stored in Surfaces/{}/vtkFiles'.format(Date))
print('##################################')
print('###PART 2 DONE: MAKING SURFACES###')
print('##################################')


"""
PART 3: GENERATION OF FEM FILES
"""
Programs_path=os.getenv('HOME')+'/Programs'	#path to Programs

#Generating .msh files from .vtk files
def mergevtk(i,msh_src,vtk_src):	
	lv_endo='{}/Patient_{}-LVEndo-Frame_1.vtk'.format(vtk_src,i)
	rv_endo='{}/Patient_{}-RVEndo-Frame_1.vtk'.format(vtk_src,i)
	rv_epi='{}/Patient_{}-RVEpi-Frame_1.vtk'.format(vtk_src,i)
	msh='{}/Patient_{}.msh'.format(msh_src,i)
	out='{}/Patient_{}.out.txt'.format(msh_src,i)
	biv_mesh='{}/scripts/biv_mesh.geo'.format(root)
	gmsh='{}/gmsh/build/gmsh'.format(Programs_path) 	#path to gmsh
	os.system('{} -3 {} -merge {} {} {} -o {} >& {}'.format(
		gmsh, lv_endo, rv_endo, rv_epi, biv_mesh, msh, out))

#help function to write_fem for writing .pts files
def write_pts(start_pts, end_pts, words, outfile_pts):
	if words[0]=='$Nodes':
		start_pts=True
		return
	if words[0]=='$EndNodes':
		end_pts=True
	if start_pts==True and end_pts==False:
		if len(words)==1:	#first line to write
			outfile_pts.write('{}\n'.format(words[0]))
		else:
			outfile_pts.write('{} {} {}\n'.format(words[1],words[2],words[3]))

#help function to write_fem for writing .elem and .tris files
def write_elem(start_elem,end_elem,words,outfile_elem,outfile_tris):
	if words[0]=='$Elements':
		start_elem=True
		return 	#jumping to next line
	if words[0]=='$EndElements':
		end_elem=True
	if start_elem==True and end_elem==False:
		if len(words)==1:	#first line to write
			outfile_elem.write('{}\n'.format(words[0]))	#writing up nr of elements
		elif len(words)>7:
			i=int(words[5])-1
			j=int(words[6])-1
			k=int(words[7])-1
			if words[1]=='2':
				outfile_tris.write('{} {} {} 1\n'.format(i,j,k))
			elif words[1]=='4':
				l=int(words[8])-1
				outfile_elem.write('Tt {} {} {} {} 1\n'.format(i,j,k,l))

#function for generating pts, elem and tris files from msh files
def write_fem(input_file,outputname):
	infile=open(input_file,'r')
	outfile_pts=open(outputname+'.pts','w')
	outfile_elem=open(outputname+'.elem','w')
	outfile_tris=open(outputname+'.tris','w')
	start_pts=False
	end_pts=False
	start_elem=False
	end_elem=False
	for line in infile:
		words=line.split()
		#pts part
		write_pts(start_pts,end_pts,words,outfile_pts)
		#elem and tris part
		write_elem(start_elem,end_elem,words,outfile_elem,outfile_tris)
	infile.close()
	outfile_pts.close()
	outfile_elem.close()
	outfile_tris.close()

#Adjusting and moving files to each patient folder
def write_files(patient_path,i):
	shutil.copyfile('{}/scripts/stim_coord.dat'.format(root),patient_path+
		'/stim_coord.dat')
	shutil.copyfile('{}/scripts/base.par'.format(root),patient_path+'/base.par')
	infile=open('{}/scripts/risk_strat_1_16.sh'.format(root),'r').readlines()
	outfile=open(patient_path+'/risk_strat_1_16.sh','w')
	new_jobid='#SBATCH --job-name=Pat_{}'.format(i)
	for line in infile:
		if line != infile[2]:
			outfile.write(line)
		else:
			outfile.write(new_jobid+'\n')	#changes jobid to current patient
	outfile.close()

vtk_src='{}/Surfaces/Data-{}/vtkFiles'.format(root,time) #source .vtk files
msh_src='Surfaces/Data-{}/mshFiles'.format(time) 
fem_src='FEM/Data-{}'.format(time)
os.mkdir(root+'/'+msh_src)	#storing msh & msh output files here
os.mkdir(root+'/'+fem_src)	#storing pts, elem & tris files here

for i in range(1,N+1):
	if os.path.isfile('{}/{}/Patient_{}_scar.vtk'.format(vtk_src,i)):	#patient exists
		mergevtk(i,'{}/{}'.format(root,msh_src),vtk_src)	#generation of .msh files
		print('Generated .msh file for Patient {}.'.format(i))

	#generating pts, tris & elem files from msh files
	write_fem('{}/{}/Patient_{}.msh'.format(root,msh_src,i),'Patient_{}'.format(i))
	print('Generated .tris, .elem & .pts file for Patient {}.'.format(i))

	#moving FEM files to correct folder
	patient_path='{}/{}/Patient_{}'.format(root,fem_src,i)
	os.mkdir(patient_path)
	for j in ['tris', 'elem', 'pts']:
		os.rename('Patient_{}.{}'.format(i,j), '{}/Patient_{}.{}'.format(patient_path,i,j))

	#creating stim_coord.dat & rist_strat_1_16.sh in each patient folder
	write_files(patient_path,i)

print('All .msh and .out.txt files are stored in Surfaces/mshFiles')
print('All FEM files are stored in FEM/Data-{}'.format(time))
print('########################################')
print('###PART 3 DONE: GENERATED FEM MODELS####')
print('########################################')
