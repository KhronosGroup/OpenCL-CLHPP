cl.hpp base sources.

Three components:
  input_cl.hpp: 
    Acts as the master source for the header. 
	The reason for doing it this way is to generate an appropriate set of functors with varying argument counts without assuming variadic template support in the header.

  gen_cl_hpp.py:
    A generator script written in python to convert input_cl.hpp into cl.hpp, generating the functor expansions as necessary.

  examples:
    A simple example application using the very basic features of the header and generating cl.hpp dynamically through the build system.

  CMake scripts:
    A build system that both generates the example and drives generation of cl.hpp.

After building the example, cl.hpp appears in examples/include. This is the final version that may be moved (with versioning confirmation) into the Khronos/headers tree.
