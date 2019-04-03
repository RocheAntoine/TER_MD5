#!/bin/bash

#SBATCH --comment "TER_MD5"
#SBATCH -J "MD5_OMPI"
#SBATCH --error=logs/job.%J.err
#SBATCH --output=logs/job.%J.out
#SBATCH -p short
#SBATCH --time=24:00:00
#SBATCH --mem 50000
#SBATCH -n 1
#SBATCH -c 28
#SBATCH -N 1
#SBATCH --mail-user=<shurizaidu02300@gmail.com>
#SBATCH --mail-type=ALL,TIME_LIMIT_80

# Definitions des parametres

prefixe=2

module unload mpc/3.2.1
module load intel/2018.3
module load openmpi/2.0.4.1.1_icc

fichierRes=
nbNoeuds=

somme=0
for mot in `cat mots.txt`
do
    res=`srun src/OMPI/main $mot $prefixe | tail -n 1`
    echo "Sortie : $res"
    echo -e "MIX\t$nbNoeuds noeuds\t$res\t$mot"  >> $fichierRes
    res=`echo $res | cut -d ' ' -f1`
    somme=`bc -l <<< "$res+$somme"`
done
echo "OMPI_old : Temps total : $somme" >> $fichierRes
