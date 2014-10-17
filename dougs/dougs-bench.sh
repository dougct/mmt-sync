#!/bin/bash

#-----------------------------------------------
#BENCHMARKS
#-----------------------------------------------

DOUGS=$HOME'/mmt-sync/dougs'
#DOUGS=$HOME'/projects/mmt-sync/dougs'

BENCHMARKS=$DOUGS/exe
BENCH_BIN='/home/douglas/mmt-sync/dougs/exe/simple'

testes=`find $DOUGS/src -maxdepth 1 -type f -iname "*.cpp" -not -iname "*.hpp" -printf '%f '`
bench_list=`
for x in \`echo $testes\`; do
    basename $x .cpp
done | sort`

function start_bench {
    comando=$BENCH_BIN/$1' '$2
    #comando=$BENCH_BIN/$1' '$2' '4
    exec_pin execute_tests "$comando"
}

