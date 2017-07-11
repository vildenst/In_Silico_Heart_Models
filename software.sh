#!/bin/bash -l
#SBATCH --job-name=SW2
#SBATCH --account=nn9249k
#SBATCH --time=10:00:00
#SBATCH --ntasks=1
#SBATCH --mem-per-cpu=4G

#SBATCH --output=out2.txt
#SBATCH --error=err2.txt

source /cluster/bin/jobsetup

#loading cmake for compiling
module load cmake
module load gcc

root=$PWD 	#saving path to In_Silico_Heart_Models
cd ~		#returning to login folder to install Programs folder

#creates Program folder if it doesn't exist
if [ ! -d "Programs" ]; then
	mkdir Programs
fi 

cd Programs	#chaning to Programs folder to install VTK & ITK
Programs_path=$PWD

#moving matlab toolbox to Programs
mv root/Medical_Image_Processing_Toolbox .

#installing necessary python packages
module load python2
pip install --user numpy
pip install --user scipy
pip install --U matplotlib --user

#Installing VTK
if [ '$1'=true ]; then
	git clone git://vtk.org/VTK.git
	mkdir VTK-build && cd VTK-build	#cd into build folder
	#cmake ..
	#make -j15
fi

#Installing ITK
if [ '$2'=true ]; then
	cd $Programs_path
	git clone https://itk.org/ITK.git
	cd ITK 
	mkdir bin && cd bin
	#cmake ../ -DModuleItkVtkGlue=ON
	#make -j15
fi

#Installing gmsh
if [ '$3'=true ]; then
	cd $Programs_path
fi

export VTK_DIR=$Programs_path/VTK-build
export ITK_DIR=$Programs_path/ITK/bin

python build_folders.py