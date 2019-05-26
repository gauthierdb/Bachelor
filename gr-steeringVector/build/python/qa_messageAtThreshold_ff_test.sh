#!/bin/sh
export VOLK_GENERIC=1
export GR_DONT_LOAD_PREFS=1
export srcdir=/home/gauthier/gr-steeringVector/python
export PATH=/home/gauthier/gr-steeringVector/build/python:$PATH
export LD_LIBRARY_PATH=/home/gauthier/gr-steeringVector/build/lib:$LD_LIBRARY_PATH
export PYTHONPATH=/home/gauthier/gr-steeringVector/build/swig:$PYTHONPATH
/usr/bin/python2 /home/gauthier/gr-steeringVector/python/qa_messageAtThreshold_ff.py 
