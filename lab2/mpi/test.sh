#!/bin/bash

cd ../serial
make
cd ../mpi
make

for P in Jacobi GaussSeidelSOR RedBlackSOR
do
    for X in 16
    do
        for Y in 16
        do
            ../serial/${P,,} $X $Y
            for Px in 4
            do
                for Py in 4
                do
                    mpirun -np $(($Px * $Py)) ${P,,} $X $Y $Px $Py
                    diff res${P}Naive_${X}x$Y res${P}MPI_${X}x${Y}_${Px}x$Py
                done
            done
        done
    done
done

make clean
rm -f res*
cd ../serial
make clean
