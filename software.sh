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

#creates Program folder if it doesn't exist
if [ ! -d "Vilde" ]; then
	mkdir Programs
fi 

module list