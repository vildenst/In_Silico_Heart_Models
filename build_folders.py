
'''
Purpose of this script
1) Git doesn't allow empty folders. Need to create them.
2) Need to build and compile some programs
'''
import os

def exists_folders(path):	#checking if folders exist
	if not os.path.isdir(path):
		ps.makedirs(path)

root=os.getcwd() #In_Silico_Heart_Models
seg='/seg'
Surfaces='/Surfaces'
FEM='/FEM'
Conv_build='/Conversion_Process/ConvertFile/build'
Conv_Data='/Conversion_Process/Data'
Matlab_Data='/Matlab_Process/Data'
Matlab_align=Matlab_Data+'/Aligned'
Matlab_scar=Matlab_Data+'/ScarImages'
Matlab_seg=Matlab_Data+'/Seg'
Matlab_text=Matlab_Data+'/Texts'
Scar_data='/Scar_Process/Data'
Scar_build='/Scar_Process/ScarProcessing/build'

folders=[seg, Surfaces, Conv_build, Conv_Data,
Matlab_Data, Matlab_align, Matlab_scar,Matlab_seg, 
Matlab_text, Scar_data, Scar_build, FEM]

for path in folders:
	exists_folders(path)
	#os.chdir(root)

#need to build in both Scar and Convertion Process
for build in [Conv_build, Scar_build]:
	os.system(build'/cmake ..')
	os.system(build'/make')

#need to compile C program from .msh to .elem and .pts
os.system('gcc msh2carp.c -o msh2carp.out')