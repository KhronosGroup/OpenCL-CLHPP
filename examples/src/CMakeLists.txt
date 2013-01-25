add_custom_command(
	OUTPUT ${CLCPPEXAMPLES_SOURCE_DIR}/include/CL/cl.hpp 
	COMMAND python gen_cl_hpp.py --strip_amd && move cl.hpp ${CLCPPEXAMPLES_SOURCE_DIR}/include/CL
	DEPENDS ${CLCPPEXAMPLES_SOURCE_DIR}/../input_cl.hpp
	COMMENT "Rebuilding cl.hpp ..."
	WORKING_DIRECTORY "${CLCPPEXAMPLES_SOURCE_DIR}/../")

set(TRIVIAL_SOURCEs
    trivial.cpp )

set(TRIVIAL_HEADERS
    ${CLCPPEXAMPLES_SOURCE_DIR}/include/CL/cl.hpp
    ${CLCPPEXAMPLES_SOURCE_DIR}/../input_cl.hpp)

include_directories(
   ${CLCPPEXAMPLES_SOURCE_DIR}/include 
   ${OPENCL_DIST_DIR}/include)

if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  link_directories( ${OPENCL_DIST_DIR}/lib/x86_64 )
else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
  link_directories( ${OPENCL_DIST_DIR}/lib/x86 )
endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )

set(CMAKE_CXX_FLAGS " -W3 /EHsc" )

add_executable(trivial ${TRIVIAL_SOURCEs} ${TRIVIAL_HEADERS})
target_link_libraries( trivial OpenCL.lib ) 
