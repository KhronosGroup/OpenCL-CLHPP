#include <CL/cl.hpp>
#undef _UP

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

void test_CopyContextNonNull()
{
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);
    clRetainContext_ExpectAndReturn(make_context(1), CL_SUCCESS);

    contextPool[0] = contextPool[1];
}

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

} // extern "C"
