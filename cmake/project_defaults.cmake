#!cmake .
cmake_minimum_required ( VERSION 3.21 )

# Header style guard for multiple inclusion protection
if( DEFINED PROJECT_DEFAULTS )
  return()
endif()
set( MODERN_CMAKE_UTILS ON )

#-------------------------------------------------------------------------------
# Extend module path
#-------------------------------------------------------------------------------
if( DEFINED ENV{CMAKE_PREFIX_PATH} )
  foreach( _dir $ENV{CMAKE_PREFIX_PATH} )
    if( EXISTS "${_dir}" )
      list( INSERT CMAKE_MODULE_PATH 1 "${_dir}" )
    endif()
  endforeach()
endif()
list( REMOVE_DUPLICATES CMAKE_MODULE_PATH )

#-------------------------------------------------------------------------------
# Increase sensitivity to all warnings
#-------------------------------------------------------------------------------
include( strict )  # << Report as many compilation issues as able

#------------------------------------------------------------------------------
# Choose minimum C++ standard for remainder of code
#------------------------------------------------------------------------------
set( CMAKE_CXX_STANDARD          17 CACHE STRING "C++ standard to build all targets." )
set( CMAKE_CXX_STANDARD_REQUIRED 17 CACHE BOOL   "The CMAKE_CXX_STANDARD selected C++ standard is a requirement." )

enable_testing()

include( set_target )

# vim:syntax=cmake:nospell
