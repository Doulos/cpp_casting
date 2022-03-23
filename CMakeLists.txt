#!cmake .

cmake_minimum_required( VERSION 3.20 )

project( cpp_casting VERSION 0.0 DESCRIPTION "{:DESC:}" LANGUAGES CXX )

list( PREPEND CMAKE_MODULE_PATH "$ENV{PROJECT_DIR}/cmake" )
include( project_defaults )

set_target( cpp_casting )
add_executable( "${TARGET}" )
target_sources( "${TARGET}" PRIVATE cpp_casting.cpp )
add_test( NAME test-"${TARGET}" COMMAND "${TARGET}" )

# vim:syntax=cmake:nospell
