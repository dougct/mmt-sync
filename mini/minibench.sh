#!/bin/bash

#-----------------------------------------------
#BENCHMARKS
#-----------------------------------------------

MINI=$HOME'/mmt-sync/mini'
#MINI=$HOME'/projects/mmt-sync/mini'

BENCHMARKS=$MINI/exe
BENCH_BIN='?'

testes=`find $MINI/src -maxdepth 1 -type f -iname "*.cpp" -not -iname "*.hpp" -printf '%f '`
bench_list=`
for x in \`echo $testes\`; do
    basename $x .cpp
done | sort`

function start_bench {
    comando=$BENCH_BIN/$1' '$2
    #comando=$BENCH_BIN/$1' '$2' '4
    exec_pin execute_tests "$comando"
}

