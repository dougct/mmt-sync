#!/bin/bash

#-----------------------------------------------
#BENCHMARKS
#-----------------------------------------------

BENCHMARKS=$HOME'/parsec-3.0/pkgs/apps'
#BENCH_INPUT='/home/douglas/parsec-3.0/pkgs/apps/blackscholes/inputs'
#BENCH_BIN='/home/douglas/parsec-3.0/pkgs/apps/blackscholes/inst/amd64-linux.gcc/bin'

bench_list='canneal'

function start_bench {
    $1 $2
}

NUL='/dev/null'

function mv_zero {
    if [[ -e instru0.txt ]]; then
        mv instru0.txt 'instru'$1'.txt'
    fi
}

#-----------------------------------------------
#PARSEC
#-----------------------------------------------

function blackscholes {
    blackscholes_bin='/home/douglas/parsec-3.0/pkgs/apps/blackscholes/inst/amd64-linux.gcc/bin/blackscholes '
    blackscholes_input='/home/douglas/parsec-3.0/pkgs/apps/blackscholes/inputs/in_4K.txt '
    blackscholes_output=' prices.txt'
    #comando=$blackscholes_bin '4' $blackscholes_input $blackscholes_output 
    comando='/home/douglas/parsec-3.0/pkgs/apps/blackscholes/src/blackscholes '$1' /home/douglas/parsec-3.0/pkgs/apps/blackscholes/inputs/in_4K.txt prices.txt'
#    echo "$comando"
    exec_pin _Z9bs_threadPv "$comando"
}

function bodytrack {
    bodytrack_bin='/home/douglas/parsec-3.0/pkgs/apps/bodytrack/inst/amd64-linux.gcc/bin/bodytrack'
    bodytrack_input='/home/douglas/parsec-3.0/pkgs/apps/bodytrack/inputs/sequenceB_1'
    bodytrack_output=$1
    comando= '/home/douglas/parsec-3.0/pkgs/apps/bodytrack/inst/amd64-linux.gcc/bin/bodytrack /home/douglas/parsec-3.0/pkgs/apps/bodytrack/inputs/sequenceB_1 4 1 1000 5 0 2'
    exec_pin thread_entry "$comando"
}

function swaptions {
    swaptions_bin='/home/douglas/parsec-3.0/pkgs/apps/swaptions/inst/amd64-linux.gcc/bin/swaptions'
    comando='/home/douglas/parsec-3.0/pkgs/apps/swaptions/src/swaptions -ns 16 -sm 5000 -nt '$1
    exec_pin _Z6workerPv "$comando"
}

function fluidanimate {
    fluidanimate_bin='/home/douglas/parsec-3.0/pkgs/apps/fluidanimate/inst/amd64-linux.gcc/bin/fluidanimate'
    fluidanimate_input='/home/douglas/parsec-3.0/pkgs/apps/fluidanimate/inputs/in_35K.fluid'
    fluidanimate_output=$1
    comando='/home/douglas/parsec-3.0/pkgs/apps/fluidanimate/src/fluidanimate '$1' 5 /home/douglas/parsec-3.0/pkgs/apps/fluidanimate/inputs/in_35K.fluid /home/douglas/parsec-3.0/pkgs/apps/fluidanimate/inputs/out.fluid'
    exec_pin _Z15AdvanceFramesMTPv "$comando"
}

function canneal {
    canneal_bin='/home/douglas/parsec-3.0/pkgs/kernels/src/canneal'
    canneal_input='/home/douglas/parsec-3.0/pkgs/kernels/canneal/inputs/10.nets'
    canneal_output=$1
    comando='/home/douglas/parsec-3.0/pkgs/kernels/canneal/src/canneal '$1' 5 100 /home/douglas/parsec-3.0/pkgs/kernels/canneal/inputs/10.nets 1'
    exec_pin _ZN15annealer_thread3RunEv "$comando"
}


