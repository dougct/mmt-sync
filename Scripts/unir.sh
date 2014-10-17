#!/bin/bash

testes='blackscholes bodytrack swaptions fluidanimate tachyon namd barnes fft fmm ocean_cp ocean_ncp radix volrend water_nsquared'

pasta1=$1
pasta2=$2
dest='.'

for bench in $testes; do
    ./unir $pasta1/$bench'_table.txt' $pasta2/$bench'_table.txt' $dest/$bench'_table.txt'
done
