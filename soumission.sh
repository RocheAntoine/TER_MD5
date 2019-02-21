#!/bin/bash

generationMot=false

tailleMotMin=1
tailleMotMax=10
nMots=10

nbNoeudsMin=8
nbNoeudsMax=8


heure=`date '+%Y-%m-%d_%H:%M:%S'`

if [[ "$generationMot" = true ]]
then
    > mots.txt
    for(( i=tailleMotMin;i<=tailleMotMax;i++ ))
    do
    mot=`cat /dev/urandom | tr -dc 'a-zA-Z0-9+\-\*/=()><%$@#!&\.\;?,_' | fold -w $i | head -n $nMots >> mots.txt`
    done
fi

#echo -e "\nTest avec $nbNoeuds noeuds $heure" >> resultats/res.txt

for (( nbNoeuds=nbNoeudsMin ; nbNoeuds<=nbNoeudsMax ; nbNoeuds++ ))
do
    nbProc=$(echo "$nbNoeuds*28"|bc)
    sed -i 's/#SBATCH -N.*/#SBATCH -N '$nbNoeuds'/' jobs/job*.sh
    sed -i 's/#SBATCH -n.*/#SBATCH -n '$nbNoeuds'/' jobs/job*.sh
    sed -i 's/#SBATCH -n.*/#SBATCH -n '$nbProc'/' jobs/jobMPI.sh
    sed -i 's/fichierRes=.*/fichierRes=resultats\/res_'$heure'.txt/' jobs/job*.sh
    sed -i 's/nbNoeuds=.*/nbNoeuds='$nbNoeuds'/' jobs/job*.sh
    #    #sbatch jobs/jobMPI.sh
    sbatch jobs/jobOMPI.sh
    #    #sbatch jobs/jobMPC.sh
    #    #sbatch jobs/jobICC.sh
done
