#!/usr/bin/env bash

nMots=100
tailleMot=3
prefixe=2
mot=`cat /dev/urandom | tr -dc 'a-zA-Z0-9+\-\*/=()><%$@#!&\.\;?,_' | fold -w $tailleMot | head -n $nMots > mots.txt`

for mot in `cat mots.txt`
do
    mpirun -n 4 src/MPI/main $mot 2 &
done