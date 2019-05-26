# Copyright 2011,2012,2016 Free Software Foundation, Inc.
#
# This file is part of GNU Radio
#
# GNU Radio is free software; you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation; either version 3, or (at your option)
# any later version.
#
# GNU Radio is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with GNU Radio; see the file COPYING.  If not, write to
# the Free Software Foundation, Inc., 51 Franklin Street,
# Boston, MA 02110-1301, USA.

########################################################################
# Setup library
########################################################################
include(GrPlatform) #define LIB_SUFFIX

include_directories(${Boost_INCLUDE_DIR})
link_directories(${Boost_LIBRARY_DIRS})
list(APPEND steeringVector_sources
    steeringVector_impl.cc
    phaseOffset_impl.cc
    messageAtThreshold_impl.cc
    sample_offset_multi_impl.cc
    multiplyComplex_impl.cc
    messageAtThreshold_ff_impl.cc )

set(steeringVector_sources "${steeringVector_sources}" PARENT_SCOPE)
if(NOT steeringVector_sources)
	MESSAGE(STATUS "No C++ sources... skipping lib/")
	return()
endif(NOT steeringVector_sources)

add_library(gnuradio-steeringVector SHARED ${steeringVector_sources})
target_link_libraries(gnuradio-steeringVector ${Boost_LIBRARIES} ${GNURADIO_ALL_LIBRARIES})
set_target_properties(gnuradio-steeringVector PROPERTIES DEFINE_SYMBOL "gnuradio_steeringVector_EXPORTS")

if(APPLE)
    set_target_properties(gnuradio-steeringVector PROPERTIES
        INSTALL_NAME_DIR "${CMAKE_INSTALL_PREFIX}/lib"
    )
endif(APPLE)

########################################################################
# Install built library files
########################################################################
include(GrMiscUtils)
GR_LIBRARY_FOO(gnuradio-steeringVector RUNTIME_COMPONENT "steeringVector_runtime" DEVEL_COMPONENT "steeringVector_devel")

########################################################################
# Build and register unit test
########################################################################
include(GrTest)

include_directories(${CPPUNIT_INCLUDE_DIRS})

list(APPEND test_steeringVector_sources
    ${CMAKE_CURRENT_SOURCE_DIR}/test_steeringVector.cc
    ${CMAKE_CURRENT_SOURCE_DIR}/qa_steeringVector.cc
)

add_executable(test-steeringVector ${test_steeringVector_sources})

target_link_libraries(
  test-steeringVector
  ${GNURADIO_RUNTIME_LIBRARIES}
  ${Boost_LIBRARIES}
  ${CPPUNIT_LIBRARIES}
  gnuradio-steeringVector
)

GR_ADD_TEST(test_steeringVector test-steeringVector)

########################################################################
# Print summary
########################################################################
message(STATUS "Using install prefix: ${CMAKE_INSTALL_PREFIX}")
message(STATUS "Building for version: ${VERSION} / ${LIBVER}")

