# OpenCL<sup>TM</sup> API C++ bindings

Doxgen documentation for the bindings is available here:

  http://khronosgroup.github.io/OpenCL-CLHPP/

Components:

  * `include/CL/opencl.hpp`:
    The latest, maintained, version of the C++ bindings. It should work with all
    versions of OpenCL (including 1.x). This is what most users will want.

  * `include/CL/cl2.hpp`:
    Includes `opencl.hpp` and emits a warning, for backwards compability.

  * `docs`:
    Doxygen file used to generate HTML documentation for `opencl.hpp`.

  * `examples`:
    A simple example application using the very basic features of the bindings.

  * `tests`:
    A (very small, incomplete) set of regression tests. Building the tests
    requires Python, Ruby, Unity and CMock. For the last two we use
    [Unity 2.1.0](https://github.com/ThrowTheSwitch/Unity/releases/tag/v2.1.0)
    and [CMock top-of-tree from Github](https://github.com/ThrowTheSwitch/CMock)
    (the version 2.0.204 on Sourceforge does not work).

  * `CMakeLists.txt`:
    Build system for the examples and tests and logic for the bindings
    installation.

## Build Instructions

> While the C++ Headers can be built and installed in isolation, it is part of the [OpenCL SDK](https://github.com/KhronosGroup/OpenCL-SDK). If looking for streamlined build experience and a complete development package, refer to the SDK build instructions instead of the following guide.

### Dependencies

The C++ Headers require:

- the [OpenCL Headers](https://github.com/KhronosGroup/OpenCL-Headers/).
  - It is recommended to install the headers via CMake, however a convenience shorthand is provided. Providing `OPENCL_CLHPP_HEADERS_DIR` to CMake, one may specify the location of OpenCL Headers. By default, the C++ Headers will look for OpenCL Headers under `${OPENCL_DIST_DIR}/include`.
- the [OpenCL-ICD-Loader](https://github.com/KhronosGroup/OpenCL-ICD-Loader/) when building the examples
  - It is recommended to install the ICD loader via CMake, however a convenience shorthand is provided. Providing `OPENCL_CLHPP_LOADER_DIR` to CMake, one may specify the location of the OpenCL ICD loader. By default, the C++ headers will look for OpenCL ICD loader under `${OPENCL_DIST_DIR}/lib`.
- The C++ Headers uses CMake for its build system.
If CMake is not provided by your build system or OS package manager, please consult the [CMake website](https://cmake.org).
- The unit tests require [CMock](https://github.com/ThrowTheSwitch/CMock) and [Unity](https://github.com/ThrowTheSwitch/Unity). To get these external dependencies, use `--recursive` when cloning
the repository, or run `git submodule update --init --recursive`.
- Generating the mock input requires [Ruby](https://www.ruby-lang.org/en/).
- Generating the docs requires Doxygen. When it is available, you can generate HTML documentation by building the `docs` target.

### Example Build

1. Clone this repo, the OpenCL ICD Loader and the OpenCL Headers:

        git clone --recursive https://github.com/KhronosGroup/OpenCL-CLHPP
        git clone https://github.com/KhronosGroup/OpenCL-ICD-Loader
        git clone https://github.com/KhronosGroup/OpenCL-Headers

1. Install OpenCL Headers CMake package

        cmake -D CMAKE_INSTALL_PREFIX=./OpenCL-Headers/install -S ./OpenCL-Headers -B ./OpenCL-Headers/build 
        cmake --build ./OpenCL-Headers/build --target install

1. Build and install OpenCL ICD Loader CMake package. _(Note that `CMAKE_PREFIX_PATH` need to be an absolute path. Update as needed.)_

        cmake -D CMAKE_PREFIX_PATH=/absolute/path/to/OpenCL-Headers/install -D CMAKE_INSTALL_PREFIX=./OpenCL-ICD-Loader/install -S ./OpenCL-ICD-Loader -B ./OpenCL-ICD-Loader/build 
        cmake --build ./OpenCL-ICD-Loader/build --target install

1. Build and install OpenCL C++ Headers CMake package.

        cmake -D CMAKE_PREFIX_PATH="/absolute/path/to/OpenCL-Headers/install;/absolute/path/to/OpenCL-ICD-Loader/install" -D CMAKE_INSTALL_PREFIX=./OpenCL-CLHPP/install -S ./OpenCL-CLHPP -B ./OpenCL-CLHPP/build 
        cmake --build ./OpenCL-CLHPP/build --target install

### Example Use

Example CMake invocation

```bash
cmake -D CMAKE_PREFIX_PATH="/chosen/install/prefix/of/headers;/chosen/install/prefix/of/loader;/chosen/install/prefix/of/cppheaders" /path/to/opencl/app
```

and sample `CMakeLists.txt`

```cmake
cmake_minimum_required(VERSION 3.0)
cmake_policy(VERSION 3.0...3.18.4)
project(proj)
add_executable(app main.cpp)
find_package(OpenCLHeaders REQUIRED)
find_package(OpenCLICDLoader REQUIRED)
find_package(OpenCLHeadersCpp REQUIRED)
target_link_libraries(app PRIVATE OpenCL::Headers OpenCL::OpenCL OpenCL::HeadersCpp)
```