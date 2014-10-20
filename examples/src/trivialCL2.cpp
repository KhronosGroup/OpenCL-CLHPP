#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

#include <iostream>
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
        "typedef struct { global int *bar; } Foo; kernel void vectorAdd(global const Foo* aNum, global const int *inputA, global const int *inputB, global int *output){output[get_global_id(0)] = inputA[get_global_id(0)] + inputB[get_global_id(0)] + *(aNum->bar);}")
        , false);
    try {
        // TODO: 
        vectorAddProgram.build("-cl-std=CL2.0");
    }
    catch (...) {
        // TODO Need to get the device
        auto v = vectorAddProgram.getInfo<CL_PROGRAM_DEVICES>();
        std::cout << v[0].getInfo<CL_DEVICE_VERSION>() << "\n";
        std::string buildInfo = vectorAddProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(v[0]);
        std::cout << buildInfo << "\n";
    }

    typedef struct { int *bar; } Foo;
	auto vectorAddKernel =
		cl::KernelFunctor<
        std::shared_ptr<Foo>,
        int*,
        cl::coarse_svm_vector_class<int>&,
        cl::Buffer&
        >(vectorAddProgram, "vectorAdd");
    
    // TODO: Test once default context setting is available
    //std::shared_ptr<int> anSVMIntDefaultContext = cl::allocate_svm<int, cl::SVMTraitCoarse>();

    // Store pointer to pointer here to test clSetKernelExecInfo
	std::shared_ptr<int> anSVMInt = cl::allocate_svm<int, cl::SVMTraitCoarse>(theContext);
    *anSVMInt = 5;
    cl::SVMAllocator<int, cl::SVMTraitCoarse> svmAlloc(theContext);
    std::shared_ptr<Foo> fooPointer = std::allocate_shared<Foo>(svmAlloc);
    fooPointer->bar = anSVMInt.get();

    std::vector<int, cl::SVMAllocator<int, cl::SVMTraitCoarse>> inputA(numElements, 1, svmAlloc);
    
    // TODO: Try this once we have default contexts again
    //cl::coarse_svm_vector_class<int> inputB(numElements, 2);

    cl::coarse_svm_vector_class<int> inputB(numElements, 2, svmAlloc);
        
    std::vector<int> output(numElements, 0xdeadbeef);

    cl::Buffer outputBuffer(theQueue, begin(output), end(output), false);
	
    //cl::vector_class<void*> ptrs;
    //ptrs.push_back(static_cast<void*>(fooPointer.get()));
    //vectorAddKernel.setSVMPointers(ptrs);
    vectorAddKernel.setSVMPointers(fooPointer.get());
	cl_int error;
    vectorAddKernel(
        cl::EnqueueArgs(
            theQueue,
            cl::NDRange(numElements),
            cl::NDRange(numElements)),
        fooPointer,
        inputA.data(),
        inputB,
		outputBuffer,
		error);

    cl::copy(theQueue, outputBuffer, begin(output), end(output));

    std::cout << "Output:\n";
    for( int i = 1; i < numElements; ++i ) {
        std::cout << "\t" << output[i] << "\n";
    }
    std::cout << "\n\n";

}
