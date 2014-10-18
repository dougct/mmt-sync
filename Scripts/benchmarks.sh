#!/bin/bash

# This script contains calls for the functions that run the benchmarks in
# PARSEC and SPLASH. Basically, we have to create a custom call to run the
# benchmark and then run it under Pin.

#-----------------
# Helper functions
#-----------------

# A whitespace separated list with the name of the benchmarks we will execute.
bench_list='canneal'


function mv_zero {
    if [[ -e instru0.txt ]]; then
        mv instru0.txt 'instru'$1'.txt'
    fi
}


# In the functions below, `command` is the command line to run the benchmark.
# After running it, we run it again under Pin. The Pin tool requires the
# mangled name of the function that is the entry point of the threads.
# There can be more than one of these functions in the program, but we
# only test one of them. You can find the name of this function by looking
# at the source code for the function being passed as a parameter to pthread_create,
# and then use objdump -t func-name | grep func-name.

#------------------
# PARSEC Benchmarks
#------------------


function blackscholes {
    command='/home/douglas/parsec-3.0/pkgs/apps/blackscholes/src/blackscholes '$1' /home/douglas/parsec-3.0/pkgs/apps/blackscholes/inputs/in_4K.txt prices.txt'
    exec_pin _Z9bs_threadPv "$command"
}

function bodytrack {
    command= '/home/douglas/parsec-3.0/pkgs/apps/bodytrack/inst/amd64-linux.gcc/bin/bodytrack /home/douglas/parsec-3.0/pkgs/apps/bodytrack/inputs/sequenceB_1 4 1 1000 5 0 2'
    exec_pin thread_entry "$command"
}

function swaptions {
    command='/home/douglas/parsec-3.0/pkgs/apps/swaptions/src/swaptions -ns 16 -sm 5000 -nt '$1
    exec_pin _Z6workerPv "$command"
}

function fluidanimate {
    command='/home/douglas/parsec-3.0/pkgs/apps/fluidanimate/src/fluidanimate '$1' 5 /home/douglas/parsec-3.0/pkgs/apps/fluidanimate/inputs/in_35K.fluid /home/douglas/parsec-3.0/pkgs/apps/fluidanimate/inputs/out.fluid'
    exec_pin _Z15AdvanceFramesMTPv "$command"
}

function canneal {
    command='/home/douglas/parsec-3.0/pkgs/kernels/canneal/src/canneal '$1' 5 100 /home/douglas/parsec-3.0/pkgs/kernels/canneal/inputs/10.nets 1'
    exec_pin _ZN15annealer_thread3RunEv "$command"
}


#------------------
# SPLASH Benchmarks
#------------------

function fft {
    comando='/home/douglas/parsec-3.0/ext/splash2/kernels/fft/src/fft -p'$1' -m20 -n131072 -l8'
    exec_pin SlaveStart "$comando"
    mv_zero $1
}

