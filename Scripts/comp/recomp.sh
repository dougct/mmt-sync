#!/bin/bash

#Set directories
PARSEC=/scratch/$USER/volume/benchmarks/parsec/apps
TACHYON=/scratch/$USER/volume/benchmarks
SPLASH=/scratch/$USER/volume/benchmarks/splash2/apps

OUT_DIR=$HOME/Benchmarks
THIS_DIR=$HOME/mmt-sync/Scripts/comp
MMT_SYNC=$HOME/mmt-sync

#Choose if you want to compile with "simple" mode or with "postdom" mode

#export PASS_NAME=FunctionLimit
#SUBFOLDER=simple

#export PASS_NAME=LongestPath
#SUBFOLDER=longpath

export PASS_NAME=PostDom
SUBFOLDER=postdom



FIRST_PATH=$PATH
export PATH=$MMT_SYNC/Scripts/compilation':'$PATH

set -e

mkdir -p $OUT_DIR/$SUBFOLDER
rm -f $OUT_DIR/$SUBFOLDER/*



function cleaning {
    make clean
    rm -f *.s *.s1 *.o *.oo *.bc *.bc2
}



cd $PARSEC/blackscholes/src && cleaning && make
cp $PARSEC/blackscholes/src/blackscholes $OUT_DIR/$SUBFOLDER/

cd $PARSEC/swaptions/src && cleaning && make
cp $PARSEC/swaptions/src/swaptions $OUT_DIR/$SUBFOLDER/

cd $PARSEC/fluidanimate/src && cleaning && make
cp $PARSEC/fluidanimate/src/fluidanimate $OUT_DIR/$SUBFOLDER/

#cd $PARSEC/bodytrack/obj && cleaning && make
#cp $PARSEC/bodytrack/obj/TrackingBenchmark/bodytrack $OUT_DIR/$SUBFOLDER/



cd $TACHYON/tachyon/unix && cleaning && make linux-64-thr
cp $TACHYON/tachyon/compile/linux-64-thr/tachyon $OUT_DIR/$SUBFOLDER/



cd $SPLASH/fmm/src && cleaning && make
cp $THIS_DIR/interactions.c $SPLASH/fmm/src && cp $THIS_DIR/construct_grid.c $SPLASH/fmm/src
cd $SPLASH/fmm/src && touch interactions.c && touch construct_grid.c && make
cp $SPLASH/fmm/src/fmm $OUT_DIR/$SUBFOLDER/

cd $SPLASH/barnes/src && cleaning && make
cp $SPLASH/barnes/src/barnes $OUT_DIR/$SUBFOLDER/

cd $SPLASH/water_nsquared/src && cleaning && make
cp $SPLASH/water_nsquared/src/water_nsquared $OUT_DIR/$SUBFOLDER/

cd $SPLASH/ocean_ncp/src && cleaning && make
cp $SPLASH/ocean_ncp/src/ocean_ncp $OUT_DIR/$SUBFOLDER/

cd $SPLASH/ocean_cp/src && cleaning && make
cp $SPLASH/ocean_cp/src/ocean_cp $OUT_DIR/$SUBFOLDER/

cd $SPLASH/radix/src && cleaning
cp $THIS_DIR/radix.c $SPLASH/radix/src
cd $SPLASH/radix/src && touch radix.c && make
cp $SPLASH/radix/src/radix $OUT_DIR/$SUBFOLDER/

cd $SPLASH/fft/src && cleaning
cp $THIS_DIR/fft.c $SPLASH/fft/src
cd $SPLASH/fft/src && touch fft.c && make
cp $SPLASH/fft/src/fft $OUT_DIR/$SUBFOLDER/

set +e
cd $SPLASH/volrend/src && cleaning && make
set -e
cp $THIS_DIR/raytrace.c $SPLASH/volrend/src
cd $SPLASH/volrend/src && touch raytrace.c && make
cp $SPLASH/volrend/src/volrend $OUT_DIR/$SUBFOLDER/


#bodytrack does not work with the newer wrapper and libraries are not included
export PATH=$HOME/Benchmarks/comp':'$FIRST_PATH
cd $PARSEC/bodytrack/obj && cleaning && make
cp $PARSEC/bodytrack/obj/TrackingBenchmark/bodytrack $OUT_DIR/$SUBFOLDER/


echo 'DONE!!'
