#!/bin/bash

module load intel/2018.3
module load openmpi/2.0.4.1.1_icc

cd ICC
make clean
make -j
cd ../MPI
make clean
make -j