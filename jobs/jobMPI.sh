#!/bin/bash

#SBATCH --comment "TER_MD5"
#SBATCH -J "MD5_MPI"
#SBATCH --error=logs/job.%J.err
#SBATCH --output=logs/job.%J.out
#SBATCH -p short
#SBATCH --time=24:00:00
#SBATCH --mem 50000
#SBATCH -n 28
#SBATCH -N 1
#SBATCH --mail-user=<shurizaidu02300@gmail.com>
#SBATCH --mail-type=ALL,TIME_LIMIT_80

# Definitions des parametres

prefixe=2

module load intel/2018.3
module load openmpi/2.0.4.1.1_icc

somme=0
for mot in `cat mots.txt`
do
    res=`srun src/MPI/main $mot $prefixe | tail -n 1`
    echo -e "MPI \t$res\t$mot"  >> resultats/res.txt
    res=`echo $res | cut -d ' ' -f1`
    somme=`bc -l <<< "$res+$somme"`
done
echo "MPI : Temps total : $somme" >> resultats/res.txt