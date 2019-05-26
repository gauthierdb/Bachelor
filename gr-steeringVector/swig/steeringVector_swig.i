/* -*- c++ -*- */

#define STEERINGVECTOR_API

%include "gnuradio.i"			// the common stuff

//load generated python docstrings
%include "steeringVector_swig_doc.i"

%{
#include "steeringVector/steeringVector.h"
#include "steeringVector/phaseOffset.h"
#include "steeringVector/messageAtThreshold.h"
#include "steeringVector/sample_offset_multi.h"
#include "steeringVector/multiplyComplex.h"
#include "steeringVector/messageAtThreshold_ff.h"
%}


%include "steeringVector/steeringVector.h"
GR_SWIG_BLOCK_MAGIC2(steeringVector, steeringVector);

%include "steeringVector/phaseOffset.h"
GR_SWIG_BLOCK_MAGIC2(steeringVector, phaseOffset);
%include "steeringVector/messageAtThreshold.h"
GR_SWIG_BLOCK_MAGIC2(steeringVector, messageAtThreshold);

%include "steeringVector/sample_offset_multi.h"
GR_SWIG_BLOCK_MAGIC2(steeringVector, sample_offset_multi);
%include "steeringVector/multiplyComplex.h"
GR_SWIG_BLOCK_MAGIC2(steeringVector, multiplyComplex);
%include "steeringVector/messageAtThreshold_ff.h"
GR_SWIG_BLOCK_MAGIC2(steeringVector, messageAtThreshold_ff);
