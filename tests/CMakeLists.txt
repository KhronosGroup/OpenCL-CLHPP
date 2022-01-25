set(OPENCL_HEADERS_INCLUDE_DIR_GENEXPR $<TARGET_PROPERTY:OpenCL::Headers,INTERFACE_INCLUDE_DIRECTORIES>)
###############################
###                         ###
### Workaround to CMake bug ###
###                         ###
###############################
#
# The generated mocks\Mockcl.h include the OpenCL headers as
#
# #include "CL/cl_platform.h"
# #include "cl.h"
#
# which requires the tests to not only inherit the INTERFACE_INCLUDE_DIRECTORIES, but also the same dir with
# /CL appended at the end. There seems to be a bug in CMake (3.19 even) where if a target has the same genexpr
# predicate twice, the second time it always fails. Having both
#
# $<TARGET_PROPERTY:Headers,INTERFACE_INCLUDE_DIRECTORIES> (invisbly via linking to OpenCL::Headers)
# $<TARGET_PROPERTY:Headers,INTERFACE_INCLUDE_DIRECTORIES>/CL
#
# means we can only get one of these switches. We forcibly go around it by extracting the value manually.
#
# Bug ticket: https://gitlab.kitware.com/cmake/cmake/-/issues/22735
#
# ticket closed with by-design. We should find a way to fix the Mock tests to not consume the public API using
# mixed style includes.
get_target_property(OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES OpenCL::Headers INTERFACE_INCLUDE_DIRECTORIES)

if(OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES MATCHES "BUILD_INTERFACE")
  # When building OpenCL::Headers is part of this build (OpenCL-SDK), we have a property like for eg.
  # $<BUILD_INTERFACE:C:/OpenCL-SDK/external/OpenCL-Headers>;$<INSTALL_INTERFACE:include>
  # and need to touch up this property. We want the BUILD_INTERFACE part only.
  # When OpenCL::Headers is consumed through a Package Config file (OpenCL-CLHPP), this property
  # no longer holds unevaluated generator expressions.
  string(REGEX MATCHALL
    [[\$<BUILD_INTERFACE:(.*)>;]]
    OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES
    "${OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES}")
  set(OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_MATCH_1}")
endif()
# End of workaround

add_custom_command(
    OUTPUT stripped/cl.h
    COMMAND ${CMAKE_COMMAND} -E make_directory stripped
    COMMAND ${CMAKE_COMMAND} -E make_directory mocks
    COMMAND ${CMAKE_COMMAND} -D INPUT="${OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES}/CL/cl.h" -D OUTPUT="stripped/cl.h" -P ${CMAKE_CURRENT_SOURCE_DIR}/strip_defines.cmake
    COMMENT "Stripping defines from cl.h"
    DEPENDS ${OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES}/CL/cl.h strip_defines.cmake)

add_custom_target(
    strip_cl_defines ALL
    DEPENDS stripped/cl.h
    SOURCES strip_defines.cmake)

add_custom_command(
    OUTPUT mocks/Mockcl.c mocks/Mockcl.h
    COMMAND ${RUBY_EXECUTABLE} ${CMOCK_DIR}/lib/cmock.rb -o${CMAKE_CURRENT_SOURCE_DIR}/cmock.yml stripped/cl.h
    COMMENT "Generating mocks"
    DEPENDS stripped/cl.h cmock.yml)

add_custom_target(
    mock_cl_header ALL
    DEPENDS mocks/Mockcl.c mocks/Mockcl.h
    SOURCES cmock.yml)

add_dependencies(mock_cl_header strip_cl_defines)

add_custom_command(
    OUTPUT test_openclhpp_Runner.c
    COMMAND ${RUBY_EXECUTABLE} ${UNITY_DIR}/auto/generate_test_runner.rb test_openclhpp.cpp cmock.yml ${CMAKE_CURRENT_BINARY_DIR}/test_openclhpp_Runner.c
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating test runner"
    DEPENDS test_openclhpp.cpp cmock.yml)

add_custom_target(
    openclhpp_runner ALL
    DEPENDS test_openclhpp_Runner.c
    SOURCES test_openclhpp.cpp)

if(CMAKE_COMPILER_IS_GNUCC OR CMAKE_CXX_COMPILER_ID MATCHES "(Apple)?Clang")
    add_compile_options(-Wno-deprecated-declarations)
endif()

add_definitions(-DCL_TARGET_OPENCL_VERSION=300)
add_definitions(-DCL_EXPERIMENTAL)

add_definitions(-DUNITY_SUPPORT_64)
if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    add_definitions(-DUNITY_POINTER_WIDTH=64)
    add_definitions("-DCMOCK_MEM_PTR_AS_INT=unsigned long long")
    add_definitions(-DCMOCK_MEM_ALIGN=3)
endif()
if( CMAKE_SIZEOF_LONG EQUAL 8 )
    add_definitions(-DUNITY_LONG_WIDTH=64)
endif()

set(TEST_HEADERS
    ${PROJECT_SOURCE_DIR}/include/CL/opencl.hpp
    mocks/Mockcl.h)

set(TEST_SOURCES
    ${CMAKE_CURRENT_BINARY_DIR}/test_openclhpp_Runner.c
    test_openclhpp.cpp
    ${CMAKE_CURRENT_BINARY_DIR}/mocks/Mockcl.c
    ${CMOCK_DIR}/src/cmock.c
    ${UNITY_DIR}/src/unity.c)

# TODO enable testing for OpenCL 1.0 and 1.1
foreach(VERSION 120 200 210 220 300)
  foreach(OPTION "" CL_HPP_ENABLE_EXCEPTIONS CL_HPP_ENABLE_SIZE_T_COMPATIBILITY CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY CL_HPP_CL_1_2_DEFAULT_BUILD CL_HPP_USE_CL_DEVICE_FISSION CL_HPP_USE_CL_IMAGE2D_FROM_BUFFER_KHR CL_HPP_USE_CL_SUB_GROUPS_KHR CL_HPP_USE_IL_KHR)
    if(OPTION STREQUAL "")
      # The empty string means we're not setting any special option.
      set(UNDERSCORE_OPTION "${OPTION}")
      set(DEFINE_OPTION "")
    else()
      set(UNDERSCORE_OPTION "_${OPTION}")
      set(DEFINE_OPTION "-D${OPTION}")
    endif()

    set(TEST_EXE test_openclhpp_${VERSION}${UNDERSCORE_OPTION})
    add_executable(${TEST_EXE} ${TEST_SOURCES}) #${TEST_HEADERS})
    target_link_libraries(${TEST_EXE}
      PRIVATE
        OpenCL::HeadersCpp
        OpenCL::Headers
        Threads::Threads
    )
    target_include_directories(${TEST_EXE}
      PRIVATE
        ${CMAKE_CURRENT_BINARY_DIR}/mocks

        ${OPENCL_HEADERS_INTERFACE_INCLUDE_DIRECTORIES}/CL
        #${OPENCL_HEADERS_INCLUDE_DIR_GENEXPR}/CL

        ${UNITY_DIR}/src
        ${CMOCK_DIR}/src
    )
    target_compile_definitions(${TEST_EXE}
        PUBLIC -DCL_HPP_TARGET_OPENCL_VERSION=${VERSION} ${DEFINE_OPTION}
    )
    add_dependencies(${TEST_EXE}
        strip_cl_defines
        mock_cl_header
        openclhpp_runner
    )
    add_test(NAME ${TEST_EXE} COMMAND ${TEST_EXE})
  endforeach(OPTION)
endforeach(VERSION)
