add_custom_command(
    OUTPUT stripped/cl.h
    COMMAND ${CMAKE_COMMAND} -E make_directory stripped
    COMMAND ${CMAKE_COMMAND} -E make_directory mocks
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/strip_defines.py ${OPENCL_INCLUDE_DIR}/CL/cl.h stripped/cl.h
    COMMENT "Stripping defines from cl.h"
    DEPENDS ${OPENCL_INCLUDE_DIR}/CL/cl.h strip_defines.py)
	
add_custom_command(
    OUTPUT mocks/Mockcl.c mocks/Mockcl.h
    COMMAND ruby ${CMOCK_DIR}/lib/cmock.rb -o${CMAKE_CURRENT_SOURCE_DIR}/cmock.yml stripped/cl.h
    COMMENT "Generating mocks"
    DEPENDS stripped/cl.h cmock.yml)
	
add_custom_command(
    OUTPUT test_clhpp_Runner.c
    COMMAND ruby ${UNITY_DIR}/auto/generate_test_runner.rb test_clhpp.cpp cmock.yml ${CMAKE_CURRENT_BINARY_DIR}/test_clhpp_Runner.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating test runner"
    DEPENDS test_clhpp.cpp cmock.yml)
	
add_custom_command(
    OUTPUT test_cl2hpp_Runner.c
    COMMAND ruby ${UNITY_DIR}/auto/generate_test_runner.rb test_cl2hpp.cpp cmock.yml ${CMAKE_CURRENT_BINARY_DIR}/test_cl2hpp_Runner.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating test runner for 2.0"
    DEPENDS test_cl2hpp.cpp cmock.yml)
	
include_directories(
    ${CLCPP_BINARY_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/mocks
    ${OPENCL_DIST_DIR}/include
    ${OPENCL_INCLUDE_DIR}
    ${OPENCL_INCLUDE_DIR}/CL
    ${UNITY_DIR}/src
    ${CMOCK_DIR}/src)

add_definitions(-DUNITY_SUPPORT_64)
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    add_definitions(-DUNITY_POINTER_WIDTH=64)
    add_definitions("-DCMOCK_MEM_PTR_AS_INT=unsigned long long")
    add_definitions(-DCMOCK_MEM_ALIGN=3)
endif()
if( CMAKE_SIZEOF_LONG EQUAL 8 )
    add_definitions(-DUNITY_LONG_WIDTH=64)
endif()

set(TEST_CLHPP_HEADERS
    ${CLCPP_BINARY_DIR}/include/CL/cl.hpp
    mocks/Mockcl.h)
set_source_files_properties(${CLCPP_BINARY_DIR}/include/CL/cl.hpp PROPERTIES GENERATED 1)

set(TEST_CL2HPP_HEADERS
    ${CLCPP_BINARY_DIR}/include/CL/cl2.hpp
    mocks/Mockcl.h)
set_source_files_properties(${CLCPP_BINARY_DIR}/include/CL/cl2.hpp PROPERTIES GENERATED 1)

set(TEST_CLHPP_SOURCES_COMMON
    ${CMAKE_CURRENT_BINARY_DIR}/mocks/Mockcl.c
    ${CMOCK_DIR}/src/cmock.c
    ${UNITY_DIR}/src/unity.c)

set(TEST_CLHPP_SOURCES
    ${TEST_CLHPP_SOURCES_COMMON}
    ${CMAKE_CURRENT_BINARY_DIR}/test_clhpp_Runner.c
    test_clhpp.cpp)

set(TEST_CL2HPP_SOURCES
    ${TEST_CLHPP_SOURCES_COMMON}
    ${CMAKE_CURRENT_BINARY_DIR}/test_cl2hpp_Runner.c
    test_cl2hpp.cpp)

  
#set_source_files_properties(test_cl2hpp_Runner.c PROPERTIES LANGUAGE "CXX")
#set_source_files_properties(test_clhpp_Runner.c PROPERTIES LANGUAGE "CXX")
add_executable(test_clhpp ${TEST_CLHPP_SOURCES} ${TEST_CLHPP_HEADERS})
add_executable(test_clhpp_deprecated_1_1 ${TEST_CLHPP_SOURCES} ${TEST_CLHPP_HEADERS})
set_target_properties(test_clhpp_deprecated_1_1 PROPERTIES COMPILE_DEFINITIONS "CL_USE_DEPRECATED_OPENCL_1_1_APIS")
add_executable(test_clhpp_deprecated_1_2 ${TEST_CLHPP_SOURCES} ${TEST_CLHPP_HEADERS})
set_target_properties(test_clhpp_deprecated_1_2 PROPERTIES COMPILE_DEFINITIONS "CL_USE_DEPRECATED_OPENCL_1_1_APIS")
set_target_properties(test_clhpp_deprecated_1_2 PROPERTIES COMPILE_DEFINITIONS "CL_USE_DEPRECATED_OPENCL_1_2_APIS")
add_dependencies(test_clhpp generate_clhpp)
add_executable(test_cl2hpp ${TEST_CL2HPP_SOURCES} ${TEST_CL2HPP_HEADERS})
add_dependencies(test_cl2hpp generate_cl2hpp)
add_test(run_test_clhpp test_clhpp)
add_test(run_test_clhpp_deprecated_1_1 test_clhpp_deprecated_1_1)
add_test(run_test_cl2hpp test_cl2hpp)

if( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
    set(TEST_CLHPP_CXX11_SOURCES
        ${TEST_CLHPP_SOURCES_COMMON}
        ${CMAKE_CURRENT_BINARY_DIR}/test_clhpp_Runner.c
        test_clhpp_cxx11.cpp)
    set_source_files_properties(test_clhpp_cxx11.cpp PROPERTIES COMPILE_FLAGS "-std=c++11")
    set_source_files_properties(test_cl2hpp.cpp PROPERTIES COMPILE_FLAGS "-std=c++11")
    add_executable(test_clhpp_cxx11 ${TEST_CLHPP_CXX11_SOURCES} ${TEST_CLHPP_HEADERS})
    add_test(run_test_clhpp_cxx11 test_clhpp_cxx11)
endif()
