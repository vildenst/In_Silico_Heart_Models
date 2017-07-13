# !/bin/bash 

#loading cmake for compiling
module purge
module load cmake
module load gcc

root=$PWD 	#saving path to In_Silico_Heart_Models
cd ~		#returning to login folder to install Programs folder

#creates Program folder if it doesn't exist
if [ ! -d "Programs" ]; then
	mkdir Programs
fi 

cd Programs	#chaning to Programs folder to install VTK, ITK & gmsh
Programs_path=$PWD

#moving matlab toolbox to Programs
mv $root/Medical_Image_Processing_Toolbox .
echo "Moved Medical_Image_Processing_Toolbox to" $PWD

#installing necessary python packages
echo "checking if numpy, scipy and matplotlib are installed ..."
module load python2
pip install --user numpy
pip install --user scipy
pip install -U matplotlib --user

#Installing VTK
read -p "Do you want to install VTK (y/n)? " vtkchoice
case "$vtkchoice" in
	y|Y|Yes|yes ) 
	echo "installing VTK ... This will take some time"
	echo "downloading VTK in " $Programs_path
	git clone git://vtk.org/VTK.git
	mkdir VTK-build && cd VTK-build	#cd into build folder
	cmake ..
	make -j10
	echo "building VTK in "$PWD
	vtk_dir=$PWD;;
	n|N|No|no ) 
	echo "Will not install VTK."
	read -p "Please specify the path to your VTK build: " vtk_dir;;
	* ) 
	echo "Invalid answer. Please type y or n"
	exit 1;;
esac

#Installing ITK
read -p "Do you want to install ITK (y/n)? " itkchoice
case "$itkchoice" in
	y|Y|Yes|yes ) 
	echo "installing ITK ... This will take some time"
	echo 'downloading ITK in '$Programs_path
	cd $Programs_path
	git clone https://itk.org/ITK.git
	cd ITK 
	mkdir bin && cd bin
	cmake ../ -DModuleITKVtkGlue=ON
	make -j10
	echo 'building ITK in '$PWD
	itk_dir=$PWD;;
	n|N|No|no ) 
	echo "Will not install ITK."
	read -p "Please specify the path to your ITK build: " itk_dir;;
	* ) 
	echo "Invalid answer. Please type y or n"
	exit 1;;
esac

#Installing gmsh
read -p "Do you want to install gmsh (y/n)? " gmshchoice
case "$gmshchoice" in
	y|Y|Yes|yes ) 
	echo "installing gmsh ... This might take some time"
	echo "downloading gmsh in " $Programs_path;;
	cd $Programs_path
	mv root/gmsh .	#moving gmsh folder into Programs
	mkdir gmsh/build && cd gmsh/build
	echo "building gmsh in "$PWD
	module purge
	module load openmpi.intel/1.8.5
	module load cmake
	cmake ../ -DENABLE_FLTK=0 ..
	make -j10
	gmsh_path='empty'
	n|N|No|no ) 
	echo "Will not install gmsh. "
	read -p "Please specify the path to your gmsh build: " gmsh_path;;
	* ) 
	echo "Invalid answer. Please type y or n"
	exit 1;;
esac

export VTK_DIR=$vtk_dir
export ITK_DIR=$itk_dir

python build_folders.py $gmsh_path