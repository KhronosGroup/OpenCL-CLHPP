set(TRIVIAL_SOURCES
    trivial.cpp )

set(TRIVIAL_HEADERS
    ${CLCPP_BINARY_DIR}/include/CL/cl.hpp)
set_source_files_properties(${CLCPP_BINARY_DIR}/include/CL/cl.hpp PROPERTIES GENERATED 1)

set(TRIVIALCL2_SOURCES
    trivialCL2.cpp )

set(TRIVIALCL2_HEADERS
    ${CLCPP_BINARY_DIR}/include/CL/cl2.hpp)
set_source_files_properties(${CLCPP_BINARY_DIR}/include/CL/cl2.hpp PROPERTIES GENERATED 1)



include_directories(
    ${CLCPP_BINARY_DIR}/include
    ${OPENCL_DIST_DIR}/include
    ${OPENCL_INCLUDE_DIR})

link_directories( ${OPENCL_LIB_DIR} )

if( MSVC )
  set(CMAKE_CXX_FLAGS " -W3 /EHsc" )
elseif( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "-Wall -std=c++0x" )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  set(CMAKE_CXX_FLAGS "-Wall -std=c++11" )
endif()

add_executable(trivial ${TRIVIAL_SOURCES} ${TRIVIAL_HEADERS})
add_dependencies(trivial generate_clhpp)
target_link_libraries( trivial OpenCL )

add_executable(trivialCL2 ${TRIVIALCL2_SOURCES} ${TRIVIALCL2_HEADERS})
add_dependencies(trivialCL2 generate_cl2hpp)
target_link_libraries( trivialCL2 OpenCL )
