#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=8:ppn=8
#PBS -l walltime=00:10:00

module load openmpi/1.8.3
cd /home/parallel/parlab02/pps/lab2/mpi
for X in 2048 4096 6144
do
	for exe in jacobi gaussseidelsor redblacksor
	do
		for P in '1 1' '2 1' '2 2' '4 2' '4 4' '8 4' '8 8'
		do
            read -a P <<< ${P}
			mpirun -np $((${P[0]} * ${P[1]})) --map-by node --mca btl self,tcp ${exe} ${X} ${X} ${P[0]} ${P[1]}
		done
	done
done
