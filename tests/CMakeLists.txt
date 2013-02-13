add_custom_command(
    OUTPUT stripped/cl.h
    COMMAND ${CMAKE_COMMAND} -E make_directory stripped
    COMMAND ${CMAKE_COMMAND} -E make_directory mocks
    COMMAND python ${CMAKE_CURRENT_SOURCE_DIR}/strip_defines.py ${OPENCL_DIST_DIR}/include/CL/cl.h stripped/cl.h
    COMMENT "Stripping defines from cl.h"
    DEPENDS ${OPENCL_DIST_DIR}/include/CL/cl.h strip_defines.py)

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

include_directories(
    ${CLCPP_BINARY_DIR}/include
    ${CMAKE_CURRENT_BINARY_DIR}/mocks
    ${OPENCL_DIST_DIR}/include
    ${OPENCL_DIST_DIR}/include/CL
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

set(TEST_CLHPP_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/test_clhpp_Runner.c
    test_clhpp.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/mocks/Mockcl.c
    ${CMOCK_DIR}/src/cmock.c
    ${UNITY_DIR}/src/unity.c)

add_executable(test_clhpp ${TEST_CLHPP_SOURCES} ${TEST_CLHPP_HEADERS})
add_dependencies(test_clhpp generate_clhpp)
add_test(run_test_clhpp test_clhpp)
