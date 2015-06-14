#ifdef TEST_CL2
# include <CL/cl2.hpp>
# define TEST_RVALUE_REFERENCES
# define VECTOR_CLASS cl::vector
# define STRING_CLASS cl::string

#else
# include <CL/cl.hpp>
// cl.hpp will switch to C++11 atomics in certain cases, for testing internal use we need to include support here too
# if (_MSC_VER >= 1700) || (__cplusplus >= 201103L)
#  define TEST_RVALUE_REFERENCES
#  define TEST_CPP11_ATOMICS
# endif // (_MSC_VER >= 1700) || (__cplusplus >= 201103L)

#endif // !CL_HPP_TEST_CL2

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

static inline cl_mem make_mem(int index)
{
    return (cl_mem) (size_t) (0x33333333 + index);
}

static inline cl_command_queue make_command_queue(int index)
{
    return (cl_command_queue) (size_t) (0xc0c0c0c0 + index);
}

static inline cl_kernel make_kernel(int index)
{
    return (cl_kernel) (size_t) (0xcececece + index);
}

static inline cl_program make_program(int index)
{
    return (cl_program)(size_t)(0xcfcfcfcf + index);
}

/* Pools of pre-allocated wrapped objects for tests. There is no device pool,
 * because there is no way to know whether the test wants the device to be
 * reference countable or not.
 */
static const int POOL_MAX = 5;
static cl::Platform platformPool[POOL_MAX];
static cl::Context contextPool[POOL_MAX];
static cl::CommandQueue commandQueuePool[POOL_MAX];
static cl::Buffer bufferPool[POOL_MAX];
static cl::Image2D image2DPool[POOL_MAX];
static cl::Image3D image3DPool[POOL_MAX];
static cl::Kernel kernelPool[POOL_MAX];

/****************************************************************************
 * Stub functions shared by multiple tests
 ****************************************************************************/

/**
 * Stub implementation of clGetCommandQueueInfo that returns the first context.
 */
static cl_int clGetCommandQueueInfo_context(
    cl_command_queue id,
    cl_command_queue_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void)num_calls;


    TEST_ASSERT_EQUAL_HEX(CL_QUEUE_CONTEXT, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= sizeof(cl_context));
    if (param_value_size_ret != NULL)
        *param_value_size_ret = sizeof(cl_context);
    if (param_value != NULL)
        *(cl_context *)param_value = make_context(0);

    return CL_SUCCESS;
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
 * Stub implementation of clGetContextInfo that just returns the first device.
 */
static cl_int clGetContextInfo_device(
    cl_context id,
    cl_context_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;

    TEST_ASSERT_EQUAL_HEX(CL_CONTEXT_DEVICES, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= sizeof(cl_device_id));
    if (param_value_size_ret != NULL)
        *param_value_size_ret = sizeof(cl_device_id);
    if (param_value != NULL)
        *(cl_device_id *) param_value = make_device_id(0);
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

/**
 * A stub for clGetPlatformInfo that will only support querying
 * CL_PLATFORM_VERSION, and will return version 2.0.
 */
static cl_int clGetPlatformInfo_version_2_0(
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
        param_value_size_ret, "OpenCL 2.0 Mock");
}

/* Simulated reference counts. The table points to memory held by the caller.
 * This makes things simpler in the common case of only one object to be
 * reference counted.
 */
class RefcountTable
{
private:
    int n; // number of objects
    void * const *objects; // object IDs
    int *refcounts;        // current refcounts

    int find(void *object)
    {
        int idx = 0;
        while (idx < n && objects[idx] != object)
            idx++;
        TEST_ASSERT(idx < n);
        TEST_ASSERT(refcounts[idx] > 0); // otherwise object has been destroyed
        return idx;
    }

public:
    RefcountTable() : n(0), objects(NULL), refcounts(NULL) {}

    void init(int n, void * const *objects, int *refcounts)
    {
        this->n = n;
        this->objects = objects;
        this->refcounts = refcounts;
    }

    void reset()
    {
        init(0, NULL, NULL);
    }

    cl_int retain(void *object)
    {
        int idx = find(object);
        ++refcounts[idx];
        return CL_SUCCESS;
    }

    cl_int release(void *object)
    {
        int idx = find(object);
        --refcounts[idx];
        return CL_SUCCESS;
    }
};

/* Stubs for retain/release calls that track reference counts. The stubs
 * check that the reference count never becomes negative and that a zero
 * reference count is never incremented.
 *
 * Use the prepareRefcount* calls to set up the global variables first.
 */

#define MAKE_REFCOUNT_STUBS(cl_type, retainfunc, releasefunc, table) \
    static RefcountTable table; \
    static cl_int retainfunc ## _refcount(cl_type object, int num_calls) \
    { \
        (void) num_calls; \
        return table.retain(object); \
    } \
    static cl_int releasefunc ## _refcount(cl_type object, int num_calls) \
    { \
        (void) num_calls; \
        return table.release(object); \
    } \
    static void prepare_ ## table(int n, cl_type const *objects, int *refcounts) \
    { \
        table.init(n, (void * const *) objects, refcounts); \
        retainfunc ## _StubWithCallback(retainfunc ## _refcount); \
        releasefunc ## _StubWithCallback(releasefunc ## _refcount); \
    }

MAKE_REFCOUNT_STUBS(cl_device_id, clRetainDevice, clReleaseDevice, deviceRefcounts)
MAKE_REFCOUNT_STUBS(cl_context, clRetainContext, clReleaseContext, contextRefcounts)
MAKE_REFCOUNT_STUBS(cl_mem, clRetainMemObject, clReleaseMemObject, memRefcounts)

/* The indirection through MAKE_MOVE_TESTS2 with a prefix parameter is to
 * prevent the simple-minded parser from Unity from identifying tests from the
 * macro value.
 */
#ifdef TEST_RVALUE_REFERENCES
#define MAKE_MOVE_TESTS2(prefix, type, makeFunc, releaseFunc, pool) \
    void prefix ## MoveAssign ## type ## NonNull() \
    { \
        releaseFunc ## _ExpectAndReturn(makeFunc(0), CL_SUCCESS); \
        pool[0] = std::move(pool[1]); \
        TEST_ASSERT_EQUAL_PTR(makeFunc(1), pool[0]()); \
        TEST_ASSERT_NULL(pool[1]()); \
    } \
    \
    void prefix ## MoveAssign ## type ## Null() \
    { \
        pool[0]() = NULL; \
        pool[0] = std::move(pool[1]); \
        TEST_ASSERT_EQUAL_PTR(makeFunc(1), pool[0]()); \
        TEST_ASSERT_NULL(pool[1]()); \
    } \
    \
    void prefix ## MoveConstruct ## type ## NonNull() \
    { \
        cl::type tmp(std::move(pool[0])); \
        TEST_ASSERT_EQUAL_PTR(makeFunc(0), tmp()); \
        TEST_ASSERT_NULL(pool[0]()); \
        tmp() = NULL; \
    } \
    \
    void prefix ## MoveConstruct ## type ## Null() \
    { \
        cl::type empty; \
        cl::type tmp(std::move(empty)); \
        TEST_ASSERT_NULL(tmp()); \
        TEST_ASSERT_NULL(empty()); \
    }
#else
#define MAKE_MOVE_TESTS2(prefix, type, makeFunc, releaseFunc, pool) \
    void prefix ## MoveAssign ## type ## NonNull() {} \
    void prefix ## MoveAssign ## type ## Null() {} \
    void prefix ## MoveConstruct ## type ## NonNull() {} \
    void prefix ## MoveConstruct ## type ## Null() {}
#endif // !TEST_RVALUE_REFERENCES
#define MAKE_MOVE_TESTS(type, makeFunc, releaseFunc, pool) \
    MAKE_MOVE_TESTS2(test, type, makeFunc, releaseFunc, pool)

void setUp()
{
    /* We reach directly into the objects rather than using assignment to
     * avoid the reference counting functions from being called.
     */
    for (int i = 0; i < POOL_MAX; i++)
    {
        platformPool[i]() = make_platform_id(i);
        contextPool[i]() = make_context(i);
        commandQueuePool[i]() = make_command_queue(i);
        bufferPool[i]() = make_mem(i);
        image2DPool[i]() = make_mem(i);
        image3DPool[i]() = make_mem(i);
        kernelPool[i]() = make_kernel(i);
    }

    deviceRefcounts.reset();
    contextRefcounts.reset();
    memRefcounts.reset();
}

void tearDown()
{
    /* Wipe out the internal state to avoid a release call being made */
    for (int i = 0; i < POOL_MAX; i++)
    {
        platformPool[i]() = NULL;
        contextPool[i]() = NULL;
        commandQueuePool[i]() = NULL;
        bufferPool[i]() = NULL;
        image2DPool[i]() = NULL;
        image3DPool[i]() = NULL;
        kernelPool[i]() = NULL;
    }
}

/****************************************************************************
 * Tests for atomic wrappers
 ****************************************************************************/

void testCompareExchange()
{
#ifndef TEST_CL2
    /* This just tests that a compare-and-swap happens - there is no reliable
     * way to ensure that it is atomic and performs a memory barrier.
     */

    // Test success case
#ifdef TEST_CPP11_ATOMICS
    std::atomic<int> dest(123);
#else // #if defined(TEST_CPP11_ATOMICS)
    volatile int dest = 123;
#endif // #if defined(TEST_CPP11_ATOMICS)
    int old = cl::detail::compare_exchange(&dest, 456, 123);
    TEST_ASSERT_EQUAL(456, dest);
    TEST_ASSERT_EQUAL(123, old);

    // Test failure case
    dest = 234;
    old = cl::detail::compare_exchange(&dest, 345, 456);
    TEST_ASSERT_EQUAL(234, dest);
    TEST_ASSERT_EQUAL(234, old);
#endif // !TEST_CL2
}

void testFence()
{
#ifndef TEST_CL2
    /* No reliable way to test that it actually does what it says on the tin.
     * Just test that it can be called without exploding.
     */
    cl::detail::fence();
#endif // !TEST_CL2
}

/****************************************************************************
 * Tests for cl::Context
 ****************************************************************************/

void testCopyContextNonNull()
{
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);
    clRetainContext_ExpectAndReturn(make_context(1), CL_SUCCESS);

    contextPool[0] = contextPool[1];
    TEST_ASSERT_EQUAL_PTR(make_context(1), contextPool[0]());
}

void testMoveAssignContextNonNull();
void testMoveAssignContextNull();
void testMoveConstructContextNonNull();
void testMoveConstructContextNull();
MAKE_MOVE_TESTS(Context, make_context, clReleaseContext, contextPool)

/// Stub for querying CL_CONTEXT_DEVICES that returns two devices
static cl_int clGetContextInfo_testContextGetDevices(
    cl_context context,
    cl_context_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;
    TEST_ASSERT_EQUAL_PTR(make_context(0), context);
    TEST_ASSERT_EQUAL_HEX(CL_CONTEXT_DEVICES, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= 2 * sizeof(cl_device_id));
    if (param_value_size_ret != NULL)
        *param_value_size_ret = 2 * sizeof(cl_device_id);
    if (param_value != NULL)
    {
        cl_device_id *devices = (cl_device_id *) param_value;
        devices[0] = make_device_id(0);
        devices[1] = make_device_id(1);
    }
    return CL_SUCCESS;
}

/// Test that queried devices are not refcounted
void testContextGetDevices1_1()
{
    clGetContextInfo_StubWithCallback(clGetContextInfo_testContextGetDevices);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);

    VECTOR_CLASS<cl::Device> devices = contextPool[0].getInfo<CL_CONTEXT_DEVICES>();
    TEST_ASSERT_EQUAL(2, devices.size());
    TEST_ASSERT_EQUAL_PTR(make_device_id(0), devices[0]());
    TEST_ASSERT_EQUAL_PTR(make_device_id(1), devices[1]());
}

/// Test that queried devices are correctly refcounted
void testContextGetDevices1_2()
{
    clGetContextInfo_StubWithCallback(clGetContextInfo_testContextGetDevices);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);

    clRetainDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);
    clRetainDevice_ExpectAndReturn(make_device_id(1), CL_SUCCESS);

    VECTOR_CLASS<cl::Device> devices = contextPool[0].getInfo<CL_CONTEXT_DEVICES>();
    TEST_ASSERT_EQUAL(2, devices.size());
    TEST_ASSERT_EQUAL_PTR(make_device_id(0), devices[0]());
    TEST_ASSERT_EQUAL_PTR(make_device_id(1), devices[1]());

    // Prevent release in the destructor
    devices[0]() = NULL;
    devices[1]() = NULL;
}

// This is used to get a list of all platforms, so expect two calls
// First, return to say we have two platforms
// Then return the two platform id_s
static cl_int clGetPlatformIDs_testContextFromType(
    cl_uint num_entries,
    cl_platform_id *platforms,
    cl_uint *num_platforms,
    int num_calls)
{
    if (num_calls == 0)
    {
        TEST_ASSERT_NULL(platforms);
        TEST_ASSERT_NOT_NULL(num_platforms);
        *num_platforms = 2;
        return CL_SUCCESS;
    }
    else if (num_calls == 1)
    {
        TEST_ASSERT_NOT_NULL(platforms);
        TEST_ASSERT_EQUAL(2, num_entries);
        platforms[0] = make_platform_id(0);
        platforms[1] = make_platform_id(1);
        return CL_SUCCESS;
    }
    else
    {
        TEST_FAIL_MESSAGE("clGetPlatformIDs called too many times");
        return CL_INVALID_VALUE;
    }
}

// Expect three calls to this
// 1. Platform 1, we have no GPUs
// 2. Platform 2, we have two GPUs
// 3. Here are the two cl_device_id's
static cl_int clGetDeviceIDs_testContextFromType(
    cl_platform_id  platform,
    cl_device_type  device_type,
    cl_uint  num_entries,
    cl_device_id  *devices,
    cl_uint  *num_devices,
    int num_calls)
{
    if (num_calls == 0)
    {
        TEST_ASSERT_EQUAL_PTR(make_platform_id(0), platform);
        TEST_ASSERT_EQUAL(CL_DEVICE_TYPE_GPU, device_type);
        TEST_ASSERT_NOT_NULL(num_devices);
        return CL_DEVICE_NOT_FOUND;
    }
    else if (num_calls == 1)
    {
        TEST_ASSERT_EQUAL_PTR(make_platform_id(1), platform);
        TEST_ASSERT_EQUAL(CL_DEVICE_TYPE_GPU, device_type);
        TEST_ASSERT_NOT_NULL(num_devices);
        *num_devices = 2;
        return CL_SUCCESS;
    }
    else if (num_calls == 2)
    {
        TEST_ASSERT_EQUAL_PTR(make_platform_id(1), platform);
        TEST_ASSERT_EQUAL(CL_DEVICE_TYPE_GPU, device_type);
        TEST_ASSERT_EQUAL(2, num_entries);
        TEST_ASSERT_NOT_NULL(devices);
        devices[0] = make_device_id(0);
        devices[1] = make_device_id(1);
        return CL_SUCCESS;
    }
    else
    {
        TEST_FAIL_MESSAGE("clGetDeviceIDs called too many times");
        return CL_INVALID_VALUE;
    }
}

// Stub for clCreateContextFromType
// - expect platform 1 with GPUs and non-null properties
static cl_context clCreateContextFromType_testContextFromType(
    const cl_context_properties  *properties,
    cl_device_type  device_type,
    void  (CL_CALLBACK *pfn_notify) (const char *errinfo,
    const void  *private_info,
    size_t  cb,
    void  *user_data),
    void  *user_data,
    cl_int  *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_NOT_NULL(properties);
    TEST_ASSERT_EQUAL(CL_DEVICE_TYPE_GPU, device_type);
    TEST_ASSERT_EQUAL(CL_CONTEXT_PLATFORM, properties[0]);
    TEST_ASSERT_EQUAL(make_platform_id(1), properties[1]);
    return make_context(0);

}

void testContextFromType()
{
    clGetPlatformIDs_StubWithCallback(clGetPlatformIDs_testContextFromType);
    clGetDeviceIDs_StubWithCallback(clGetDeviceIDs_testContextFromType);
#if defined(TEST_CL2)
    // The cl2.hpp header will perform an extra retain here to be consistent
    // with other APIs retaining runtime-owned objects before releasing them
    clRetainDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);
    clRetainDevice_ExpectAndReturn(make_device_id(1), CL_SUCCESS);
#endif

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);

    // End of scope of vector of devices within constructor
    clReleaseDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);
    clReleaseDevice_ExpectAndReturn(make_device_id(1), CL_SUCCESS);

    clCreateContextFromType_StubWithCallback(clCreateContextFromType_testContextFromType);

    cl::Context context(CL_DEVICE_TYPE_GPU);
    TEST_ASSERT_EQUAL_PTR(make_context(0), context());

    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);

}

/****************************************************************************
 * Tests for cl::CommandQueue
 ****************************************************************************/

void testMoveAssignCommandQueueNonNull();
void testMoveAssignCommandQueueNull();
void testMoveConstructCommandQueueNonNull();
void testMoveConstructCommandQueueNull();
MAKE_MOVE_TESTS(CommandQueue, make_command_queue, clReleaseCommandQueue, commandQueuePool);

// Stub for clGetCommandQueueInfo that returns context 0
static cl_int clGetCommandQueueInfo_testCommandQueueGetContext(
    cl_command_queue command_queue,
    cl_command_queue_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;
    TEST_ASSERT_EQUAL_PTR(make_command_queue(0), command_queue);
    TEST_ASSERT_EQUAL_HEX(CL_QUEUE_CONTEXT, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= sizeof(cl_context));
    if (param_value_size_ret != NULL)
        *param_value_size_ret = sizeof(cl_context);
    if (param_value != NULL)
        *(cl_context *) param_value = make_context(0);
    return CL_SUCCESS;
}

void testCommandQueueGetContext()
{
    cl_context expected = make_context(0);
    int refcount = 1;

    clGetCommandQueueInfo_StubWithCallback(clGetCommandQueueInfo_testCommandQueueGetContext);
    prepare_contextRefcounts(1, &expected, &refcount);

    cl::Context ctx = commandQueuePool[0].getInfo<CL_QUEUE_CONTEXT>();
    TEST_ASSERT_EQUAL_PTR(expected, ctx());
    TEST_ASSERT_EQUAL(2, refcount);

    ctx() = NULL;
}

// Stub for clGetCommandQueueInfo that returns device 0
static cl_int clGetCommandQueueInfo_testCommandQueueGetDevice(
    cl_command_queue command_queue,
    cl_command_queue_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    (void) num_calls;
    TEST_ASSERT_EQUAL_PTR(make_command_queue(0), command_queue);
    TEST_ASSERT_EQUAL_HEX(CL_QUEUE_DEVICE, param_name);
    TEST_ASSERT(param_value == NULL || param_value_size >= sizeof(cl_device_id));
    if (param_value_size_ret != NULL)
        *param_value_size_ret = sizeof(cl_device_id);
    if (param_value != NULL)
        *(cl_device_id *) param_value = make_device_id(0);
    return CL_SUCCESS;
}

void testCommandQueueGetDevice1_1()
{
    cl_device_id expected = make_device_id(0);

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);
    clGetCommandQueueInfo_StubWithCallback(clGetCommandQueueInfo_testCommandQueueGetDevice);

    cl::Device device = commandQueuePool[0].getInfo<CL_QUEUE_DEVICE>();
    TEST_ASSERT_EQUAL_PTR(expected, device());

    device() = NULL;
}

void testCommandQueueGetDevice1_2()
{
    cl_device_id expected = make_device_id(0);
    int refcount = 1;

    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clGetCommandQueueInfo_StubWithCallback(clGetCommandQueueInfo_testCommandQueueGetDevice);
    prepare_deviceRefcounts(1, &expected, &refcount);

    cl::Device device = commandQueuePool[0].getInfo<CL_QUEUE_DEVICE>();
    TEST_ASSERT_EQUAL_PTR(expected, device());
    TEST_ASSERT_EQUAL(2, refcount);

    device() = NULL;
}

// stub for clCreateCommandQueue - returns queue zero
static cl_command_queue clCreateCommandQueue_testCommandQueueFromSpecifiedContext(
    cl_context context,
    cl_device_id device,
    cl_command_queue_properties properties,
    cl_int *errcode_ret,
    int num_calls)
{
    (void) num_calls;
    TEST_ASSERT_EQUAL_PTR(make_context(0), context);
    TEST_ASSERT_EQUAL_PTR(make_device_id(0), device);
    TEST_ASSERT(properties == 0);
    return make_command_queue(0);
}

#if CL_HPP_TARGET_OPENCL_VERSION >= 200
// stub for clCreateCommandQueueWithProperties - returns queue zero
static cl_command_queue clCreateCommandQueueWithProperties_testCommandQueueFromSpecifiedContext(
    cl_context context,
    cl_device_id device,
    const cl_queue_properties *properties,
    cl_int *errcode_ret,
    int num_calls)
{
    (void)num_calls;
    TEST_ASSERT_EQUAL_PTR(make_context(0), context);
    TEST_ASSERT_EQUAL_PTR(make_device_id(0), device);
    TEST_ASSERT(properties[0] == CL_QUEUE_PROPERTIES);
    TEST_ASSERT(properties[1] == 0);
    return make_command_queue(0);
}
#endif // #if CL_HPP_TARGET_OPENCL_VERSION >= 200

void testCommandQueueFromSpecifiedContext()
{
    cl_command_queue expected = make_command_queue(0);
    cl_context expected_context =  make_context(0);
    cl_device_id expected_device = make_device_id(0);

    int context_refcount = 1;
    int device_refcount = 1;
    prepare_contextRefcounts(1, &expected_context, &context_refcount);
    prepare_deviceRefcounts(1, &expected_device, &device_refcount);

    // This is the context we will pass in to test
    cl::Context context = contextPool[0];

    // Assumes the context contains the fi rst device
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);

#if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clCreateCommandQueueWithProperties_StubWithCallback(clCreateCommandQueueWithProperties_testCommandQueueFromSpecifiedContext);
#else // #if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clCreateCommandQueue_StubWithCallback(clCreateCommandQueue_testCommandQueueFromSpecifiedContext);
#endif // #if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);

#if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_2_0);
#else // #if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
#endif // #if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clReleaseCommandQueue_ExpectAndReturn(expected, CL_SUCCESS);

    cl::CommandQueue queue(context);
    TEST_ASSERT_EQUAL_PTR(expected, queue());

    // Context not destroyed yet
    TEST_ASSERT_EQUAL(2, context_refcount);
    // Device object destroyed at end of scope
    TEST_ASSERT_EQUAL(1, device_refcount);

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

// These tests do not use the MAKE_MOVE_TESTS helper because they need to
// check whether the device is reference-countable, and to check that
// the reference-countable flag is correctly moved.
void testMoveAssignDeviceNonNull()
{
#ifdef TEST_RVALUE_REFERENCES
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);

    // Release called when trg overwritten
    clReleaseDevice_ExpectAndReturn(make_device_id(1), CL_SUCCESS);

    cl::Device src(make_device_id(0));
    cl::Device trg(make_device_id(1));
    trg = std::move(src);
    TEST_ASSERT_EQUAL_PTR(make_device_id(0), trg());
    TEST_ASSERT_NULL(src());

    // Prevent destructor from interfering with the test
    trg() = NULL;
#endif
}

void testMoveAssignDeviceNull()
{
#ifdef TEST_RVALUE_REFERENCES
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);

    cl::Device trg;
    cl::Device src(make_device_id(1));
    trg = std::move(src);
    TEST_ASSERT_EQUAL_PTR(make_device_id(1), trg());
    TEST_ASSERT_NULL(src());

    // Prevent destructor from interfering with the test
    trg() = NULL;
#endif
}

void testMoveConstructDeviceNonNull()
{
#ifdef TEST_RVALUE_REFERENCES
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);

    cl::Device src(make_device_id(0));
    cl::Device trg(std::move(src));
    TEST_ASSERT_EQUAL_PTR(make_device_id(0), trg());
    TEST_ASSERT_NULL(src());

    // Prevent destructor from interfering with the test
    trg() = NULL;
#endif
}

void testMoveConstructDeviceNull()
{
#ifdef TEST_RVALUE_REFERENCES
    cl::Device empty;
    cl::Device trg(std::move(empty));
    TEST_ASSERT_NULL(trg());
    TEST_ASSERT_NULL(empty());
#endif
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
 * Tests for cl::Buffer
 ****************************************************************************/

void testMoveAssignBufferNonNull();
void testMoveAssignBufferNull();
void testMoveConstructBufferNonNull();
void testMoveConstructBufferNull();
MAKE_MOVE_TESTS(Buffer, make_mem, clReleaseMemObject, bufferPool);

// Stub of clCreateBuffer for testBufferConstructorContextInterator
// - return the first memory location

static cl_mem clCreateBuffer_testBufferConstructorContextIterator(
    cl_context context,
    cl_mem_flags flags,
    size_t size,
    void *host_ptr,
    cl_int *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL_PTR(make_context(0), context);
    TEST_ASSERT_BITS(CL_MEM_COPY_HOST_PTR, flags, !CL_MEM_COPY_HOST_PTR);
    TEST_ASSERT_BITS(CL_MEM_READ_ONLY, flags, CL_MEM_READ_ONLY);
    TEST_ASSERT_EQUAL(sizeof(int)*1024, size);
    TEST_ASSERT_NULL(host_ptr);
    if (errcode_ret)
        errcode_ret = CL_SUCCESS;
    return make_mem(0);
}

// Declare forward these functions
static void * clEnqueueMapBuffer_testCopyHostToBuffer(
    cl_command_queue command_queue,
    cl_mem buffer,
    cl_bool blocking_map,
    cl_map_flags map_flags,
    size_t offset,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event,
    cl_int *errcode_ret,
    int num_calls);

static cl_int clEnqueueUnmapMemObject_testCopyHostToBuffer(
    cl_command_queue  command_queue ,
    cl_mem  memobj,
    void  *mapped_ptr,
    cl_uint  num_events_in_wait_list ,
    const cl_event  *event_wait_list ,
    cl_event  *event,
    int num_calls);

static cl_int clWaitForEvents_testCopyHostToBuffer(
    cl_uint num_events,
    const cl_event *event_list,
    int num_calls);

static cl_int clReleaseEvent_testCopyHostToBuffer(
    cl_event event,
    int num_calls);

void testBufferConstructorContextIterator()
{
    cl_mem expected = make_mem(0);

    // Assume this context includes make_device_id(0) for stub clGetContextInfo_device
    cl::Context context(make_context(0));

    clCreateBuffer_StubWithCallback(clCreateBuffer_testBufferConstructorContextIterator);
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clRetainDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);

#if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clCreateCommandQueueWithProperties_StubWithCallback(clCreateCommandQueueWithProperties_testCommandQueueFromSpecifiedContext);
#else // #if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clCreateCommandQueue_StubWithCallback(clCreateCommandQueue_testCommandQueueFromSpecifiedContext);
#endif //#if CL_HPP_TARGET_OPENCL_VERSION >= 200
    clReleaseDevice_ExpectAndReturn(make_device_id(0), CL_SUCCESS);
    clEnqueueMapBuffer_StubWithCallback(clEnqueueMapBuffer_testCopyHostToBuffer);
    clEnqueueUnmapMemObject_StubWithCallback(clEnqueueUnmapMemObject_testCopyHostToBuffer);
    clWaitForEvents_StubWithCallback(clWaitForEvents_testCopyHostToBuffer);
    clReleaseEvent_StubWithCallback(clReleaseEvent_testCopyHostToBuffer);
    clReleaseCommandQueue_ExpectAndReturn(make_command_queue(0), CL_SUCCESS);

    std::vector<int> host(1024);

    cl::Buffer buffer(context, host.begin(), host.end(), true);

    TEST_ASSERT_EQUAL_PTR(expected, buffer());

    // Tidy up at end of test
    clReleaseMemObject_ExpectAndReturn(expected, CL_SUCCESS);
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);
}

void testBufferConstructorQueueIterator()
{
    cl_context expected_context = make_context(0);
    int context_refcount = 1;
    cl_mem expected = make_mem(0);

    cl::CommandQueue queue(make_command_queue(0));

    prepare_contextRefcounts(1, &expected_context, &context_refcount);
    clGetCommandQueueInfo_StubWithCallback(clGetCommandQueueInfo_context);
    clCreateBuffer_StubWithCallback(clCreateBuffer_testBufferConstructorContextIterator);

    clEnqueueMapBuffer_StubWithCallback(clEnqueueMapBuffer_testCopyHostToBuffer);
    clEnqueueUnmapMemObject_StubWithCallback(clEnqueueUnmapMemObject_testCopyHostToBuffer);
    clWaitForEvents_StubWithCallback(clWaitForEvents_testCopyHostToBuffer);
    clReleaseEvent_StubWithCallback(clReleaseEvent_testCopyHostToBuffer);

    std::vector<int> host(1024);

    cl::Buffer buffer(queue, host.begin(), host.end(), true);

    TEST_ASSERT_EQUAL_PTR(expected, buffer());
    TEST_ASSERT_EQUAL(1, context_refcount);

    // Tidy up at end of test
    clReleaseMemObject_ExpectAndReturn(expected, CL_SUCCESS);
    clReleaseCommandQueue_ExpectAndReturn(make_command_queue(0), CL_SUCCESS);
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

void testGetImageInfoBuffer()
{
    cl_mem expected = make_mem(1);
    int refcount = 1;

    clGetImageInfo_StubWithCallback(clGetImageInfo_testGetImageInfoBuffer);
    prepare_memRefcounts(1, &expected, &refcount);

    cl::Image1DBuffer image(make_mem(0));
    const cl::Buffer &buffer = image.getImageInfo<CL_IMAGE_BUFFER>();
    TEST_ASSERT_EQUAL_PTR(make_mem(1), buffer());
    // Ref count should be 2 here because buffer has not been destroyed yet
    TEST_ASSERT_EQUAL(2, refcount);

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
    clReleaseMemObject_ExpectAndReturn(make_mem(2), CL_SUCCESS);
    clRetainMemObject_ExpectAndReturn(make_mem(1), CL_SUCCESS);

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
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
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
    TEST_ASSERT_EQUAL_PTR(buffer(), image());

    buffer() = NULL;
}

/****************************************************************************
 * Tests for cl::Image2D
 ****************************************************************************/

void testMoveAssignImage2DNonNull();
void testMoveAssignImage2DNull();
void testMoveConstructImage2DNonNull();
void testMoveConstructImage2DNull();
MAKE_MOVE_TESTS(Image2D, make_mem, clReleaseMemObject, image2DPool);

#ifdef CL_USE_DEPRECATED_OPENCL_1_1_APIS
static cl_mem clCreateImage2D_testCreateImage2D_1_1(
    cl_context context,
    cl_mem_flags flags,
    const cl_image_format *image_format,
    size_t image_width,
    size_t image_height,
    size_t image_row_pitch,
    void *host_ptr,
    cl_int *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL(0, num_calls);
    TEST_ASSERT_EQUAL_PTR(make_context(0), context);
    TEST_ASSERT_EQUAL_HEX(CL_MEM_READ_WRITE, flags);

    TEST_ASSERT_NOT_NULL(image_format);
    TEST_ASSERT_EQUAL_HEX(CL_R, image_format->image_channel_order);
    TEST_ASSERT_EQUAL_HEX(CL_FLOAT, image_format->image_channel_data_type);

    TEST_ASSERT_EQUAL(64, image_width);
    TEST_ASSERT_EQUAL(32, image_height);
    TEST_ASSERT_EQUAL(256, image_row_pitch);
    TEST_ASSERT_NULL(host_ptr);

    if (errcode_ret != NULL)
        *errcode_ret = CL_SUCCESS;
    return make_mem(0);
}
#endif

void testCreateImage2D_1_1()
{
#ifdef CL_USE_DEPRECATED_OPENCL_1_1_APIS
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);
    clCreateImage2D_StubWithCallback(clCreateImage2D_testCreateImage2D_1_1);

    cl_int err;
    cl::Context context;
    context() = make_context(0);
    cl::Image2D image(
        context, CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_R, CL_FLOAT), 64, 32, 256, NULL, &err);

    TEST_ASSERT_EQUAL(CL_SUCCESS, err);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), image());

    context() = NULL;
    image() = NULL;
#endif
}

static cl_mem clCreateImage_testCreateImage2D_1_2(
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
    TEST_ASSERT_EQUAL_HEX(CL_R, image_format->image_channel_order);
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

void testCreateImage2D_1_2()
{
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clCreateImage_StubWithCallback(clCreateImage_testCreateImage2D_1_2);

    cl_int err;
    cl::Context context;
    context() = make_context(0);
    cl::Image2D image(
        context, CL_MEM_READ_WRITE,
        cl::ImageFormat(CL_R, CL_FLOAT), 64, 32, 256, NULL, &err);

    TEST_ASSERT_EQUAL(CL_SUCCESS, err);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), image());

    context() = NULL;
    image() = NULL;
}

/****************************************************************************
 * Tests for cl::Image3D
 ****************************************************************************/

void testMoveAssignImage3DNonNull();
void testMoveAssignImage3DNull();
void testMoveConstructImage3DNonNull();
void testMoveConstructImage3DNull();
MAKE_MOVE_TESTS(Image3D, make_mem, clReleaseMemObject, image3DPool);

#ifdef CL_USE_DEPRECATED_OPENCL_1_1_APIS
static cl_mem clCreateImage3D_testCreateImage3D_1_1(
    cl_context context,
    cl_mem_flags flags,
    const cl_image_format *image_format,
    size_t image_width,
    size_t image_height,
    size_t image_depth,
    size_t image_row_pitch,
    size_t image_slice_pitch,
    void *host_ptr,
    cl_int *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL(0, num_calls);
    TEST_ASSERT_EQUAL_PTR(make_context(0), context);
    TEST_ASSERT_EQUAL_HEX(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, flags);

    TEST_ASSERT_NOT_NULL(image_format);
    TEST_ASSERT_EQUAL_HEX(CL_R, image_format->image_channel_order);
    TEST_ASSERT_EQUAL_HEX(CL_FLOAT, image_format->image_channel_data_type);

    TEST_ASSERT_EQUAL(64, image_width);
    TEST_ASSERT_EQUAL(32, image_height);
    TEST_ASSERT_EQUAL(16, image_depth);
    TEST_ASSERT_EQUAL(256, image_row_pitch);
    TEST_ASSERT_EQUAL(65536, image_slice_pitch);
    TEST_ASSERT_EQUAL_PTR((void *) 0xdeadbeef, host_ptr);

    if (errcode_ret != NULL)
        *errcode_ret = CL_SUCCESS;
    return make_mem(0);
}
#endif

void testCreateImage3D_1_1()
{
#ifdef CL_USE_DEPRECATED_OPENCL_1_1_APIS
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_1);
    clCreateImage3D_StubWithCallback(clCreateImage3D_testCreateImage3D_1_1);

    cl_int err;
    cl::Context context;
    context() = make_context(0);
    cl::Image3D image(
        context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        cl::ImageFormat(CL_R, CL_FLOAT), 64, 32, 16, 256, 65536, (void *) 0xdeadbeef, &err);

    TEST_ASSERT_EQUAL(CL_SUCCESS, err);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), image());

    context() = NULL;
    image() = NULL;
#endif
}

static cl_mem clCreateImage_testCreateImage3D_1_2(
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
    TEST_ASSERT_EQUAL_HEX(CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR, flags);

    TEST_ASSERT_NOT_NULL(image_format);
    TEST_ASSERT_EQUAL_HEX(CL_R, image_format->image_channel_order);
    TEST_ASSERT_EQUAL_HEX(CL_FLOAT, image_format->image_channel_data_type);

    TEST_ASSERT_NOT_NULL(image_desc);
    TEST_ASSERT_EQUAL_HEX(CL_MEM_OBJECT_IMAGE3D, image_desc->image_type);
    TEST_ASSERT_EQUAL(64, image_desc->image_width);
    TEST_ASSERT_EQUAL(32, image_desc->image_height);
    TEST_ASSERT_EQUAL(16, image_desc->image_depth);
    TEST_ASSERT_EQUAL(256, image_desc->image_row_pitch);
    TEST_ASSERT_EQUAL(65536, image_desc->image_slice_pitch);
    TEST_ASSERT_EQUAL(0, image_desc->num_mip_levels);
    TEST_ASSERT_EQUAL(0, image_desc->num_samples);
    TEST_ASSERT_NULL(image_desc->buffer);

    TEST_ASSERT_EQUAL_PTR((void *) 0xdeadbeef, host_ptr);

    if (errcode_ret != NULL)
        *errcode_ret = CL_SUCCESS;
    return make_mem(0);
}

void testCreateImage3D_1_2()
{
    clGetContextInfo_StubWithCallback(clGetContextInfo_device);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_platform);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clCreateImage_StubWithCallback(clCreateImage_testCreateImage3D_1_2);

    cl_int err;
    cl::Context context;
    context() = make_context(0);
    cl::Image3D image(
        context, CL_MEM_READ_WRITE | CL_MEM_COPY_HOST_PTR,
        cl::ImageFormat(CL_R, CL_FLOAT), 64, 32, 16, 256, 65536, (void *) 0xdeadbeef, &err);

    TEST_ASSERT_EQUAL(CL_SUCCESS, err);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), image());

    context() = NULL;
    image() = NULL;
}

/****************************************************************************
 * Tests for cl::Kernel
 ****************************************************************************/
void testMoveAssignKernelNonNull();
void testMoveAssignKernelNull();
void testMoveConstructKernelNonNull();
void testMoveConstructKernelNull();
MAKE_MOVE_TESTS(Kernel, make_kernel, clReleaseKernel, kernelPool);

static cl_int scalarArg;
static cl_int3 vectorArg;

void testKernelSetArgScalar()
{
    scalarArg = 0xcafebabe;
    clSetKernelArg_ExpectAndReturn(make_kernel(0), 3, 4, &scalarArg, CL_SUCCESS);
    kernelPool[0].setArg(3, scalarArg);
}

void testKernelSetArgVector()
{
    vectorArg.s[0] = 0x12345678;
    vectorArg.s[1] = 0x23456789;
    vectorArg.s[2] = 0x87654321;
    clSetKernelArg_ExpectAndReturn(make_kernel(0), 2, 16, &vectorArg, CL_SUCCESS);
    kernelPool[0].setArg(2, vectorArg);
}

void testKernelSetArgMem()
{
    clSetKernelArg_ExpectAndReturn(make_kernel(0), 1, sizeof(cl_mem), &bufferPool[1](), CL_SUCCESS);
    kernelPool[0].setArg(1, bufferPool[1]);
}

void testKernelSetArgLocal()
{
    clSetKernelArg_ExpectAndReturn(make_kernel(0), 2, 123, NULL, CL_SUCCESS);
    kernelPool[0].setArg(2, cl::Local(123));
}

/****************************************************************************
 * Tests for cl::copy
 ****************************************************************************/

// This method should allocate some host accesible memory
// so we must do this ourselves
void *some_host_memory;

static void * clEnqueueMapBuffer_testCopyHostToBuffer(
    cl_command_queue command_queue,
    cl_mem buffer,
    cl_bool blocking_map,
    cl_map_flags map_flags,
    size_t offset,
    size_t size,
    cl_uint num_events_in_wait_list,
    const cl_event *event_wait_list,
    cl_event *event,
    cl_int *errcode_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL_PTR(make_command_queue(0), command_queue);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), buffer);
    TEST_ASSERT_EQUAL(CL_TRUE, blocking_map);
    TEST_ASSERT_EQUAL(CL_MAP_WRITE, map_flags);
    TEST_ASSERT_EQUAL(sizeof(int)*1024, size);

    some_host_memory = malloc(sizeof(int) * 1024);

    // Set the return event
    if (event)
        *event = NULL;

    // Set the return error code
    if (errcode_ret)
        *errcode_ret = CL_SUCCESS;

    return some_host_memory;
}

static cl_int clEnqueueUnmapMemObject_testCopyHostToBuffer(
    cl_command_queue  command_queue ,
    cl_mem  memobj,
    void  *mapped_ptr,
    cl_uint  num_events_in_wait_list ,
    const cl_event  *event_wait_list ,
    cl_event  *event,
    int num_calls)
{
    TEST_ASSERT_EQUAL_PTR(make_command_queue(0), command_queue);
    TEST_ASSERT_EQUAL_PTR(make_mem(0), memobj);
    TEST_ASSERT_EQUAL_PTR(some_host_memory, mapped_ptr);
    TEST_ASSERT_NOT_NULL(event);
    return CL_SUCCESS;
}

static cl_int clWaitForEvents_testCopyHostToBuffer(
    cl_uint num_events,
    const cl_event *event_list,
    int num_calls)
{
    TEST_ASSERT_NOT_NULL(event_list);
    TEST_ASSERT_EQUAL(1, num_events);
    return CL_SUCCESS;
}

static cl_int clReleaseEvent_testCopyHostToBuffer(
    cl_event event,
    int num_calls)
{
    TEST_ASSERT_NOT_NULL(event);
    return CL_SUCCESS;
}

void testCopyHostToBuffer()
{
    cl_context context_expect = make_context(0);
    int context_refcount = 1;
    prepare_contextRefcounts(1, &context_expect, &context_refcount);
    cl::Context context = contextPool[0];

    cl_mem mem_expect = make_mem(0);
    int mem_refcount = 1;
    prepare_memRefcounts(1, &mem_expect, &mem_refcount);
    cl::Buffer buffer(make_mem(0));

    cl_command_queue queue_expect = make_command_queue(0);
    cl::CommandQueue queue(queue_expect);
    clReleaseCommandQueue_ExpectAndReturn(queue_expect, CL_SUCCESS);

    // Returns the pointer to host memory
    clEnqueueMapBuffer_StubWithCallback(clEnqueueMapBuffer_testCopyHostToBuffer);
    clEnqueueUnmapMemObject_StubWithCallback(clEnqueueUnmapMemObject_testCopyHostToBuffer);

    clWaitForEvents_StubWithCallback(clWaitForEvents_testCopyHostToBuffer);
    clReleaseEvent_StubWithCallback(clReleaseEvent_testCopyHostToBuffer);

    std::vector<int> host(1024);
    for (int i = 0; i < 1024; i++)
        host[i] = i;

    cl::copy(queue, host.begin(), host.end(), buffer);

    // Check that the memory was copied to some_host_memory
    TEST_ASSERT_EQUAL_MEMORY(&host[0], some_host_memory, sizeof(int) * 1024);

    free(some_host_memory);

}


/****************************************************************************
* Tests for getBuildInfo
****************************************************************************/

static cl_int clGetDeviceInfo_testGetBuildInfo(
    cl_device_id device,
    cl_device_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL(param_name, CL_DEVICE_PLATFORM);
    TEST_ASSERT_EQUAL(param_value_size, sizeof(cl_platform_id));
    TEST_ASSERT_NOT_EQUAL(param_value, NULL);
    TEST_ASSERT_EQUAL(param_value_size_ret, NULL);
    cl_platform_id temp = make_platform_id(0);
    memcpy(param_value, &temp, sizeof(cl_platform_id));
    return CL_SUCCESS;
}


static  cl_int clGetProgramBuildInfo_testGetBuildInfo(
    cl_program program,
    cl_device_id device,
    cl_program_build_info param_name,
    size_t param_value_size,
    void *param_value,
    size_t *param_value_size_ret,
    int num_calls)
{
    TEST_ASSERT_EQUAL(param_name, CL_PROGRAM_BUILD_LOG);

    const char returnString[] = 
        "This is the string returned by the build info function.";
    if (param_value) {
        ::size_t returnSize = param_value_size;
        if (sizeof(returnString) < returnSize) {
            returnSize = sizeof(returnString);
        }
        memcpy(param_value, returnString, returnSize);
    }
    else {
        if (param_value_size_ret) {
            *param_value_size_ret = sizeof(returnString);
        }
    }

    return CL_SUCCESS;
}

void testGetBuildInfo()
{
    cl_device_id fakeDevice = make_device_id(0);
    clGetDeviceInfo_ExpectAndReturn(fakeDevice, CL_DEVICE_PLATFORM, sizeof(cl_platform_id), NULL, NULL, CL_SUCCESS);
    clGetDeviceInfo_StubWithCallback(clGetDeviceInfo_testGetBuildInfo);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clGetPlatformInfo_StubWithCallback(clGetPlatformInfo_version_1_2);
    clGetProgramBuildInfo_StubWithCallback(clGetProgramBuildInfo_testGetBuildInfo);
    clGetProgramBuildInfo_StubWithCallback(clGetProgramBuildInfo_testGetBuildInfo);

    cl::Program prog(make_program(0));
    cl::Device dev(fakeDevice);
    
    cl_int err;
    std::string log = prog.getBuildInfo<CL_PROGRAM_BUILD_LOG>(dev, &err);

    prog() = NULL;
    dev() = NULL;
}

/**
* Stub implementation of clGetCommandQueueInfo that returns first one image then none
*/
static cl_int clGetSupportedImageFormats_testGetSupportedImageFormats(
    cl_context context,
    cl_mem_flags flags,
    cl_mem_object_type image_type,
    cl_uint num_entries,
    cl_image_format *image_formats,
    cl_uint *num_image_formats,
    int num_calls)
{        
    // Catch failure case that causes error in bugzilla 13355:
    // returns CL_INVALID_VALUE if flags or image_type are not valid, 
    // or if num_entries is 0 and image_formats is not NULL.
    if (num_entries == 0 && image_formats != NULL) {
        return CL_INVALID_VALUE;
    }
    if (num_entries == 0)  {
        // If num_entries was 0 this is the query for number
        if (num_image_formats) {
            if (num_calls == 0) {
                *num_image_formats = 1;
            }
            else {
                *num_image_formats = 0;
            }
        }
    }
    else {
        // Should return something
        TEST_ASSERT_NOT_NULL(image_formats);
        
        // For first call we should return one format here
        if (num_calls == 1) {
            TEST_ASSERT_EQUAL(num_entries, 1);
            image_formats[0] = cl::ImageFormat(CL_RGB, CL_FLOAT);
        }
    }

    return CL_SUCCESS;
}

void testGetSupportedImageFormats()
{
    cl_context ctx_cl = make_context(0);

    clGetSupportedImageFormats_StubWithCallback(clGetSupportedImageFormats_testGetSupportedImageFormats);
    clGetSupportedImageFormats_StubWithCallback(clGetSupportedImageFormats_testGetSupportedImageFormats);
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);

    cl::Context ctx(ctx_cl);
    std::vector<cl::ImageFormat> formats;
    cl_int ret = CL_SUCCESS;

    ret = ctx.getSupportedImageFormats(
        CL_MEM_READ_WRITE,
        CL_MEM_OBJECT_IMAGE2D,
        &formats);
    TEST_ASSERT_EQUAL(ret, CL_SUCCESS);
    TEST_ASSERT_EQUAL(formats.size(), 1);
    ret = ctx.getSupportedImageFormats(
        CL_MEM_READ_WRITE,
        CL_MEM_OBJECT_IMAGE2D,
        &formats);
    TEST_ASSERT_EQUAL(formats.size(), 0);
    TEST_ASSERT_EQUAL(ret, CL_SUCCESS);
}

void testCreateSubDevice()
{
    // TODO

}

void testGetContextInfoDevices()
{
    // TODO
}

} // extern "C"
