#!/bin/bash

#SBATCH --comment "TER_MD5"
#SBATCH -J "MD5_MPI"
#SBATCH --error=logs/job.%J.err
#SBATCH --output=logs/job.%J.out
#SBATCH -p short
#SBATCH --time=24:00:00
#SBATCH --mem 50000
#SBATCH -n 84
#SBATCH -N 3
#SBATCH --mail-user=<shurizaidu02300@gmail.com>
#SBATCH --mail-type=ALL,TIME_LIMIT_80

# Definitions des parametres

prefixe=2
fichierRes=resultats/res_2019-03-20_12:45:36.txt
module load intel/2018.3
module load openmpi/2.0.4.1.1_icc

srun src/MPI/main