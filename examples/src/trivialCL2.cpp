#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

//#define CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY
#include <CL/cl2.hpp>
#include <iostream>
#include <vector>
#include <memory>
#include <algorithm>

const int numElements = 32;

int main(void)
{
    // Filter for a 2.0 platform and set it as the default
    std::vector<cl::Platform> platforms;
    cl::Platform::get(&platforms);
    cl::Platform plat;
    for (auto &p : platforms) {
        std::string platver = p.getInfo<CL_PLATFORM_VERSION>();
        if (platver.find("OpenCL 2.") != std::string::npos) {
            plat = p;
        }
    }
    if (plat() == 0)  {
        std::cout << "No OpenCL 2.0 platform found.";
        return -1;
    }

    cl::Platform newP = cl::Platform::setDefault(plat);
    if (newP != plat) {
        std::cout << "Error setting default platform.";
        return -1;
    }

#if defined(CL_HPP_ENABLE_EXCEPTIONS)
    cl::Program errorProgram(
        std::string(
        "sakfdjnksajfnksajnfsa")
        , false);
    try {
        errorProgram.build("-cl-std=CL2.0");
    }
    catch (...) {
        // Print build info for all devices
        cl_int buildErr = CL_SUCCESS;
        auto buildInfo = errorProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(&buildErr);
        std::cerr << "Errors for failed build for all devices" << std::endl;
        for (auto &pair : buildInfo) {
            std::cerr << "Device: " << pair.first.getInfo<CL_DEVICE_NAME>() << std::endl << pair.second << std::endl << std::endl;
        }
    }


    cl::Program errorProgramException(
        std::string(
        "sakfdjnksajfnksajnfsa")
        , false);
    try {
        errorProgramException.build("-cl-std=CL2.0");
    }
    catch (const cl::BuildError &err) {
        // Print build info for all devices
        auto buildInfo = err.getBuildLog();
        std::cerr << "Errors for failed build for all devices from thrown exception" << std::endl;
        for (auto &pair : buildInfo) {
            std::cerr << "Device: " << pair.first.getInfo<CL_DEVICE_NAME>() << std::endl << pair.second << std::endl << std::endl;
        }
    }
#endif // #if defined(CL_HPP_ENABLE_EXCEPTIONS)


    std::string kernel1{"global int globalA;"
        "kernel void updateGlobal(){"
        "  globalA = 75;"
        "}"};
    std::string kernel2{
        "typedef struct { global int *bar; } Foo; kernel void vectorAdd(global const Foo* aNum, global const int *inputA, global const int *inputB, global int *output, int val, write_only pipe int outPipe){"
        "  output[get_global_id(0)] = inputA[get_global_id(0)] + inputB[get_global_id(0)] + val + *(aNum->bar);"
        "  write_pipe(outPipe, &val);"
        "  queue_t default_queue = get_default_queue(); "
        "  ndrange_t ndrange = ndrange_1D(get_global_size(0), get_global_size(0)); "
        "  enqueue_kernel(default_queue, CLK_ENQUEUE_FLAGS_WAIT_KERNEL, ndrange, "
        "    ^{"
        "      output[get_global_size(0)+get_global_id(0)] = inputA[get_global_size(0)+get_global_id(0)] + inputB[get_global_size(0)+get_global_id(0)] + globalA;"
        "    });"
        "}" };
#if defined(CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY)
    // Old interface style
    cl::Program::Sources programStrings;
    programStrings.push_back(std::pair<const char*, size_t>(kernel1.data(), kernel1.length()));
    programStrings.push_back(std::pair<const char*, size_t>(kernel2.data(), kernel2.length()));
#else // #if defined(CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY)
    // New simpler string interface style
    std::vector<std::string> programStrings;
    programStrings.push_back(kernel1);        
    programStrings.push_back(kernel2);
#endif // #if defined(CL_HPP_ENABLE_PROGRAM_CONSTRUCTION_FROM_ARRAY_COMPATIBILITY)
    cl::Program vectorAddProgram(
        programStrings);
#if defined(CL_HPP_ENABLE_EXCEPTIONS)
    try {
        vectorAddProgram.build("-cl-std=CL2.0");
    }
    catch (...) {
        // Print build info for all devices
        cl_int buildErr = CL_SUCCESS;
        auto buildInfo = vectorAddProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(&buildErr);
        for (auto &pair : buildInfo) {
            std::cerr << pair.second << std::endl << std::endl;
        }
        
        return 1;
    }
#else // #if defined(CL_HPP_ENABLE_EXCEPTIONS)
    cl_int buildErr = vectorAddProgram.build("-cl-std=CL2.0");
    if (buildErr != CL_SUCCESS) {
        std::cerr << "Build error: " << buildErr << "\n";
        return -1;
    }
#endif // #if defined(CL_HPP_ENABLE_EXCEPTIONS)



    typedef struct { int *bar; } Foo;

    // Get and run kernel that initializes the program-scope global
    // A test for kernels that take no arguments
    auto program2Kernel =
        cl::KernelFunctor<>(vectorAddProgram, "updateGlobal");
    program2Kernel(
        cl::EnqueueArgs(
        cl::NDRange(1)));


    //////////////////
    // SVM allocations

    // Store pointer to pointer here to test clSetKernelExecInfo
    // Code using cl namespace allocators etc as a test
    // std::shared_ptr etc should work fine too
    cl::pointer_class<int> anSVMInt = cl::allocate_svm<int, cl::SVMTraitCoarse>();
    *anSVMInt = 5;
    cl::SVMAllocator<int, cl::SVMTraitCoarse> svmAlloc;
    cl::pointer_class<Foo> fooPointer = cl::allocate_pointer<Foo>(svmAlloc);
    fooPointer->bar = anSVMInt.get();

    std::vector<int, cl::SVMAllocator<int, cl::SVMTraitCoarse>> inputA(numElements, 1, svmAlloc);
    
    cl::coarse_svm_vector_class<int> inputB(numElements, 2, svmAlloc);

    //
    //////////////


    std::vector<int> output(numElements, 0xdeadbeef);
    cl::Buffer outputBuffer(begin(output), end(output), false);
    cl::Pipe aPipe(sizeof(cl_int), numElements / 2);
    // Unfortunately, there is no way to check for a default or know if a kernel needs one
    // so the user has to create one
    // We can't preemptively do so on device creation because they cannot then replace it
    cl::DeviceCommandQueue deviceQueue = cl::DeviceCommandQueue::makeDefault(
        cl::Context::getDefault(), cl::Device::getDefault());
    
    auto vectorAddKernel =
        cl::KernelFunctor<
        cl::pointer_class<Foo>,
        int*,
        cl::coarse_svm_vector_class<int>&,
        cl::Buffer,
        int,
        cl::Pipe&
        >(vectorAddProgram, "vectorAdd");


    // Only the last of these will actually be used
    // but this will check that the API is working for all
    // of them
    cl::vector_class<void*> ptrs;
    ptrs.push_back(static_cast<void*>(fooPointer.get()));
    vectorAddKernel.setSVMPointers(ptrs);
    vectorAddKernel.setSVMPointers(fooPointer.get());
    vectorAddKernel.setSVMPointers(fooPointer);

	cl_int error;
	vectorAddKernel(
        cl::EnqueueArgs(
            cl::NDRange(numElements/2),
            cl::NDRange(numElements/2)),
        fooPointer,
        inputA.data(),
        inputB,
        outputBuffer,
        3,
        aPipe,
        error);

    cl::copy(outputBuffer, begin(output), end(output));

    cl::Device d = cl::Device::getDefault();
    std::cout << "Max pipe args: " << d.getInfo<CL_DEVICE_MAX_PIPE_ARGS>() << "\n";
    std::cout << "Max pipe active reservations: " << d.getInfo<CL_DEVICE_PIPE_MAX_ACTIVE_RESERVATIONS>() << "\n";
    std::cout << "Max pipe packet size: " << d.getInfo<CL_DEVICE_PIPE_MAX_PACKET_SIZE>() << "\n";
    std::cout << "Device SVM capabilities: " << d.getInfo<CL_DEVICE_SVM_CAPABILITIES>() << "\n";
    std::cout << "\tCL_DEVICE_SVM_COARSE_GRAIN_BUFFER = " << CL_DEVICE_SVM_COARSE_GRAIN_BUFFER << "\n";
    std::cout << "\tCL_DEVICE_SVM_FINE_GRAIN_BUFFER = " << CL_DEVICE_SVM_FINE_GRAIN_BUFFER << "\n";
    std::cout << "\tCL_DEVICE_SVM_FINE_GRAIN_SYSTEM = " << CL_DEVICE_SVM_FINE_GRAIN_SYSTEM << "\n";
    std::cout << "\tCL_DEVICE_SVM_ATOMICS = " << CL_DEVICE_SVM_ATOMICS << "\n";

    auto v = vectorAddProgram.getInfo<CL_PROGRAM_BINARIES>();    
    auto v2 = vectorAddProgram.getInfo<CL_PROGRAM_BINARY_SIZES>();
    std::vector<std::vector<unsigned char>> v3;
    std::vector<size_t> v4;
    vectorAddProgram.getInfo(CL_PROGRAM_BINARIES, &v3);
    vectorAddProgram.getInfo(CL_PROGRAM_BINARY_SIZES, &v4);

    std::cout << "Binaries: " << v.size() << "\n";
    std::cout << "Binary sizes: " << v2.size() << "\n";
    for (size_t s : v2) {
        std::cout << "\t" << s << "\n";
    }

    std::cout << "Output:\n";
    for (int i = 1; i < numElements; ++i) {
        std::cout << "\t" << output[i] << "\n";
    }
    std::cout << "\n\n";

    return 0;
}
