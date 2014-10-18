#!/bin/bash

# This script is used to run the benchmarks, call the Pin toll, and run the heuristics.
# It runs the benchmarks, the run them under Pin, which produces files named 'instru_.txt'.
# These text files contain the traces from the execution of the benchmarks. Our heuristics
# take these files as input the simulate the execution of the benchmarks the MMT architecture.

# Call a benchmark.
function start_bench {
    $1 $2
}

#------------
# DIRECTORIES
#------------
MAIN_FOLDER=$HOME'/mmt-sync'
PIN_DIR=$HOME'/pin'


#--------------
# PIN Variables
#--------------

PINBIN=$PIN_DIR'/intel64/bin/pinbin -injection child'
export LD_LIBRARY_PATH=$PIN_DIR'/intel64/lib-ext/'

# Call the Pin tool passing as a parameter the file with the traces ('instru').
function exec_pin {
    TOOL1=$MAIN_FOLDER'/Tools/obj-intel64/insList.so'
    $PINBIN -t $TOOL1 -o instru -e $1 -s false -f false -- $2
}

#-----------
# HEURISTICS
#-----------
HEURISTIC=$MAIN_FOLDER'/Heuristics/bin'
HEURPARAM=$MAIN_FOLDER'/Heuristics/parameters'
HEURINFO=$MAIN_FOLDER'/Heuristics/info'
HEURISTIC_MAIN=$MAIN_FOLDER'/Execute/heuristic'
HMODE='1'

# Search the names of the heuristics we want to compare.
heuristics=`find $HEURISTIC -iname *.so -printf '%f '`
heuristics=`
for x in \`echo $heuristics\`; do
    echo \`basename $x .so\`
done | sort`


#----------------------------------
# Helper functions and definitions.
#----------------------------------

# Types of heuristics.
bintypes='HardwareOnly FunctionLevel LongestPath'

# Get the list of files that contain the execution traces.
function files_list {
    list=''
    i=$1
    while [ $i -le $2 ]
    do
        list=$list'instru'$i'.txt '
        i=$(( $i + 1 ))
    done
    echo $list
}

#-----------
# Benchmarks
#-----------

BENCH_BIN='?'

# This variable should be set according to the benchmarks we want to run.
# That is, if we want to run PARSEC & SPLASH, we should 'source' benchmarks.sh,
# which contains calls to the benchmarks in PARSEC & SPLASH. And if we want to
# run our toy benchmarks, then we should 'source' the script in the folder 'mini',
# which contains calls to the toy benchmarks.
# TODO: make this a parameter to be set when calling the script.
source $MAIN_FOLDER/'Scripts/benchmarks.sh'
#source $MAIN_FOLDER/'mini/minibench.sh'
#source $MAIN_FOLDER/'dougs/dougs-bench.sh'

#--------------------
# Main Exection Logic
#--------------------
set -e

OUTPUT='output_tables'$HMODE'.txt'

echo -n '' > $OUTPUT

x=2
# The number that controls this loop is the number of threads with which
# we want to execute the benchmarks and heuristics.
while [ $x -le 16 ]; do
    echo $x' threads' >> $OUTPUT
    
    for teste in $bench_list; do
        echo $teste >> $OUTPUT
        
        # For each category of heuristic
        for cur_bintypes in $bintypes; do
            
            if [[ $cur_bintypes == 'HardwareOnly' ]]; then
                BENCH_BIN=$BENCHMARKS'/simple'
            elif [[ $cur_bintypes == 'FunctionLevel' ]]; then
                BENCH_BIN=$BENCHMARKS'/simple'
            elif [[ $cur_bintypes == 'LongestPath' ]]; then
                BENCH_BIN=$BENCHMARKS'/longpath'
            elif [[ $cur_bintypes == 'PostDom' ]]; then
                BENCH_BIN=$BENCHMARKS'/postdom'
            elif [[ $cur_bintypes == 'Structured' ]]; then
                BENCH_BIN=$BENCHMARKS'/structured'
            fi
            
            # Execute the benchmark
            start_bench $teste $x
            
            # Execute the heuristics
            simult=8
            y=0
            for heuristic in $heuristics; do
                
                heur_bintype=`cat $HEURINFO/$heuristic'.txt'`
                if [[ $heur_bintype != $cur_bintypes ]]; then
                    continue
                fi
                
                if [[ -e $HEURPARAM/$heuristic'.txt'  ]]; then
                    while read param; do
                        $HEURISTIC_MAIN $HMODE 0 $HEURISTIC/$heuristic.so "$param" $(files_list 1 $x ) > 'tempout'$heuristic"$param"'.txt' &
                        
                        y=$(( $y + 1 ))
                        if [[ $y -eq $simult ]]; then
                            wait
                            y=0
                        fi
                    done < $HEURPARAM/$heuristic'.txt'
                else
                    $HEURISTIC_MAIN $HMODE 0 $HEURISTIC/$heuristic.so '1' $(files_list 1 $x ) > 'tempout'$heuristic'.txt' &
                    
                    y=$(( $y + 1 ))
                    if [[ $y -eq $simult ]]; then
                        wait
                        y=0
                    fi
                fi
                
            done
            if [[ $y -gt 0 ]]; then
                wait
            fi
            
            
            for heuristic in $heuristics; do
                
                heur_bintype=`cat $HEURINFO/$heuristic'.txt'`
                if [[ $heur_bintype != $cur_bintypes ]]; then
                    continue
                fi
                
                if [[ -e $HEURPARAM/$heuristic'.txt'  ]]; then
                    while read param; do
                        echo -n $heuristic' ('$param'): ' >> $OUTPUT
                        cat 'tempout'$heuristic"$param"'.txt' >> $OUTPUT
                    done < $HEURPARAM/$heuristic'.txt'
                else
                    echo -n $heuristic': ' >> $OUTPUT
                    cat 'tempout'$heuristic'.txt' >> $OUTPUT
                fi
                
            done
            rm -f tempout*
            rm -f instru*
        done
        echo '' >> $OUTPUT
        
    done
    
    echo '' >> $OUTPUT
    echo '' >> $OUTPUT
    
    x=$(( $x * 2 ))
done

mkdir -p output_tables
$MAIN_FOLDER/Scripts/analisar $OUTPUT output_tables

