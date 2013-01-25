cmake_minimum_required(VERSION 2.6)
project(CLCPP)

set(OPENCL_DIST_DIR $ENV{AMDAPPSDKROOT} CACHE PATH "OpenCL source dir")
add_subdirectory(examples)
