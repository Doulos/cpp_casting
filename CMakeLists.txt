#!cmake .

cmake_minimum_required( VERSION 3.20 )

project( cpp_casting VERSION 1.0 DESCRIPTION "Examples of good and bad casting in C++" LANGUAGES CXX )

#-------------------------------------------------------------------------------
# Find project working tree directory containing cmake/)
set( _dir "${CMAKE_CURRENT_SOURCE_DIR}" )
cmake_path( GET _dir ROOT_PATH _root )
while( NOT IS_DIRECTORY "${_dir}/cmake" )
  cmake_path( GET _dir PARENT_PATH _dir )
  if( "${_dir}" STREQUAL "${_root}" )
    message( FATAL_ERROR "Unable to find project workintg tree directory!" )
  endif()
endwhile()
set( WORKTREE_DIR "${_dir}" CACHE PATH "Points to directory containing cmake/ subdirectory for the project." )
list( PREPEND CMAKE_MODULE_PATH "${WORKTREE_DIR}/cmake" )
include( project_defaults )

set_target( cpp_casting )
add_executable( "${Target}" )
target_sources( "${Target}" PRIVATE cpp_casting.cpp )
add_test( NAME test-"${Target}" COMMAND "${Target}" )

# vim:syntax=cmake:nospell
