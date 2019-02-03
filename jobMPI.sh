#!/bin/bash

#SBATCH --comment "TER_MPI"
#SBATCH -J "MD5_MPI"
#SBATCH --error=logs/job.%J.err
#SBATCH --output=logs/job.%J.out
#SBATCH -p short
#SBATCH --time=10:00:00
#SBATCH --mem 50000
#SBATCH -n 56
#SBATCH -N 2

# Definitions des parametres

nThreads=56
prefixe=2

module load intel/2018.3
module load openmpi/2.0.4.1.1_icc

for mot in `cat mots.txt`
do
    res=`srun -n $nThreads MPI/mainMPI $mot $prefixe | tail -n 1`
    echo -e "MPI \t$res\t$mot"  >> resultats/res.txt
done
