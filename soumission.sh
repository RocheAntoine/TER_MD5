#!/bin/bash

tailleMot=4
nMots=3

nbNoeuds=10
nbProc=$(echo "$nbNoeuds*28"|bc)
#rm mots.txt
#for(( i=0;i<nMots;i++ ))
#do
mot=`cat /dev/urandom | tr -dc 'a-zA-Z0-9+\-\*/=()><%$@#!&\.\;?,_' | fold -w $tailleMot | head -n $nMots > mots.txt`
#done

heure=`date '+%Y-%m-%d %H:%M:%S'`
echo -e "\nTest avec $nbNoeuds noeuds $heure" >> resultats/res.txt

sed -i 's/#SBATCH -N.*/#SBATCH -N '$nbNoeuds'/' jobs/job*.sh
sed -i 's/#SBATCH -n.*/#SBATCH -n '$nbNoeuds'/' jobs/job*.sh
sed -i 's/#SBATCH -n.*/#SBATCH -n '$nbProc'/' jobs/jobMPI.sh

#sbatch jobs/jobMPI.sh
#sbatch jobs/jobOMPI.sh
#sbatch jobs/jobMPC.sh
#sbatch jobs/jobICC.sh
