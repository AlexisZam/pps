#!/bin/bash

for fw in "fw_sr" "fw_tiled"
do
    for B in 1 2 4 8 16 32 64 128 256 512 1024 2048 4096
    do
        sed -i "s/N run.*/N run-$fw-$B/" run.sh
        sed -i "s/o run.*/o run-$fw-$B.out/" run.sh
        sed -i "s/e run.*/e run-$fw-$B.err/" run.sh
        sed -i "s/\.\/.*/.\/$fw \$N $B/" run.sh
        cat run.sh
        # qsub -q serial run.sh
    done
done