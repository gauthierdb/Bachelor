#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/gauthier/gr-steeringVector/lib
export PATH=/home/gauthier/gr-steeringVector/build/lib:$PATH
export LD_LIBRARY_PATH=/home/gauthier/gr-steeringVector/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=$PYTHONPATH
test-steeringVector 
