#!/bin/bash

cd ../serial
make
cd ../mpi
make

for exe in Jacobi GaussSeidelSOR RedBlackSOR
do
    for X in 2048 4096 6144
    do
        ../serial/${exe,,} ${X} ${X}
        for P in '1 1' '2 1' '2 2' '4 2' '4 4' '8 4' '8 8'
        do
            read -a P <<< ${P}
            mpirun -np $((${P[0]} * ${P[1]})) --map-by node ${exe,,} ${X} ${X} ${P[0]} ${P[1]}
            diff -q res${exe}Naive_${X}x${X} res${exe}MPI_${X}x${X}_${P[0]}x${P[1]}
        done
    done
done

make clean
rm -f res*
cd ../serial
make clean
