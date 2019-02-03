#!/bin/bash

#SBATCH --comment "TER_MPI"
#SBATCH -J "MD5_OMPI"
#SBATCH --error=logs/job.%J.err
#SBATCH --output=logs/job.%J.out
#SBATCH -p short
#SBATCH --time=10:00:00
#SBATCH --mem 50000
#SBATCH -n 2
#SBATCH -c 28
#SBATCH -N 2

# Definitions des parametres

prefixe=2

module load intel/2018.3
module load openmpi/2.0.4.1.1_icc

#srun ICC/mainICC $mot $prefixe o

for mot in `cat mots.txt`
do
    res=`srun ICC/mainICC $mot $prefixe o | tail -n 1`
    echo $res
    res=`echo $res | tail -n 1`
    echo -e "MIX \t$res\t$mot"  >> resultats/res.txt
done
