#!/bin/bash

#PBS -N run
#PBS -o run.out
#PBS -e run.err
#PBS -l nodes=sandman:ppn=64
#PBS -l walltime=00:30:00

cd /home/parallel/parlab02/pps/lab3/z2
for lock in nosync_lock pthread_lock tas_lock ttas_lock array_lock clh_lock; do
    echo Lock: $lock
    for nthreads in 1 2 4 8 16 32 64; do
        for list_size in 16 1024 8192; do
            echo List Size: $list_size
            export MT_CONF=$(seq -s , 0 $(($nthreads - 1)))
            ./$lock $list_size
        done
    done
done
