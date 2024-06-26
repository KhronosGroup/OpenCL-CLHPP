cmake_minimum_required(VERSION 3.16)

project(PkgConfigTest
  LANGUAGES CXX
)

find_package(OpenCL
  REQUIRED
  CONFIG
  COMPONENTS
    HeadersCpp
    Headers
    OpenCL
)

add_executable(${PROJECT_NAME}
  ../pkgconfig.cpp
)

target_link_libraries(${PROJECT_NAME}
  PRIVATE
    OpenCL::HeadersCpp
    OpenCL::Headers
    OpenCL::OpenCL
)

target_compile_definitions(${PROJECT_NAME}
  PRIVATE
    CL_HPP_ENABLE_EXCEPTIONS
    CL_HPP_TARGET_OPENCL_VERSION=300
)

include(CTest)

add_test(
  NAME ${PROJECT_NAME}
  COMMAND ${PROJECT_NAME}
)
