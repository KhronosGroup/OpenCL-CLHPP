cmake_minimum_required(VERSION 3.1)

project(OpenCLHeadersCpp
  VERSION 3.0
  LANGUAGES CXX # Ideally should be NONE, but GNUInstallDirs detects platform arch using try_compile
  # https://stackoverflow.com/questions/43379311/why-does-project-affect-cmakes-opinion-on-cmake-sizeof-void-p
)
enable_testing()

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_DOCS "Build Documentation" ON)
option(BUILD_EXAMPLES "Build Examples" ON)
option(OPENCL_CLHPP_BUILD_TESTING "Enable support for OpenCL C++ headers testing." OFF)
set(THREADS_PREFER_PTHREAD_FLAG ON CACHE BOOL
  "find_package(Threads) preference. Recommendation is to keep default value."
)
# Legacy helper variables, see later at finding headers and ICD Loader
set(OPENCL_DIST_DIR "" CACHE PATH "OpenCL source dir (legacy helper var)")
set(OPENCL_LIB_DIR "${OPENCL_DIST_DIR}/lib" CACHE PATH "OpenCL library dir (legacy helper var)" )
set(OPENCL_INCLUDE_DIR "${OPENCL_DIST_DIR}/include" CACHE PATH "OpenCL source dir (legacy helper var)")
# Helper vars with names consistent with OpenCL-ICD-Loader's OPENCL_ICD_LOADER_HEADERS_DIR
# Default values taken from legacy defaults
set(OPENCL_CLHPP_HEADERS_DIR "${OPENCL_INCLUDE_DIR}" CACHE PATH "OpenCL headers dir")
set(OPENCL_CLHPP_LOADER_DIR "${OPENCL_LIB_DIR}" CACHE PATH "OpenCL library dir")

add_library(HeadersCpp INTERFACE)
add_library(OpenCL::HeadersCpp ALIAS HeadersCpp)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR OPENCL_CLHPP_BUILD_TESTING)
  include(CTest)
endif()
if((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR OPENCL_CLHPP_BUILD_TESTING) AND BUILD_TESTING)
  set(CLHPP_BUILD_TESTS ON)
endif()

# In the spirit of backward compatibility, to not break existing build automation
# we first check if the helper vars refer to existing files. If yes, use them.
# Otherwise search using 3.0 installed CMake Package Configs.
if(EXISTS "${OPENCL_CLHPP_HEADERS_DIR}/CL/cl.h")
  if(NOT TARGET OpenCL::Headers)
    message(STATUS "Defining OpenCL::Headers through OPENCL_INCLUDE_DIR")
    add_library(OpenCLHeaders INTERFACE)
    add_library(OpenCL::Headers ALIAS OpenCLHeaders)
    target_include_directories(OpenCLHeaders INTERFACE ${OPENCL_INCLUDE_DIR})
  endif()
else()
  if(NOT TARGET OpenCL::Headers)
    find_package(OpenCLHeaders REQUIRED)
  endif()
endif()
if(BUILD_EXAMPLES OR CLHPP_BUILD_TESTS)
  enable_language(C)
  find_package(Threads REQUIRED)
endif()
if(CLHPP_BUILD_TESTS)
  find_program(RUBY_EXECUTABLE ruby REQUIRED)
endif()

target_include_directories(HeadersCpp
  INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_link_libraries(HeadersCpp
  INTERFACE
    OpenCL::Headers
)

install(
  TARGETS HeadersCpp
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

set(UNITY_DIR ${PROJECT_SOURCE_DIR}/external/Unity CACHE PATH "Unity dir")
set(CMOCK_DIR ${PROJECT_SOURCE_DIR}/external/CMock CACHE PATH "CMock dir")

if(BUILD_DOCS)
  add_subdirectory(docs)
endif(BUILD_DOCS)

if(BUILD_EXAMPLES)
  add_subdirectory(examples)
endif(BUILD_EXAMPLES)

if(CLHPP_BUILD_TESTS)
  add_subdirectory(tests)
endif(CLHPP_BUILD_TESTS)
