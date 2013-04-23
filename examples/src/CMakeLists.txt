set(TRIVIAL_SOURCEs
    trivial.cpp )

set(TRIVIAL_HEADERS
    ${CLCPP_BINARY_DIR}/include/CL/cl.hpp)
set_source_files_properties(${CLCPP_BINARY_DIR}/include/CL/cl.hpp PROPERTIES GENERATED 1)

include_directories(
    ${CLCPP_BINARY_DIR}/include
    ${OPENCL_DIST_DIR}/include)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  link_directories( ${OPENCL_DIST_DIR}/lib/x86_64 )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  link_directories( ${OPENCL_DIST_DIR}/lib/x86 )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

if( MSVC )
  set(CMAKE_CXX_FLAGS " -W3 /EHsc" )
elseif( CMAKE_COMPILER_IS_GNUCXX )
  set(CMAKE_CXX_FLAGS "-Wall -std=c++0x" )
elseif( CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  set(CMAKE_CXX_FLAGS "-Wall -std=c++11" )
endif()

add_executable(trivial ${TRIVIAL_SOURCEs} ${TRIVIAL_HEADERS})
add_dependencies(trivial generate_clhpp)
target_link_libraries( trivial OpenCL )
