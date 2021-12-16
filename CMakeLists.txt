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
option(BUILD_TESTS "Build Unit Tests" ON)
set(THREADS_PREFER_PTHREAD_FLAG ON CACHE BOOL
  "find_package(Threads) preference. Recommendation is to keep default value."
)

if(NOT TARGET OpenCL::Headers)
  find_package(OpenCLHeaders REQUIRED)
endif()
if(NOT TARGET OpenCL::OpenCL)
  find_package(OpenCLICDLoader REQUIRED)
endif()
if(BUILD_TESTS)
  enable_language(C)
  find_package(Threads REQUIRED)
  find_program(RUBY_EXECUTABLE ruby REQUIRED)
endif()

add_library(HeadersCpp INTERFACE)
add_library(OpenCL::HeadersCpp ALIAS HeadersCpp)
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

if(BUILD_TESTS)
  add_subdirectory(tests)
endif(BUILD_TESTS)
