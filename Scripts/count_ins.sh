#!/bin/bash

#-----------------------------------------------
#DIRECTORIES
#-----------------------------------------------
MAIN_FOLDER=$HOME'/mmt-sync'
PIN_DIR=$HOME'/pin'

#-----------------------------------------------
#PIN
#-----------------------------------------------

PINBIN=$PIN_DIR'/intel64/bin/pinbin -injection child'
#PINBIN=$PIN_DIR'/intel64/bin/pinbin'
export LD_LIBRARY_PATH=$PIN_DIR'/intel64/lib-ext/'

#PINBIN=$PIN_DIR'/ia32/bin/pinbin'
#export LD_LIBRARY_PATH=$PIN_DIR'/ia32/lib-ext/'

function exec_pin {
    TOOL1=$MAIN_FOLDER'/Tools/obj-intel64/insCounter.so'
    $PINBIN -t $TOOL1 -o instru -e $1 -s false -- $2
}

#-----------------------------------------------
#BENCHMARKS
#-----------------------------------------------

source $MAIN_FOLDER/'Scripts/benchmarks.sh'
#source $MAIN_FOLDER/'mini/minibench.sh'
#source $MAIN_FOLDER/'dougs/dougs-bench.sh'

#-----------------------------------------------
#MAIN EXECUTION
#-----------------------------------------------

set -e

for teste in $bench_list; do
    echo $teste > $teste.txt
    x=1
    while [ $x -le 16 ]; do
        echo $x' threads' >> $teste.txt
        start_bench $teste $x
        cat instru >> $teste.txt
        rm instru
        echo '' >> $teste.txt
        x=$(( $x * 2 ))
    done
done

