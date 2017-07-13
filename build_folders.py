
'''
Purpose of this script
1) Git doesn't allow empty folders. Need to create them.
2) Need to build and compile some programs
'''
import os
import sys


def change_gmsh(path):
	infile=open('mat2fem.py','r').readlines()
	outfile=open('new_mat2fem.py','w')
	for line in infile:
		if str(line[1:5])=='gmsh':
			outfile.write("	gmsh='{}'\n".format(path))
		else:
			outfile.write(line)
	outfile.close()
	os.remove('mat2fem.py')
	os.rename('new_mat2fem.py','mat2fem.py')

if eval(sys.argv[1]=='empty'):
	print('gmsh installed by software.sh. Path to build is known.')
else:
	print('gmsh path specified by user. Changing mat2fem.py to correct gmsh path.')
	change_gmsh(str(sys.argv[1]))

def exists_folders(path):	#checking if folders exist
	if not os.path.isdir(path):
		os.makedirs(path)

root=os.getcwd() #In_Silico_Heart_Models
seg='seg'
Surfaces='Surfaces'
FEM='FEM'
Conv_build='Convertion_Process/ConvertFile/build'
Conv_Data='Convertion_Process/Data/'
Matlab_Data='Matlab_Process/Data'
Matlab_align=Matlab_Data+'/Aligned'
Matlab_scar=Matlab_Data+'/ScarImages'
Matlab_seg=Matlab_Data+'/Seg'
Matlab_text=Matlab_Data+'/Texts'
Scar_data='Scar_Process/Data'
Scar_build='Scar_Process/ScarProcessing/build'
Scar_meta=Scar_build+'/MetaImages'

folders=[seg, Surfaces, Conv_build, Conv_Data,
Matlab_Data, Matlab_align, Matlab_scar,Matlab_seg, 
Matlab_text, Scar_data, Scar_build, FEM, Scar_meta]

#creates the folders above
for path in folders:
	exists_folders(root+'/'+path)
	#os.chdir(root)

#need to build in both Scar and Convertion Process
os.chdir(root+'/'+Scar_build)
os.system('cmake ..')
os.system('make')

os.chdir(root+'/'+Conv_build)
os.system('cmake ..')
os.system('make')


#need to compile C program from .msh to .elem and .pts
os.chdir(root)
os.system('gcc msh2carp.c -o msh2carp.out')