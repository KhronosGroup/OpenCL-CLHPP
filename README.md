# OpenCL<sup>TM</sup> API C++ bindings (cl.hpp and cl2.hpp)

Doxgen documentation for the cl2.hpp header is available here:

  http://khronosgroup.github.io/OpenCL-CLHPP/

Components:

  * `include/CL/cl2.hpp`:
    The latest, maintained, version of the C++ bindings. It should work with all
    versions of OpenCL (including 1.x). This is what most users will want.

  * `input_cl.hpp` (DEPRECATED):
    Acts as the master source for the 1.x version of the header
    (`include/CL/cl.hpp`).
    The reason for doing it this way is to generate an appropriate set of
    functors with varying argument counts without assuming variadic template
    support in the header.
    This version of the C++ bindings is deprecated and is no longer maintained;
    it is strongly recommended to switch to `cl2.hpp` if possible.

  * `gen_cl_hpp.py`:
    A generator script written in python to convert `input_cl.hpp` into `cl.hpp`,
    generating the functor expansions as necessary.
    `cl2.hpp` does not require this as it uses variadic templates expanded in
    the compiler.

  * `docs`:
    Doxygen file used to generate HTML documentation for `cl2.hpp`.

  * `examples`:
    A simple example application using the very basic features of the header
    and generating `cl.hpp` dynamically through the build system.

  * `tests`:
    A (very small, incomplete) set of regression tests. Building the tests
    requires Python, Ruby, Unity and CMock. For the last two we use
    [Unity 2.1.0](https://github.com/ThrowTheSwitch/Unity/releases/tag/v2.1.0)
    and [CMock top-of-tree from Github](https://github.com/ThrowTheSwitch/CMock)
    (the version 2.0.204 on Sourceforge does not work).

  * `CMakeLists.txt`:
    A build system that both generates the example and drives generation of `cl.hpp`.

To get external dependencies needed for testing, use `--recursive` when cloning
the repository, or run `git submodule update --init`.

You may need to tell CMake where to find the OpenCL headers and libraries,
using the variables `OPENCL_INCLUDE_DIR` and `OPENCL_LIB_DIR`.

These can be set either as environment variables, or on the cmake command line
using the syntax `-D<VAR>=<VALUE>`.

The following is an example set of commands to checkout and build the C++
bindings (adapt paths as required):

```
    git clone --recursive https://github.com/KhronosGroup/OpenCL-CLHPP
    cd OpenCL-CLHPP
    mkdir build
    cd build
    cmake -DOPENCL_INCLUDE_DIR=/path/to/OpenCL/headers -DOPENCL_LIB_DIR=/path/to/OpenCL/library
    make
    make test
```

After building, the headers appear in `build/include/CL/`.

If Doxygen is available, you can generate HTML documentation by typing `make docs`.
