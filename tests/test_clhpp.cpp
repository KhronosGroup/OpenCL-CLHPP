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

static inline cl_command_queue make_command_queue(int index)
{
    return (cl_command_queue) (size_t) (0xc0c0c0c0 + index);
}

/* Pools of pre-allocated wrapped objects for tests. There is no device pool,
 * because there is no way to know whether the test wants the device to be
 * reference countable or not.
 */
static const int POOL_MAX = 5;
static cl::Platform platformPool[POOL_MAX];
static cl::Context contextPool[POOL_MAX];
static cl::CommandQueue commandQueuePool[POOL_MAX];

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
    }
}

/****************************************************************************
 * Tests for cl::Context
 ****************************************************************************/

void testCopyContextNonNull()
{
    clReleaseContext_ExpectAndReturn(make_context(0), CL_SUCCESS);
    clRetainContext_ExpectAndReturn(make_context(1), CL_SUCCESS);

    contextPool[0] = contextPool[1];
}

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

/****************************************************************************
 * Tests for cl::CommandQueue
 ****************************************************************************/

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

} // extern "C"
