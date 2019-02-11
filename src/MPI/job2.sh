#!/bin/bash

#SBATCH --comment "CHPS0711"
#SBATCH -J "MD5_MPI"
#SBATCH --error=logs/job.%J.err
#SBATCH --output=logs/job.%J.out
#SBATCH -p short
#SBATCH --time=05:00:00
#SBATCH --mem 50000
#SBATCH -n 3
#SBATCH -N 1

module load intel/2018.3
module load openmpi/2.0.4.1.1_icc
echo "Temps MPI" > resMPI.txt
echo "" >> resMPI.txt
for i in 2 3
do
    srun -n $i princ toute 2 m | tail -n 1 >> resMPI.txt
done