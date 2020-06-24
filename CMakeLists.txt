cmake_minimum_required(VERSION 3.0)

project(OpenCLHeadersCpp
  VERSION 2.2
  LANGUAGES CXX # Ideally should be NONE, but GNUInstallDirs detects platform arch using try_compile
  # https://stackoverflow.com/questions/43379311/why-does-project-affect-cmakes-opinion-on-cmake-sizeof-void-p
)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DOCS "Build Documentation" ON)
option(BUILD_EXAMPLES "Build Examples" ON)
option(BUILD_TESTS "Build Unit Tests" ON)

add_library(OpenCLHeadersCpp INTERFACE)
add_library(OpenCL::OpenCLHeadersCpp
  ALIAS OpenCLHeadersCpp
)
target_include_directories(OpenCLHeadersCpp
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)

install(
  TARGETS OpenCLHeadersCpp
  EXPORT OpenCLHeadersCppTargets
)

include(GNUInstallDirs)

install(
  DIRECTORY include/CL
  DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
)
export(
  EXPORT OpenCLHeadersCppTargets
  FILE ${PROJECT_BINARY_DIR}/OpenCLHeadersCpp/OpenCLHeadersCppTargets.cmake
  NAMESPACE OpenCL::
)
file(
  WRITE ${PROJECT_BINARY_DIR}/OpenCLHeadersCpp/OpenCLHeadersCppConfig.cmake
  "include(\"\${CMAKE_CURRENT_LIST_DIR}/OpenCLHeadersCppTargets.cmake\")"
)

set(config_package_location ${CMAKE_INSTALL_DATADIR}/cmake/OpenCLHeadersCpp)
install(
  EXPORT OpenCLHeadersCppTargets
  FILE OpenCLHeadersCppTargets.cmake
  NAMESPACE OpenCL::
  DESTINATION ${config_package_location}
)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenCLHeadersCpp/OpenCLHeadersCppConfig.cmake
  DESTINATION ${config_package_location}
)

unset(CMAKE_SIZEOF_VOID_P)
include(CMakePackageConfigHelpers)
write_basic_package_version_file(
  ${CMAKE_CURRENT_BINARY_DIR}/OpenCLHeadersCpp/OpenCLHeadersCppConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY AnyNewerVersion
)
install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/OpenCLHeadersCpp/OpenCLHeadersCppConfigVersion.cmake
  DESTINATION ${config_package_location}
)

if(DEFINED ENV{AMDAPPSDKROOT})
  set(OPENCL_DIST_DIR $ENV{AMDAPPSDKROOT} CACHE PATH "OpenCL source dir")
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(OPENCL_LIB_DIR ${OPENCL_DIST_DIR}/lib/x86_64 )
  else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(OPENCL_LIB_DIR ${OPENCL_DIST_DIR}/lib/x86 )
  endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
elseif(DEFINED ENV{INTELOCLSDKROOT})
  set(OPENCL_DIST_DIR $ENV{INTELOCLSDKROOT} CACHE PATH "OpenCL source dir")
  if( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(OPENCL_LIB_DIR ${OPENCL_DIST_DIR}/lib/x64 )
  else( CMAKE_SIZEOF_VOID_P EQUAL 8 )
    set(OPENCL_LIB_DIR ${OPENCL_DIST_DIR}/lib/x86 )
  endif( CMAKE_SIZEOF_VOID_P EQUAL 8 )
else()
  set(OPENCL_DIST_DIR "" CACHE PATH "OpenCL source dir")
  set(OPENCL_LIB_DIR "${OPENCL_DIST_DIR}/lib" CACHE PATH "OpenCL library dir" )
endif()

if( CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang" )
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread -Werror=return-type")
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pthread")
endif()

set(OPENCL_INCLUDE_DIR ${OPENCL_DIST_DIR}/include CACHE PATH "OpenCL source dir")
set(UNITY_DIR ${PROJECT_SOURCE_DIR}/external/Unity CACHE PATH "Unity dir")
set(CMOCK_DIR ${PROJECT_SOURCE_DIR}/external/CMock CACHE PATH "CMock dir")

if(BUILD_DOCS)
  add_subdirectory(docs)
endif(BUILD_DOCS)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

if(BUILD_TESTS)
  add_subdirectory(tests)
endif(BUILD_TESTS)
