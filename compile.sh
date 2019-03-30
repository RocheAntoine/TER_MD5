#!/bin/bash
module unload mpc/3.2.1
module load intel/2018.3
module load openmpi/2.0.4.1.1_icc


icc=false
mpi=true
mpc=false
ompi=false

cd src

if [ "$icc" = true ]; then
    cd ICC
    make clean
    make -j
    cd ../
fi

if [ "$mpi" = true ]; then
cd MPI
make clean
make -j
cd ../
fi

if [ "$ompi" = true ]; then
cd OMPI
make clean
make -j
cd ../
fi

if [ "$mpc" = true ]; then
module load mpc/3.2.1
cd MPC
make clean
make -j
fi
