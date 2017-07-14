
'''
Purpose of this script
1) Git doesn't allow empty folders. Need to create them.
2) Need to re-write some files
'''
import os
import sys


#function that re-writes gmsh path if user already had gmsh installed
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

#function for checking if a folders exists
def exists_folders(path):	
	if not os.path.isdir(path):
		os.makedirs(path)

#checking if user already had gmsh installed
if str(sys.argv[1])=='empty':
	print('gmsh installed by software.sh. Path to build is known.')
else:
	print('gmsh path specified by user. Changing mat2fem.py to correct gmsh path.')
	change_gmsh(str(sys.argv[1]))



root=os.getcwd() #In_Silico_Heart_Models

#Empty folders that need to get created
seg='seg'
Surfaces='Surfaces'
FEM='FEM'
Conv_Data='Convertion_Process/Data/'
Matlab_Data='Matlab_Process/Data'
Matlab_align=Matlab_Data+'/Aligned'
Matlab_scar=Matlab_Data+'/ScarImages'
Scar_meta=Matlab_scar+'/MetaImages'
Matlab_seg=Matlab_Data+'/Seg'
Matlab_text=Matlab_Data+'/Texts'
Scar_data='Scar_Process/Data'


folders=[seg, Surfaces, Conv_Data, Matlab_Data, 
Matlab_align, Matlab_scar,Matlab_seg, 
Matlab_text, Scar_data, FEM, Scar_meta]

#creates the folders above
for path in folders:
	exists_folders(root+'/'+path)
