#!bin/bash -l
#SBATCH --job-name=SW
#SBATCH --account=nn9249k
#SBATCH --time=10:00:00
#SBATCH --ntasks=8
#SBATCH --mem-per-cpu=4G

#SBATCH --output=out.txt
#SBATCH --error=err.txt

source /cluster/bin/jobsetup

#loading cmake for compiling
module load cmake
module load gcc

#creates Program folder if it doesn't exist
if [ ! -d "Hello" ]; then
	mkdir Programs
fi 
