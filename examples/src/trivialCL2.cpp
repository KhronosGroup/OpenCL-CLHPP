#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/cl2.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

const int numElements = 32;

int main(void)
{
    // Select an OpenCL 2.0 platform and create a context
    cl::vector_class<cl::Platform> v;
    cl::Platform::get(&v);
    cl::Platform thePlatform;
    for (cl::Platform &p : v) {
        cl::string_class s = p.getInfo<CL_PLATFORM_VERSION>();
        if (s.compare("OpenCL 2.0")>=0) {
            thePlatform = p;
        }
    }
    cl::vector_class<cl::Device> devices;
    thePlatform.getDevices(CL_DEVICE_TYPE_ALL, &devices);
    cl::Context theContext(devices);

    // TODO: Need to be able to set defaults
    // setting a default context will simplify a lot of the code below

    cl::CommandQueue theQueue(theContext, 0, 0);


    cl::Program vectorAddProgram(
        theContext,
        std::string(
        "kernel void vectorAdd(global const int *inputA, global const int *inputB, global int *output){output[get_global_id(0)] = inputA[get_global_id(0)] + inputB[get_global_id(0)];}")
        , true);
	
	auto vectorAddKernel =
		cl::KernelFunctor<
		cl::Buffer&,
		cl::Buffer&,
		cl::Buffer&
        >(vectorAddProgram, "vectorAdd");
    
    // TODO: Test once default context setting is available
    //std::shared_ptr<int> anSVMIntDefaultContext = cl::allocate_svm<int, cl::SVMTraitCoarse>();

	std::shared_ptr<int> anSVMInt = cl::allocate_svm<int, cl::SVMTraitCoarse>(theContext);
	//cl::SVMAllocator<int, cl::SVMTraitCoarse> svmAlloc;
	//std::shared_ptr<int> anotherSVMInt = std::allocate_shared<int>(svmAlloc);
    // er.. .this one isn't actually SVM is it.
	//std::vector<int> aVector(10);
	//std::vector<int, cl::SVMAllocator<int, cl::SVMTraitCoarse>> anSVMVector(10, 0, svmAlloc);
	// Todo: template typedef for SVMVector taking T and trait (with a default) and giving a vector<T, allocator<T, Trait>>
	// cl::SVMVector<int> anotherSVMVector(10);
	// cl::SVMVector<int, cl::SVMTraitCoarse> yetAnotherSVMVector(10, 0);

    std::vector<int> inputA(numElements, 1);
    std::vector<int> inputB(numElements, 2);
    std::vector<int> output(numElements, 0xdeadbeef);
    cl::Buffer inputABuffer(theQueue, begin(inputA), end(inputA), true);
    cl::Buffer inputBBuffer(theQueue, begin(inputB), end(inputB), true);
    cl::Buffer outputBuffer(theQueue, begin(output), end(output), false);
	
    vectorAddKernel(
        cl::EnqueueArgs(
            theQueue,
            cl::NDRange(numElements),
            cl::NDRange(numElements)),
        inputABuffer,
        inputBBuffer,
        outputBuffer);

	cl_int error;
	vectorAddKernel(
        cl::EnqueueArgs(
            theQueue,
		    cl::NDRange(numElements),
		    cl::NDRange(numElements)),
		inputABuffer,
		inputBBuffer,
		outputBuffer,
		error);

    cl::copy(theQueue, outputBuffer, begin(output), end(output));

    std::cout << "Output:\n";
    for (int i = 1; i < numElements; ++i) {
        std::cout << "\t" << output[i] << "\n";
    }
    std::cout << "\n\n";

}
