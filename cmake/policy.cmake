#!cmake .
cmake_minimum_required ( VERSION 3.21 )

# Things we want for all CMake compilations

#-------------------------------------------------------------------------------
# Don't allow in-source compilation
#-------------------------------------------------------------------------------
if(PROJECT_SOURCE_DIR STREQUAL PROJECT_BINARY_DIR)
  message(FATAL_ERROR "In-source builds are not allowed")
endif()

if( NOT DEFINED WORKTREE_DIR )
  message( FATAL_ERROR "WORKTREE_DIR variable was not defined" )
endif()

set( CMAKE_FIND_PACKAGE_PREFER_CONFIG ON )

#-------------------------------------------------------------------------------
# Extend module path
#-------------------------------------------------------------------------------
  set( PROJECT_DIRS "${WORKTREE_DIR}/extern;${APPS}" )
  foreach( _dir ${PROJECT_DIRS} )
     if( EXISTS "${_dir}" )
       list( APPEND CMAKE_PREFIX_PATH "${_dir}" )
     endif()
  endforeach()
  list( REMOVE_DUPLICATES CMAKE_PREFIX_PATH )
  include_directories( . "${WORKTREE_DIR}/include" "${WORKTREE_DIR}/extern/include" )

#-------------------------------------------------------------------------------
# Increase sensitivity to all warnings
#-------------------------------------------------------------------------------
if( NOT DEFINED ENV{LAX} )
  # Insist on full C++ compliance
  set(CMAKE_CXX_STANDARD_REQUIRED ON)
  set(CMAKE_CXX_EXTENSIONS OFF)
  if (MSVC)
      # warning level 4
      add_compile_options( /W4 )
  else()
      # lots of warnings
      add_compile_options( -Wall -Wextra -pedantic )
  endif()
else()
  if (MSVC)
      # warning level 4
      add_compile_options( /W4 )
  else()
      # lots of warnings except unused parameters (slightly permissive)
      add_compile_options( -Wall -Wextra -pedantic -Wno-unused-parameter)
  endif()
endif()

#------------------------------------------------------------------------------
# Choose minimum C++ standard for remainder of code
#------------------------------------------------------------------------------
if( NOT DEFINED USE_CXX_VERSION )
  set( USE_CXX_VERSION 17 )
endif()
if( NOT DEFINED USE_C_VERSION )
  set( USE_C_VERSION 11 )
endif()
set( CMAKE_CXX_STANDARD          ${USE_CXX_VERSION} CACHE STRING "C++ standard to build all targets." )
set( CMAKE_CXX_STANDARD_REQUIRED ${USE_CXX_VERSION} CACHE BOOL   "The CMAKE_CXX_STANDARD selected C++ standard is a requirement." )
set( CMAKE_C_STANDARD            ${USE_C_VERSION}   CACHE STRING "C standard to build all targets." )
set( CMAKE_C_STANDARD_REQUIRED   ${USE_C_VERSION}   CACHE BOOL   "The CMAKE_CXX_STANDARD selected C standard is a requirement." )
set( CMAKE_CXX_EXTENSIONS NO )

enable_testing()

macro( set_target target )
  set( Target "${target}" )
  list( LENGTH Targets count )
  if( ${count} EQUAL 1)
    list( GET Targets 0 first )
    if( "${first}" STREQUAL "${PROJECT_NAME}" )
      list( REMOVE_AT Targets 0 )
    endif()
  endif()
  list( APPEND Targets "${target}" )
endmacro()

# Default
set_target( ${PROJECT_NAME} )

# vim:nospell
