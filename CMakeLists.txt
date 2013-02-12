cmake_minimum_required(VERSION 2.6)
project(CLCPP)
enable_testing()

set(OPENCL_DIST_DIR $ENV{AMDAPPSDKROOT} CACHE PATH "OpenCL source dir")
set(UNITY_DIR $ENV{UNITY_DIR} CACHE PATH "Unity dir")
set(CMOCK_DIR $ENV{CMOCK_DIR} CACHE PATH "CMock dir")

add_subdirectory(include)
add_subdirectory(examples)
add_subdirectory(tests)
