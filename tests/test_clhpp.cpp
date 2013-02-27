#include <CL/cl.hpp>
#undef _UP

#include <map>

extern "C"
{
#include <unity.h>
#include <cmock.h>
#include "Mockcl.h"
#include <string.h>

/// Creates fake IDs that are easy to identify

static inline cl_platform_id make_platform_id(int index)
{
    return (cl_platform_id) (size_t) (0x1a1a1a1a + index);
}

static inline cl_context make_context(int index)
{
    return (cl_context) (size_t) (0xcccccccc + index);
}

static inline cl_device_id make_device_id(int index)
{
    return (cl_device_id) (size_t) (0xdededede + index);
}

static inline cl_mem make_mem(int index)
{
    return (cl_mem) (size_t) (0x33333333 + index);
}

/* Pools of pre-allocated wrapped objects for tests. There is no device pool,
 * because there is no way to know whether the test wants the device to be
 * reference countable or not.
 */
static const int POOL_MAX = 5;
static cl::Platform platformPool[POOL_MAX];
static cl::Context contextPool[POOL_MAX];

void setUp()
{
    /* We reach directly into the objects rather than using assignment to
     * avoid the reference counting functions from being called.
     */
    for (int i = 0; i < POOL_MAX; i++)
    {
        platformPool[i]() = make_platform_id(i);
        contextPool[i]() = make_context(i);
    }
}

void tearDown()
{
    /* Wipe out the internal state to avoid a release call being made */
    for (int i = 0; i < POOL_MAX; i++)
    {
        platformPool[i]() = NULL;
        contextPool[i]() = NULL;
    }
}

/****************************************************************************
 * Stub functions shared by multiple tests
 ****************************************************************************/

/**
 * Stub implementation of clGetDeviceInfo that just returns the first platform.
 */
static cl_int clGetDeviceInfo_platform(
    cl_device_id id,
    cl_device_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;

    TEST_ASSERT_EQUAL_HEX(CL_DEVICE_PLATFORM, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= sizeof(cl_platform_id));
    if (param_value_size_ret != NULL)
        *param_value_size_ret = sizeof(cl_platform_id);
    if (param_value != NULL)
        *(cl_platform_id *) param_value = make_platform_id(0);
    return CL_SUCCESS;
}

/**
 * Stub implementation of clGetPlatformInfo that returns a specific version.
 * It also checks that the id is the zeroth platform.
 */
static cl_int clGetPlatformInfo_version(
    cl_platform_id id,
    cl_platform_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    const char *version)
{
    size_t bytes = strlen(version) + 1;

    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_PTR(make_platform_id(0), id);
    TEST_ASSERT_EQUAL_HEX(CL_PLATFORM_VERSION, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= bytes);
    if (param_value_size_ret != NULL)
        *param_value_size_ret = bytes;
    if (param_value != NULL)
        strcpy((char *) param_value, version);
    return CL_SUCCESS;
}

/**
 * A stub for clGetPlatformInfo that will only support querying
 * CL_PLATFORM_VERSION, and will return version 1.1.
 */
static cl_int clGetPlatformInfo_version_1_1(
    cl_platform_id id,
    cl_platform_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;
    return clGetPlatformInfo_version(
        id, param_name, param_value_size, param_value,
        param_value_size_ret, "OpenCL 1.1 Mock");
}

/**
 * A stub for clGetPlatformInfo that will only support querying
 * CL_PLATFORM_VERSION, and will return version 1.2.
 */
static cl_int clGetPlatformInfo_version_1_2(
    cl_platform_id id,
    cl_platform_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;
    return clGetPlatformInfo_version(
        id, param_name, param_value_size, param_value,
        param_value_size_ret, "OpenCL 1.2 Mock");
}

/****************************************************************************
 * Tests for cl::Context
 ****************************************************************************/

void test_CopyContextNonNull()
{
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);
    clRetainContext_ExpectAndReturn(make_context(1), CL_SUCCESS);

    contextPool[0] = contextPool[1];
}

/****************************************************************************
 * Tests for cl::Device
 ****************************************************************************/

void testCopyDeviceNonNull1_1()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);

    cl::Device d0(make_device_id(0));
    cl::Device d1(make_device_id(1));
    d0 = d1;
}

void testCopyDeviceNonNull1_2()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);
    clRetainDevice_ExpectAndReturn(make_device_id(1), CL_SUCCESS);

    cl::Device d0(make_device_id(0));
    cl::Device d1(make_device_id(1));
    d0 = d1;

    // Prevent destructor from interfering with the test
    d0() = NULL;
    d1() = NULL;
}

void testCopyDeviceFromNull1_1()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);
    // No other calls expected

    cl::Device d(make_device_id(0));
    d = cl::Device();
}

void testCopyDeviceFromNull1_2()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);

    cl::Device d(make_device_id(0));
    d = cl::Device();
}

void testCopyDeviceToNull1_1()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);
    // No other calls expected

    cl::Device d0;
    cl::Device d1(make_device_id(0));
    d0 = d1;
}

void testCopyDeviceToNull1_2()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clRetainDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);

    cl::Device d0;
    cl::Device d1(make_device_id(0));
    d0 = d1;

    // Prevent destructor from interfering with the test
    d0() = NULL;
    d1() = NULL;
}

void testCopyDeviceSelf()
{
    // Use 1.2 to check the retain/release calls
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);
    clRetainDevice_ExpectAndReturn(make_device_id(1), CL_SUCCESS);

    cl::Device d0(make_device_id(0));
    cl::Device d1(make_device_id(1));
    d0 = d1;

    // Prevent destructor from interfering with the test
    d0() = NULL;
    d1() = NULL;
}

void testAssignDeviceNull()
{
    // Any version will do here
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);

    cl::Device d(make_device_id(0));
    d = (cl_device_id) NULL;
}

void testDestroyDevice1_1()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);
    // No other calls expected

    cl::Device d(make_device_id(0));
}

void testDestroyDevice1_2()
{
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);

    cl::Device d(make_device_id(0));
}

/****************************************************************************
 * Tests for cl::Image1DBuffer
 ****************************************************************************/

/**
 * Stub for querying CL_IMAGE_BUFFER and returning make_mem(1).
 */
cl_int clGetImageInfo_testGetImageInfoBuffer(
    cl_mem image, cl_image_info param_name,
    size_t param_value_size, void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL(0, num_calls);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), image);
    TEST_ASSERT_EQUAL_HEX(CL_IMAGE_BUFFER, param_name);
    TEST_ASSERT_EQUAL(sizeof(cl_mem), param_value_size);

    if (param_value != NULL)
    {
        *(cl_mem *) param_value = make_mem(1);
    }
    if (param_value_size_ret != NULL)
        *param_value_size_ret = sizeof(cl_mem);
    return CL_SUCCESS;
}

/**
 * Reference count for memory object make_mem(1) used by retain and release
 */
static int testGetImageInfoBuffer_memObjectRefCount = 0;

/**
 * Stub to reference count instances of memory object make_mem(1)
 */
cl_int clRetainMemObject_testGetImageInfoBuffer(cl_mem memobj, int num_calls)
{
    TEST_ASSERT_EQUAL(make_mem(1), memobj);
    ++testGetImageInfoBuffer_memObjectRefCount;
    return CL_SUCCESS;
}

/**
 * Stub to reference count instances of memory object make_mem(1)
 */
cl_int clReleaseMemObject_testGetImageInfoBuffer(cl_mem memobj, int num_calls)
{
    TEST_ASSERT_EQUAL(make_mem(1), memobj);
    --testGetImageInfoBuffer_memObjectRefCount;
    TEST_ASSERT(testGetImageInfoBuffer_memObjectRefCount >= 0);
    return CL_SUCCESS;
}

void testGetImageInfoBuffer()
{
    clGetImageInfo_StubWithCallback(clGetImageInfo_testGetImageInfoBuffer);
    clRetainMemObject_StubWithCallback(clRetainMemObject_testGetImageInfoBuffer);
    clReleaseMemObject_StubWithCallback(clReleaseMemObject_testGetImageInfoBuffer);

    cl::Image1DBuffer image(make_mem(0));
    const cl::Buffer &buffer = image.getImageInfo<CL_IMAGE_BUFFER>();
    TEST_ASSERT_EQUAL_PTR(make_mem(1), buffer());
    // Ref count should be 1 here because buffer has not been destroyed yet
    TEST_ASSERT_EQUAL(testGetImageInfoBuffer_memObjectRefCount, 1);

    // prevent destructor from interfering with the test
    image() = NULL;
}

/**
 * Stub for querying CL_IMAGE_BUFFER and returning NULL.
 */
cl_int clGetImageInfo_testGetImageInfoBufferNull(
    cl_mem image, cl_image_info param_name,
    size_t param_value_size, void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL(0, num_calls);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), image);
    TEST_ASSERT_EQUAL_HEX(CL_IMAGE_BUFFER, param_name);
    TEST_ASSERT_EQUAL(sizeof(cl_mem), param_value_size);

    if (param_value != NULL)
    {
        *(cl_mem *) param_value = NULL;
    }
    if (param_value_size_ret != NULL)
        *param_value_size_ret = sizeof(cl_mem);
    return CL_SUCCESS;
}

void testGetImageInfoBufferNull()
{
    clGetImageInfo_StubWithCallback(clGetImageInfo_testGetImageInfoBufferNull);

    cl::Image2D image(make_mem(0));
    cl::Buffer buffer = image.getImageInfo<CL_IMAGE_BUFFER>();
    TEST_ASSERT_NULL(buffer());

    // prevent destructor from interfering with the test
    image() = NULL;
}

void testGetImageInfoBufferOverwrite()
{
    clGetImageInfo_StubWithCallback(clGetImageInfo_testGetImageInfoBuffer);
    clRetainMemObject_ExpectAndReturn(make_mem(1), CL_SUCCESS);
    clReleaseMemObject_ExpectAndReturn(make_mem(2), CL_SUCCESS);

    cl::Image2D image(make_mem(0));
    cl::Buffer buffer(make_mem(2));
    cl_int status = image.getImageInfo(CL_IMAGE_BUFFER, &buffer);
    TEST_ASSERT_EQUAL(CL_SUCCESS, status);
    TEST_ASSERT_EQUAL_PTR(make_mem(1), buffer());

    // prevent destructor from interfering with the test
    image() = NULL;
    buffer() = NULL;
}

/**
 * A stub for clCreateImage that creates an image from a buffer
 * passing the buffer's cl_mem straight through.
 */
cl_mem clCreateImage_image1dbuffer(
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
    TEST_ASSERT_EQUAL_HEX(CL_MEM_OBJECT_IMAGE1D_BUFFER, image_desc->image_type);

    // Return the passed buffer as the cl_mem
    return image_desc->buffer;
}

void testConstructImageFromBuffer()
{
    const size_t width = 64;
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);
    clCreateImage_StubWithCallback(clCreateImage_image1dbuffer);
    clReleaseMemObject_ExpectAndReturn(make_mem(0), CL_SUCCESS);
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);

    cl::Context context(make_context(0));
    cl::Buffer buffer(make_mem(0));
    cl::Image1DBuffer image(
        context,
        CL_MEM_READ_ONLY,
        cl::ImageFormat(CL_R, CL_SIGNED_INT32),
        width,
        buffer);

    // Check that returned buffer matches the original
    TEST_ASSERT_EQUAL_HEX(buffer(), image());

    buffer() = NULL;
}

} // extern "C"
