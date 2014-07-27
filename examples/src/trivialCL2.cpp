#define CL_HPP_ENABLE_EXCEPTIONS
//#define CL_HPP_NO_STD_STRING
#define  _VARIADIC_MAX 10
#include <CL/cl2.hpp>
#include <iostream>
#include <vector>

const int numElements = 32;

int main(void)
{

    cl::Program vectorAddProgram(
        std::string(
            "kernel void vectorAdd(global const int *inputA, global const int *inputB, global int *output){output[get_global_id(0)] = inputA[get_global_id(0)] + inputB[get_global_id(0)];}")
        , true);

    auto vectorAddKernel = 
        cl::make_kernel<
            cl::Buffer&,
            cl::Buffer&,
            cl::Buffer&
            >( vectorAddProgram, "vectorAdd" );

    std::vector<int> inputA(numElements, 1);
    std::vector<int> inputB(numElements, 2);
    std::vector<int> output(numElements, 0xdeadbeef);
    cl::Buffer inputABuffer(begin(inputA), end(inputA), true);
    cl::Buffer inputBBuffer(begin(inputB), end(inputB), true);
    cl::Buffer outputBuffer(begin(output), end(output), false);

    vectorAddKernel(
        cl::EnqueueArgs(
            cl::NDRange(numElements),
            cl::NDRange(numElements)),
        inputABuffer,
        inputBBuffer,
        outputBuffer);

    cl::copy(outputBuffer, begin(output), end(output));

    std::cout << "Output:\n";
    for( int i = 1; i < numElements; ++i ) {
        std::cout << "\t" << output[i] << "\n";
    }
    std::cout << "\n\n";

}
