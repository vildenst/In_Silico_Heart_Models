import os
import shutil 

root=os.getcwd()	#path to In_Silico_Heart_Models

#PART 1: MATLAB SLICE ALIGNMENT

source ='{}/seg/'.format(root)
files=os.listdir(source)
N=len(files)	#number of .mat files to be processed
nr_errors=0
err_list=[]
dest='{}/Matlab_Process/Data/Seg/'.format(root)
for f in files:
	shutil.copy(source+f,dest+f)	#all .mat files moved to dest

os.chdir('Matlab_Process')

def run_matlab(nr_errors,err_list):
	os.system('sh run_matlab.sh')	#running the matlab script
	scar_folder ='{}/Matlab_Process/Data/ScarImages/MetaImages/'.format(root)
	scar_files=os.listdir(scar_folder)
	N2=len(scar_files)
	if (N-nr_errors)*2 > N2:	#not all .mat files are processed
		remove_error(N2,nr_errors,err_list)

def remove_error(N2,nr_errors,err_list):
		error=(N2+nr_errors*2)/2+1
		err_path='{}Patient_{}.mat'.format(dest,error)
		os.remove(err_path)		
		nr_errors+=1
		err_list.append(error)
		run_matlab(nr_errors,err_list)

def remove_error_files(fname):
	rm_path='{}/Data/Texts/'.format(os.getcwd())
	for i in ['LVEndo','LVEpi','RVEndo','RVEpi']:
		if os.path.isfile('{}{}-{}-Frame_1.txt'.format(rm_path,fname,i)):
			os.remove('{}{}-{}-Frame_1.txt'.format(rm_path,fname,i))

run_matlab(nr_errors,err_list)
if err_list:	#errors occured
	for nr in err_list:
		fname='Patient_{}'.format(nr)
		remove_error_files(fname)
		print('{} was removed due to errors. Will continue without it.'.format(fname))

#PART 2: SURFACE GENERATION
import datetime
now=datetime.datetime.now()
time = now.strftime("%d.%m-%H.%M")
os.chdir('..')
os.system('sh Process.sh {}'.format(time))	#running bash script

print('All .vtk files are stored in Surfaces/vtkFiles')


print('####################################')
print('###PART 3: GENERATING FEM MODELS####')
print('####################################')

Programs_path=os.getenv('HOME')+'/Programs'	#path to Programs
#Generating .msh files from .vtk files
def mergevtk(i,msh_src):	
	lv_endo='{}/Patient_{}-LVEndo-Frame_1.vtk'.format(src,i)
	rv_endo='{}/Patient_{}-RVEndo-Frame_1.vtk'.format(src,i)
	rv_epi='{}/Patient_{}-RVEpi-Frame_1.vtk'.format(src,i)
	msh='{}/Patient_{}.msh'.format(msh_src,i)
	out='{}/Patient_{}.out.txt'.format(msh_src,i)
	gmsh='{}/gmsh/build/gmsh'.format(Programs_path) 	#path to gmsh
	os.system('{} -3 {} -merge {} {} biv_mesh.geo -o {} >& {}'.format(gmsh, lv_endo, rv_endo, rv_epi, msh, out))

src='Surfaces/Data-{}/vtkFiles'.format(time)
msh_src='Surfaces/Data-{}/mshFiles'.format(time)
fem_src='FEM/Data-{}'.format(time)
os.mkdir(msh_src)	#storing msh & msh output files here
os.mkdir(fem_src)	#storing pts, elem & tris files here

def write_files(patient_path,i):
	shutil.copyfile('stim_coord.dat',patient_path+'/stim_coord.dat')
	infile=open('risk_strat_1_16.sh','r').readlines()
	outfile=open(patient_path+'/risk_strat_1_16.sh','w')
	new_jobid='#SBATCH --job-name=Pat_{}'.format(i)
	for line in infile:
		if line != infile[2]:
			outfile.write(line)
		else:
			outfile.write(new_jobid+'\n')	#changes jobid to current patient
	outfile.close()


for i in range(1,N+1):
	if os.path.isfile('{}/Patient_{}_scar.vtk'.format(src,i)):	#patient exists
		mergevtk(i,msh_src)
		print('Generated .msh file for Patient {}.'.format(i))

	#generating pts, tris & elem files from msh files
	os.system('./msh2carp.out {}/Patient_{}.msh Patient_{}'.format(msh_src,i,i))
	print('Generated .tris, .elem & .pts file for Patient {}.'.format(i))

	#moving FEM files to correct folder
	patient_path='{}/Patient_{}'.format(fem_src,i)
	os.mkdir(patient_path)
	for j in ['tris', 'elem', 'pts']:
		os.rename('Patient_{}.{}'.format(i,j), '{}/Patient_{}.{}'.format(patient_path,i,j))

	#creating stim_coord.dat & rist_strat_1_16.sh in each patient folder
	write_files(patient_path,i)

print('All .msh and .out.txt files are stored in Surfaces/mshFiles')
print('All FEM files are stored in FEM/Data-{}'.format(time))