mmt-sync
========

Synchronization heuristics for the Minimal Multi-Threading Architecture

# How to use this project

## Requirements

 - Use x86 64-bits operating system.
 - Linux (it was not tested in other OS).
  - If you are using Ubuntu, install a library called libelfg0. It is necessary for linking Pin.
 - GCC or Clang.
 - Download [Intel Pin instrumentation software](http://software.intel.com/en-us/articles/pin-a-dynamic-binary-instrumentation-tool) and unpack it.
   - We used version 2.10. We have not tested the latest versions.
 - Download and compile [LLVM](http://llvm.org/) version 3.3.
   - If you use the latest version of LLVM, make sure you change the names of some hearder files e,g., from "llvm/Instructions.h" to "llvm/IR/Instructions.h", and so on. Our passes' headers are in the old format.
 - Download the [PARSEC](http://parsec.cs.princeton.edu/) benchmark suite.

## Preparation

 - Download or clone the source code from this repository.
 - Compile the _main_ program in the folder _Execute_ by running the command `make`.
 - Compile the heuristics in the folder _Heuristics_ by running the command `make`.
 - In the folder _Tools_, edit the makefile and set the variable PIN_HOME to the folder where Pin is. `Make` the Pin tools.
 - In the folder _Passes_, edit the makefile and set the variable "llvmpath"  to the folder where LLVM is. Compile the LLVM passes by running `make`.


## Compilation of benchmarks

We do not use PARSEC's tool to compile the benchmarks. Instead, we run custom compiler commands, which can be found under Scripts/compilation. You will see that, in order to not have to change the makefiles, we overwrote the `gcc` command and replaced it by a script named _gcc_. To do that, run:

       export PATH=$HOME/mmt-sync/Scripts/compilation':'$PATH

The are custom commands to replace calls to GCC and G++ are all in _mmt-sync/Scripts/compilation_.

### Setting Variables

There are several scripts under _mmt-sync/Scripts/_ and you should set a couple of variables for them to work.

  - `MAIN_FOLDER`: where your framework is located (usually, something like /home/user/mmt-sync) to where our framework is located   - `LLVM_DIR`: this one should point to the folder where LLVM was compiled
  - `OUT_DIR`: where the executable files will be stored
  - `THIS_DIR`: folder with the scripts
  - `MMT_SYNC`: also the folder where the framework is located


## Recompiling the Benchmarks

We have a special script to compile all the benchmarks we used. The script is _Scripts/comp/recomp.sh_. Open it script and set the variables to the folder where the sources (PARSEC, SPLASH and TACHYON) are. 

Edit the script to choose if you want to compile them with FunctionLevel or PostDom LLVM passes. The second pass is required to execute Post-dominator heuristics.

## Compilation Problems

We ran into problems compiling some benchmarks. Thus, be aware that it is not an easy task. For instance:

  - to compile blackscholes you need to add "M4 = m4" to the makefile
  - to compile bodytrack you need to do export "VPATH=.".


## Experiments

To execute the experments, use the scripts in the folder _Scripts_:

  - `benchmarks.sh`: contains the commands we need to execute each benchmark. The variable `BENCHMARKS` is the folder where all executable files are, and `BENCH_INPUT` is the folder where the input of the benchmarks is.

 - `exec_bench.sh`: executes all heuristics. Make sure you run them on a free HDD partition because it takes a long time, the temporary files are huge, and the HDD will be busy, thus slowing down the system. Remember to set the variables `MAIN_FOLDER` and `PIN_DIR`.

 - `exec_mem.sh`: is another intensive experiment. It uses the best heuristics to analyse memory access patterns, execute-identical instructions and perform other analysis. Set `MAIN_FOLDER` and `PIN_DIR` too.


## Adding New Heuristics

To create a new heuristic, add a .cpp file to the folder _Heuristic/src_. Create a subclass of the class _Heuristic_ and implement the function `choose_threads()`. Each call of this function is an iteration, the function should set the boolean array of threads `t[i].is_active` to say if each thread will execute or will not. 

Next, implement the functions `new_heuristic` and `delete_heuristic` to create and delete your heuristic object. Finally, add a file to the _info_ folder to tell the type of binaries that the heuristic requires ('HardwareOnly', 'FunctionLimit', 'PostDominator', 'Structured').

## Directory structure
   - _Execute_: Folder with the main program that will execute a heuristic and do analysis. The command is:

       `./heuristic mode P heuristic_file "heuristic parameters" thread_file_1 thread_file_2 thread_file_3`

 The heuristic parameters must be between "" and the mode is 0 to print all analysis or 1 to print the number of fetched instructions. P is the number of fetch units to be used in opportunistic SIMD simulation. The default value is 0, which means the maximum value.

   - _Heuristics_: Folder with each heuristic. The makefile will compile all heuristics. The _parameters_ folder has a list of parameters that the heuristics receive when a script is executed. Each line of those files is a parameter. The _info_ directory tells the type of binaries that the heuristic requires ('HardwareOnly', 'FunctionLimit', 'PostDominator', 'Structured').

   - _Tools_: Folder with the Pin tool. `insConuter` counts statistics of the benchmarks: number of instructions of each thread, number of calls and returns, and so on. The tool `insList` prints each instruction address in each thread.

   - _Passes_: Folder with LLVM passes. Each folder is a pass and the makefile will compile them all. Edit the makefile to set the directory where LLVM is installed.

   - _Scripts_: Folder with helper scripts.

   - _mini_: Folder with some small handcrafted benchmarks. Compile them by using the command `make`.
   
