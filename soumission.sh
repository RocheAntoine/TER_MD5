#!/bin/bash

tailleMot=4
nMots=500
#rm mots.txt
#for(( i=0;i<nMots;i++ ))
#do
mot=`cat /dev/urandom | tr -dc 'a-zA-Z0-9+\-\*/=()><%$@#!&\.\;?,_' | fold -w $tailleMot | head -n $nMots > mots.txt`
#done

heure=`date '+%Y-%m-%d %H:%M:%S'`
echo -e "\nTest avec 4 noeuds $heure" >> resultats/res.txt

#sbatch jobMPI.sh
sbatch jobOMPMPI.sh