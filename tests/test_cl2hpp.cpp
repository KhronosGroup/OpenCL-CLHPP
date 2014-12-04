#define TEST_CL2
// Want to support 2.0 but also test that 1.1 is ok
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_HPP_MINIMUM_OPENCL_VERSION 100

// Needs to be included before CL/opencl.h to set appropriate defines
// Seemingly not for MSVC, though.
#if !defined(_MSC_VER)
#include <CL/cl2.hpp>
#endif

extern "C" {
    // Headers required to make unity use mocks correctly
    #include <unity.h>
    #include <cmock.h>
    #include "Mockcl.h"
    #include <string.h>

    // Declarations for the benefit of the runner generator
    // to import tests from test_clhpp.cpp
    // Saves duplication of tests and the runner generator
    // does not process through the include below
    void testCompareExchange();
    void testFence();
    void testCopyContextNonNull();
    void testMoveAssignContextNonNull();
    void testMoveAssignContextNull();
    void testMoveConstructContextNonNull();
    void testMoveConstructContextNull();
    void testContextGetDevices1_1();
    void testContextGetDevices1_2();
    void testContextFromType();
    void testMoveAssignCommandQueueNonNull();
    void testMoveAssignCommandQueueNull();
    void testMoveConstructCommandQueueNonNull();
    void testMoveConstructCommandQueueNull();
    void testCommandQueueGetContext();
    void testCommandQueueGetDevice1_1();
    void testCommandQueueGetDevice1_2();
    void testCommandQueueFromSpecifiedContext();
    void testCopyDeviceNonNull1_1();
    void testCopyDeviceNonNull1_2();
    void testCopyDeviceFromNull1_1();
    void testCopyDeviceFromNull1_2();
    void testCopyDeviceToNull1_1();
    void testCopyDeviceToNull1_2();
    void testCopyDeviceSelf();
    void testAssignDeviceNull();
    void testMoveAssignDeviceNonNull();
    void testMoveAssignDeviceNull();
    void testMoveConstructDeviceNonNull();
    void testMoveConstructDeviceNull();
    void testDestroyDevice1_1();
    void testDestroyDevice1_2();
    void testMoveAssignBufferNonNull();
    void testMoveAssignBufferNull();
    void testMoveConstructBufferNonNull();
    void testMoveConstructBufferNull();
    void testBufferConstructorContextIterator();
    void testBufferConstructorQueueIterator();
    void testGetImageInfoBuffer();
    void testGetImageInfoBufferNull();
    void testGetImageInfoBufferOverwrite();
    void testConstructImageFromBuffer();
    void testMoveAssignImage2DNonNull();
    void testMoveAssignImage2DNull();
    void testMoveConstructImage2DNonNull();
    void testMoveConstructImage2DNull();
    void testCreateImage2D_1_1();
    void testCreateImage2D_1_2();
    void testMoveAssignImage3DNonNull();
    void testMoveAssignImage3DNull();
    void testMoveConstructImage3DNonNull();
    void testMoveConstructImage3DNull();
    void testCreateImage3D_1_1();
    void testCreateImage3D_1_2();
    void testMoveAssignKernelNonNull();
    void testMoveAssignKernelNull();
    void testMoveConstructKernelNonNull();
    void testMoveConstructKernelNull();
    void testKernelSetArgScalar();
    void testKernelSetArgVector();
    void testKernelSetArgMem();
    void testKernelSetArgLocal();
    void testCopyHostToBuffer();
    void testGetBuildInfo();
    void testCreateSubDevice();
    void testGetContextInfoDevices();
} // extern "C"

#include "test_clhpp.cpp"


extern "C" {

static cl_mem clCreateImage_testCreateImage2DFromBuffer_2_0(
    cl_context context,
    cl_mem_flags flags,
    const cl_image_format *image_format,
    const cl_image_desc *image_desc,
    void *host_ptr,
    cl_int *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_NOT_NULL(image_format);
    TEST_ASSERT_NOT_NULL(image_desc);
    TEST_ASSERT_NULL(host_ptr);
    TEST_ASSERT_EQUAL_HEX(CL_MEM_OBJECT_IMAGE2D, image_desc->image_type);

    // Return the passed buffer as the cl_mem and success for the error code
    if (errcode_ret) {
        *errcode_ret = CL_SUCCESS;
    }
    return image_desc->buffer;
}

void testCreateImage2DFromBuffer_2_0()
{
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_2_0);
    clCreateImage_StubWithCallback(clCreateImage_testCreateImage2DFromBuffer_2_0);
    clReleaseMemObject_ExpectAndReturn(make_mem(0), CL_SUCCESS);
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);

    cl_int err;
    cl::Context context(make_context(0));

    // Create buffer
    // Create image from buffer
    cl::Buffer buffer(make_mem(0));
    cl::Image2D imageFromBuffer(
        context,
        cl::ImageFormat(CL_R, CL_FLOAT), buffer, 64, 32, 256, &err);

    TEST_ASSERT_EQUAL_PTR(buffer(), imageFromBuffer());
    TEST_ASSERT_EQUAL(CL_SUCCESS, err);

    buffer() = NULL;
}

static cl_mem clCreateImage_testCreateImage2D_2_0(
    cl_context context,
    cl_mem_flags flags,
    const cl_image_format *image_format,
    const cl_image_desc *image_desc,
    void *host_ptr,
    cl_int *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL(0, num_calls);
    TEST_ASSERT_EQUAL_PTR(make_context(0), context);
    TEST_ASSERT_EQUAL_HEX(CL_MEM_READ_WRITE, flags);

    TEST_ASSERT_NOT_NULL(image_format);
    TEST_ASSERT_EQUAL_HEX(CL_RGBA, image_format->image_channel_order);
    TEST_ASSERT_EQUAL_HEX(CL_FLOAT, image_format->image_channel_data_type);

    TEST_ASSERT_NOT_NULL(image_desc);
    TEST_ASSERT_EQUAL_HEX(CL_MEM_OBJECT_IMAGE2D, image_desc->image_type);
    TEST_ASSERT_EQUAL(64, image_desc->image_width);
    TEST_ASSERT_EQUAL(32, image_desc->image_height);
    TEST_ASSERT_EQUAL(256, image_desc->image_row_pitch);
    TEST_ASSERT_EQUAL(0, image_desc->num_mip_levels);
    TEST_ASSERT_EQUAL(0, image_desc->num_samples);
    TEST_ASSERT_NULL(image_desc->buffer);

    TEST_ASSERT_NULL(host_ptr);

    if (errcode_ret != NULL)
        *errcode_ret = CL_SUCCESS;
    return make_mem(0);
}

static cl_mem clCreateImage_testCreateImage2DFromImage_2_0(
    cl_context context,
    cl_mem_flags flags,
    const cl_image_format *image_format,
    const cl_image_desc *image_desc,
    void *host_ptr,
    cl_int *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_NOT_NULL(image_format);
    TEST_ASSERT_NOT_NULL(image_desc);
    TEST_ASSERT_NULL(host_ptr);
    TEST_ASSERT_EQUAL_HEX(CL_MEM_OBJECT_IMAGE2D, image_desc->image_type);

    // Return the passed buffer as the cl_mem and success for the error code
    if (errcode_ret) {
        *errcode_ret = CL_SUCCESS;
    }
    return image_desc->buffer;
}

static cl_int clGetImageInfo_testCreateImage2DFromImage_2_0(
    cl_mem image,
    cl_image_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    TEST_ASSERT_INT_WITHIN(6, 0, num_calls);
    return CL_SUCCESS;
}

void testCreateImage2DFromImage_2_0()
{
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_2_0);
    clCreateImage_StubWithCallback(clCreateImage_testCreateImage2D_2_0);


    cl_int err;
    cl::Context context(make_context(0));

    // As in 1.2 2D image test, needed as source for image-from-image
    cl::Image2D image(
        context, CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_RGBA, CL_FLOAT), 64, 32, 256, NULL, &err);

    TEST_ASSERT_EQUAL(CL_SUCCESS, err);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), image());

    // Continue state for next phase
    clGetImageInfo_StubWithCallback(clGetImageInfo_testCreateImage2DFromImage_2_0);
    clCreateImage_StubWithCallback(clCreateImage_testCreateImage2DFromImage_2_0);
    clReleaseMemObject_ExpectAndReturn(make_mem(0), CL_SUCCESS);
    clReleaseMemObject_ExpectAndReturn(make_mem(0), CL_SUCCESS);
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);

    // Create 2D image from 2D Image with a new channel order
    cl::Image2D imageFromImage(
        context,
        CL_sRGB,
        image,
        &err
        );

    TEST_ASSERT_EQUAL(CL_SUCCESS, err);
    TEST_ASSERT_EQUAL_PTR(image(), imageFromImage());

    //imageFromImage() = NULL;
    //image() = NULL;
    //context() = NULL;
}

} // extern "C"
