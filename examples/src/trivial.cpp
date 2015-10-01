#define __CL_ENABLE_EXCEPTIONS
#define __NO_STD_STRING
#define  _VARIADIC_MAX 10
#include <CL/cl.hpp>
#include <iostream>
#include <vector>

const int numElements = 32;

int main(void)
{
	cl::Program addProgram(
        cl::STRING_CLASS(
		"int add(int a, int b) { return a + b; }")
		, false);
	cl::Program vectorWrapper(
        cl::STRING_CLASS(
		"int add(int a, int b); kernel void vectorAdd(global const int *inputA, global const int *inputB, global int *output){output[get_global_id(0)] = add(inputA[get_global_id(0)], inputB[get_global_id(0)]);}")
		, false);
	std::vector<cl::Program> programs;
	addProgram.compile();
	vectorWrapper.compile();		
	
    cl::STRING_CLASS s = addProgram.getInfo<CL_PROGRAM_SOURCE>();
    
	programs.push_back(addProgram);
	programs.push_back(vectorWrapper);
	cl::Program vectorAddProgram = cl::linkProgram(programs);

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
