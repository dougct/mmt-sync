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
    TOOL1=$MAIN_FOLDER'/Tools/obj-intel64/insList.so'
    $PINBIN -t $TOOL1 -o instru -e $1 -s false -f true -- $2
}

#-----------------------------------------------
#HEURISTICS
#-----------------------------------------------
HEURISTIC=$MAIN_FOLDER'/Heuristics/bin'
HEURPARAM=$MAIN_FOLDER'/Heuristics/parameters'
HEURINFO=$MAIN_FOLDER'/Heuristics/info'
HEURISTIC_MAIN=$MAIN_FOLDER'/Execute/heuristic'
HMODE='0'

heuristics=`find $HEURISTIC -iname *.so -printf '%f '`
heuristics=`
for x in \`echo $heuristics\`; do
    echo \`basename $x .so\`
done | sort`

#-----------------------------------------------
#HELPFUL
#-----------------------------------------------

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

#-----------------------------------------------
#MAIN EXECUTION
#-----------------------------------------------
set -e

#x=1
x=2
while [ $x -le 16 ]; do
    
    for teste in $bench_list; do
        
        BENCH_BIN=$BENCHMARKS'/simple'
        start_bench $teste $x
        
        $HEURISTIC_MAIN $HMODE 0 $HEURISTIC/MaxFun_MinPC.so '1' $(lista_files 1 $x ) > $teste'_'$x.txt &
        
        her2_name=' '
        her2_par=' '
        if [[ $teste == 'volrend' ]]; then
            her2_name='Long'
            her2_par='16'
        elif [[ $teste == 'barnes' ]]; then
            her2_name='Long_MinPC'
            her2_par='2'
        elif [[ $teste == 'swaptions' ]]; then
            her2_name='Long_MinPC'
            her2_par='2'
        elif [[ $teste == 'water_nsquared' ]]; then
            her2_name='Distance'
            her2_par='0 0 -1'
        elif [[ $teste == 'fluidanimate' ]]; then
            her2_name='Lee'
            her2_par='0'
        elif [[ $teste == 'fft' ]]; then
            her2_name='Distance'
            her2_par='0 0 -1'
        elif [[ $teste == 'fmm' ]]; then
            her2_name='Long_MinSP_PC'
            her2_par='8'
        fi
        
        if [[ $her2_name != ' ' ]]; then
            $HEURISTIC_MAIN $HMODE 0 $HEURISTIC/$her2_name.so $her2_par $(lista_files 1 $x ) > $teste'_'$x'_2'.txt &
        fi
        
        wait
        
        rm -f instru*

    done
    
    x=$(( $x * 2 ))
done

