#include <CL/cl.hpp>
#undef _UP

extern "C"
{
#include <unity.h>
#include <cmock.h>
#include "Mockcl.h"
#include <string.h>

void setUp()
{
}

void tearDown()
{
}

void test_CopyContextNonNull()
{
    cl_context id1 = (cl_context) 0xdeadbeaf;
    cl_context id2 = (cl_context) 0xcafebabe;

    clReleaseContext_ExpectAndReturn(id1, CL_SUCCESS);
    clRetainContext_ExpectAndReturn(id2, CL_SUCCESS);

    cl::Context c1(id1);
    cl::Context c2(id2);
    c1 = c2;

    // Prevent the destructor from interfering with the test
    c1() = NULL;
    c2() = NULL;
}

/**
 * Stub implementation of clGetDeviceInfo that returns a specific device
 * version.
 */
static cl_int clGetDeviceInfo_version(
    cl_device_id id,
    cl_device_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    const char *version)
{
    size_t bytes = strlen(version) + 1;

    TEST_ASSERT_NOT_NULL(id);
    TEST_ASSERT_EQUAL_HEX(CL_DEVICE_VERSION, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= bytes);
    if (param_value_size_ret != NULL)
        *param_value_size_ret = bytes;
    if (param_value != NULL)
        strcpy((char *) param_value, version);
    return CL_SUCCESS;
}

/**
 * A stub for clGetDeviceInfo that will only support querying
 * CL_DEVICE_VERSION, and will return version 1.1.
 */
static cl_int clGetDeviceInfo_version_1_1(
    cl_device_id id,
    cl_device_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;
    return clGetDeviceInfo_version(
        id, param_name, param_value_size, param_value,
        param_value_size_ret, "OpenCL 1.1 Mock");
}

/**
 * A stub for clGetDeviceInfo that will only support querying
 * CL_DEVICE_VERSION, and will return version 1.2.
 */
static cl_int clGetDeviceInfo_version_1_2(
    cl_device_id id,
    cl_device_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;
    return clGetDeviceInfo_version(
        id, param_name, param_value_size, param_value,
        param_value_size_ret, "OpenCL 1.2 Mock");
}

void testCopyDeviceNonNull1_1()
{
    cl_device_id id1 = (cl_device_id) 0xdeadbeaf;
    cl_device_id id2 = (cl_device_id) 0xcafebabe;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_1);

    cl::Device d1(id1);
    cl::Device d2(id2);
    d1 = d2;
}

void testCopyDeviceNonNull1_2()
{
    cl_device_id id1 = (cl_device_id) 0xdeadbeaf;
    cl_device_id id2 = (cl_device_id) 0xcafebabe;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(id1, CL_SUCCESS);
    clRetainDevice_ExpectAndReturn(id2, CL_SUCCESS);

    cl::Device d1(id1);
    cl::Device d2(id2);
    d1 = d2;
    // Prevent destructor from interfering with the test
    d1() = NULL;
    d2() = NULL;
}

void testCopyDeviceFromNull1_1()
{
    cl_device_id id1 = (cl_device_id) 0xdeadbeaf;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_1);
    // No other calls expected

    cl::Device d1(id1);
    cl::Device d2;
    d1 = d2;
}

void testCopyDeviceFromNull1_2()
{
    cl_device_id id1 = (cl_device_id) 0xdeadbeaf;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(id1, CL_SUCCESS);

    cl::Device d1(id1);
    cl::Device d2;
    d1 = d2;
}

void testCopyDeviceToNull1_1()
{
    cl_device_id id2 = (cl_device_id) 0xdeadbeef;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_1);
    // No other calls expected

    cl::Device d1;
    cl::Device d2(id2);
    d1 = d2;
    // Prevent destructor from interfering with the test
    d1() = NULL;
    d2() = NULL;
}

void testCopyDeviceToNull1_2()
{
    cl_device_id id2 = (cl_device_id) 0xdeadbeef;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_2);
    clRetainDevice_ExpectAndReturn(id2, CL_SUCCESS);

    cl::Device d1;
    cl::Device d2(id2);
    d1 = d2;
    // Prevent destructor from interfering with the test
    d1() = NULL;
    d2() = NULL;
}

void testCopyDeviceSelf()
{
    cl_device_id id = (cl_device_id) 0xdeadbeef;

    // Use 1.2 to check the retain/release calls
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(id, CL_SUCCESS);
    clRetainDevice_ExpectAndReturn(id, CL_SUCCESS);

    cl::Device d1(id);
    cl::Device d2(id);
    d1 = d2;
    // Prevent destructor from interfering with the test
    d1() = NULL;
    d2() = NULL;
}

void testAssignDeviceNull()
{
    cl::Device d;

    // Any version will do here
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_1);

    d = (cl_device_id) NULL;
}

void testDestroyDevice1_1()
{
    cl_device_id id = (cl_device_id) 0xdeadbeaf;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_1);
    // No other calls expected

    cl::Device d(id);
}

void testDestroyDevice1_2()
{
    cl_device_id id = (cl_device_id) 0xdeadbeaf;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_version_1_2);
    clReleaseDevice_ExpectAndReturn(id, CL_SUCCESS);

    cl::Device d(id);
}

} // extern "C"
