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
if [ ! -d "Test_Programs" ]; then
	mkdir Test_Programs
fi 

cd Test_Programs	#chaning to Programs folder to install VTK & ITK
Test_Programs_path=$PWD

#Installing VTK
git clone git://vtk.org/VTK.git
mkdir VTK-build && cd VTK-build	#cd into build folder
#cmake ..
#make -j15
cd $Test_Programs_path

#Installing ITK
git clone https://itk.org/ITK.git
cd ITK 
mkdir bin && cd bin
#cmake ../ -DModuleItkVtkGlue=ON
#make -j15

#Building ConvertFile

#Building ScarProcessing
