#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#include <CL/cl2.hpp>
#include <iostream>
#include <vector>

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

    cl::Program vectorAddProgram(
        std::string(
        "global int globalA;"
        "kernel void updateGlobal(){"
        "  globalA = 75;"
        "}"
        "kernel void vectorAdd(global const int *inputA, global const int *inputB, global int *output, int val, write_only pipe int outPipe){"
        "  output[get_global_id(0)] = inputA[get_global_id(0)] + inputB[get_global_id(0)] + val;"
        "  write_pipe(outPipe, &val);"
        "  queue_t default_queue = get_default_queue(); "
        "  ndrange_t ndrange = ndrange_1D(get_global_size(0), get_global_size(0)); "
        "  enqueue_kernel(default_queue, CLK_ENQUEUE_FLAGS_WAIT_KERNEL, ndrange, "
        "    ^{"
        "      output[get_global_size(0)+get_global_id(0)] = inputA[get_global_size(0)+get_global_id(0)] + inputB[get_global_size(0)+get_global_id(0)] + globalA;"
        "    });"
        "}")       
        , false);
#if defined(CL_HPP_ENABLE_EXCEPTIONS)
    try {
        vectorAddProgram.build("-cl-std=CL2.0");
    }
    catch (...) {
        std::string bl = vectorAddProgram.getBuildInfo<CL_PROGRAM_BUILD_LOG>(cl::Device::getDefault());
        std::cerr << bl << std::endl;
        
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

    // Get and run kernel that initializes the program-scope global
    // A test for kernels that take no arguments
    auto program2Kernel =
        cl::KernelFunctor<>(vectorAddProgram, "updateGlobal");
    program2Kernel(
        cl::EnqueueArgs(
        cl::NDRange(1)));
    
    auto vectorAddKernel =
        cl::KernelFunctor<
            cl::Buffer&,
            cl::Buffer&,
            cl::Buffer&,
            int,
            cl::Pipe&
            >(vectorAddProgram, "vectorAdd");

    std::vector<int> inputA(numElements, 1);
    std::vector<int> inputB(numElements, 2);
    std::vector<int> output(numElements, 0xdeadbeef);
    cl::Buffer inputABuffer(begin(inputA), end(inputA), true);
    cl::Buffer inputBBuffer(begin(inputB), end(inputB), true);
    cl::Buffer outputBuffer(begin(output), end(output), false);
    cl::Pipe aPipe(sizeof(cl_int), numElements / 2);
    // Unfortunately, there is no way to check for a default or know if a kernel needs one
    // so the user has to create one
    // We can't preemptively do so on device creation because they cannot then replace it
    cl::DeviceCommandQueue deviceQueue = cl::DeviceCommandQueue::makeDefault(
        cl::Context::getDefault(), cl::Device::getDefault());
	
    vectorAddKernel(
        cl::EnqueueArgs(
            cl::NDRange(numElements/2),
            cl::NDRange(numElements/2)),
        inputABuffer,
        inputBBuffer,
        outputBuffer,
        3,
        aPipe);

	cl_int error;
	vectorAddKernel(
		cl::EnqueueArgs(
	      	cl::NDRange(numElements/2),
		    cl::NDRange(numElements/2)),
		inputABuffer,
		inputBBuffer,
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

    std::cout << "Output:\n";
    for (int i = 1; i < numElements; ++i) {
        std::cout << "\t" << output[i] << "\n";
    }
    std::cout << "\n\n";

    return 0;
}
