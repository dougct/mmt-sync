#!/bin/bash

#-----------------------------------------------
#DIRECTORIES
#-----------------------------------------------
MAIN_FOLDER=$HOME'/mmt-sync'
#MAIN_FOLDER=$HOME'/projects/mmt-sync'
PIN_DIR=$HOME'/pin'
#PIN_DIR=$HOME'/usr/pin'

#-----------------------------------------------
#PIN
#-----------------------------------------------

PINBIN=$PIN_DIR'/intel64/bin/pinbin -injection child'
#PINBIN=$PIN_DIR'/intel64/bin/pinbin'
export LD_LIBRARY_PATH=$PIN_DIR'/intel64/lib-ext/'

#PINBIN=$PIN_DIR'/ia32/bin/pinbin'
#export LD_LIBRARY_PATH=$PIN_DIR'/ia32/lib-ext/'

function exec_pin {
    TOOL1=$MAIN_FOLDER'/Tools/obj-intel64/insList.so'
    $PINBIN -t $TOOL1 -o instru -e $1 -s false -f false -- $2
}

#-----------------------------------------------
#HEURISTICS
#-----------------------------------------------
HEURISTIC=$MAIN_FOLDER'/Heuristics/bin'
HEURPARAM=$MAIN_FOLDER'/Heuristics/parameters'
HEURINFO=$MAIN_FOLDER'/Heuristics/info'
HEURISTIC_MAIN=$MAIN_FOLDER'/Execute/heuristic'
HMODE='1'

heuristics=`find $HEURISTIC -iname *.so -printf '%f '`
heuristics=`
for x in \`echo $heuristics\`; do
    echo \`basename $x .so\`
done | sort`

heuristics='MinSP_PC'

#-----------------------------------------------
#HELPFUL
#-----------------------------------------------
#bintypes='HardwareOnly FunctionLevel PostDom Structured LongestPath'
bintypes='HardwareOnly FunctionLevel LongestPath'

function lista_files {
    lista=''
    i=$1
    while [ $i -le $2 ]
    do
        lista=$lista'instru'$i'.txt '
        i=$(( $i + 1 ))
    done
    echo $lista
}

#-----------------------------------------------
#BENCHMARKS
#-----------------------------------------------

BENCH_BIN='?'

source $MAIN_FOLDER/'Scripts/benchmarks.sh'
#source $MAIN_FOLDER/'mini/minibench.sh'

#source $MAIN_FOLDER/'dougs/dougs-bench.sh'
#BENCH_BIN='/home/douglas/mmt-sync/dougs/exe/simple'
#-----------------------------------------------
#MAIN EXECUTION
#-----------------------------------------------
set -e

OUTPUT='saida'$HMODE'.txt'

echo -n '' > $OUTPUT

x=2
# Change it back to 16
while [ $x -le 16 ]; do
    echo $x' threads' >> $OUTPUT
    
    for teste in $bench_list; do
        echo $teste >> $OUTPUT
        
        #For each category of heuristic
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
            
            #Execute the benchmark
            start_bench $teste $x
            
            #Execute the heuristics
            simult=8
            y=0
            for heuristic in $heuristics; do
                
                heur_bintype=`cat $HEURINFO/$heuristic'.txt'`
                if [[ $heur_bintype != $cur_bintypes ]]; then
                    continue
                fi
                
                if [[ -e $HEURPARAM/$heuristic'.txt'  ]]; then
                    while read param; do
                        $HEURISTIC_MAIN $HMODE 0 $HEURISTIC/$heuristic.so "$param" $(lista_files 1 $x ) > 'tempout'$heuristic"$param"'.txt' &
                        
                        y=$(( $y + 1 ))
                        if [[ $y -eq $simult ]]; then
                            wait
                            y=0
                        fi
                    done < $HEURPARAM/$heuristic'.txt'
                else
                    $HEURISTIC_MAIN $HMODE 0 $HEURISTIC/$heuristic.so '1' $(lista_files 1 $x ) > 'tempout'$heuristic'.txt' &
                    
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

mkdir -p saida
$MAIN_FOLDER/Scripts/analisar $OUTPUT saida

