#!/bin/bash 

module purge

root=$PWD 	#path to In_Silico_Heart_Models
cd ~		#returning to login folder to install Programs folder

#creates Programs folder if it doesn't exist
if [ ! -d "Programs" ]; then
	mkdir Programs
fi 

#changing to Programs folder to install VTK, ITK & gmsh
cd Programs	
Programs_path=$PWD

#moving matlab toolbox to Programs
mv $root/Medical_Image_Processing_Toolbox .
echo "Moved Medical_Image_Processing_Toolbox to" $PWD

#installing necessary python packages
echo "checking if numpy, scipy and matplotlib are installed ..."
pip install --user numpy
pip install --user scipy
pip install -U matplotlib --user


#loading new modules
module purge
module load cmake
module load gcc

#Installing VTK
read -p "Do you want to install VTK (y/n)? " vtkchoice
case "$vtkchoice" in
	y|Y|Yes|yes ) 
	echo "installing VTK ... This will take some time"
	echo "downloading VTK in " $Programs_path
	mv $root/VTK .
	mkdir VTK-build && cd VTK-build	#cd into build folder
	echo "building VTK in "$PWD
	cmake ../VTK 	#running cmake with path to VTK src folder
	make -j10
	vtk_dir=$PWD;;	#path to VTK build
	n|N|No|no ) 
	echo "Will not install VTK."
	read -p "Please specify the path to your VTK build: " vtk_dir;;	#path to VTK build
	* ) 
	echo "Invalid answer. Please type y or n next time. Shutting down program ..."
	exit 1;;
esac

export VTK_DIR=$vtk_dir 	#setting path to VTK build

#Installing ITK
read -p "Do you want to install ITK (y/n)? " itkchoice
case "$itkchoice" in
	y|Y|Yes|yes ) 
	echo "installing ITK ... This will take some time"
	echo 'downloading ITK in '$Programs_path
	cd $Programs_path
	mv $root/ITK .
	cd ITK 
	mkdir bin && cd bin 	#cd into bin folder 
	echo 'building ITK in '$PWD
	echo "You will now be directed into the cmake interface to enable ITKVtkGlue."
	echo "Please do the following after the interface opens:"
	echo "1) Press c to configure."
	echo "2) Press t to enable advanced options. Then use the arrow keys to scroll"
	echo "down to Module_ITKVtkGlue, and press enter. It should now say ON instead of OFF."
	echo "3) Press c to configure, two times in a row."
	echo "4) Press g to generate."
	read -p "Press enter if understood: " ccmake
	case "$ccmake" in
		* ) 
		ccmake .. 	#need to turn on Glue (connects VTK & ITK)
		make -j10
		itk_dir=$PWD;;	#path to ITK build
	esac;;
	n|N|No|no ) 
	echo "Will not install ITK."
	read -p "Please specify the path to your ITK build: " itk_dir;;	#path to ITK build
	* ) 
	echo "Invalid answer. Please type y or n next time. Shutting down program ..."
	exit 1;;
esac

export ITK_DIR=$itk_dir 	#setting path to ITK build

#Installing gmsh
read -p "Do you want to install gmsh (y/n)? " gmshchoice
case "$gmshchoice" in
	y|Y|Yes|yes ) 
	echo "installing gmsh ... This might take some time"
	echo "downloading gmsh in "$Programs_path
	cd $Programs_path
	mv $root/gmsh .	#moving gmsh folder into Programs
	mkdir gmsh/build && cd gmsh/build 	#cd into build folder
	echo "building gmsh in "$PWD
	module purge						#clean up old modules listed
	module load openmpi.intel/1.8.5 	#need to load new module
	module load cmake 					#reload cmake
	cmake ../ -DENABLE_FLTK=0 .. 		#building gmsh without GUI (need FLTK for that)
	make -j10		
	gmsh_path='empty';; 	#gmsh path already spesified in mat2fem.py, nothing to change
	n|N|No|no ) 
	echo "Will not install gmsh. "
	read -p "Please specify the path to your gmsh build: " gmsh_path;; 	#need users gmsh path
	* ) 
	echo "Invalid answer. Please type y or n next time. Shutting down program ..."
	exit 1;;
esac

#building Convertion_Process
mkdir $root/Convertion_Process/ConvertFile/build
cd $root/Convertion_Process/ConvertFile/build
cmake ..
make

#building Scar_Process
mkdir $root/Scar_Process/ScarProcessing/build
cd $root/Scar_Process/ScarProcessing/build
cmake ..
make
 	
#need to compile C program
cd $root
gcc msh2carp.c -o msh2carp.out

#build_folders.py creates empty folders needed for later, as well as 
#re-writing some files if paths to Software needs correction
#Takes gmsh_path as arg, needed if user has build gmsh somewhere else
module purge
module load python2
python build_folders.py $gmsh_path