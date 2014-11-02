/*******************************************************************************
 * Copyright (c) 2008-2014 The Khronos Group Inc.
 *
 * Permission is hereby granted, free of charge, to any person obtaining a
 * copy of this software and/or associated documentation files (the
 * "Materials"), to deal in the Materials without restriction, including
 * without limitation the rights to use, copy, modify, merge, publish,
 * distribute, sublicense, and/or sell copies of the Materials, and to
 * permit persons to whom the Materials are furnished to do so, subject to
 * the following conditions:
 *
 * The above copyright notice and this permission notice shall be included
 * in all copies or substantial portions of the Materials.
 *
 * THE MATERIALS ARE PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
 * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
 * IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY
 * CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE
 * MATERIALS OR THE USE OR OTHER DEALINGS IN THE MATERIALS.
 ******************************************************************************/

/*! \file
 *
 *   \brief C++ bindings for OpenCL 1.0 (rev 48), OpenCL 1.1 (rev 33), 
 *       OpenCL 1.2 (rev 15) and OpenCL 2.0 (rev 23)
 *   \author Lee Howes and Bruce Merry
 *   
 *   Derived from the OpenCL 1.x C++ bindings written by 
 *   Benedict R. Gaster, Laurent Morichetti and Lee Howes
 *   With additions and fixes from:
 *       Brian Cole, March 3rd 2010 and April 2012
 *       Matt Gruenke, April 2012.
 *       Bruce Merry, February 2013.
 *       Tom Deakin and Simon McIntosh-Smith, July 2013
 *   
 *   \version 2.0.1
 *   \date August 2014
 *
 *   Optional extension support
 *
 *         cl
 *         cl_ext_device_fission
 *         #define CL_HPP_USE_CL_DEVICE_FISSION
 */

/*! \mainpage
 * \section intro Introduction
 * For many large applications C++ is the language of choice and so it seems
 * reasonable to define C++ bindings for OpenCL.
 *
 *
 * The interface is contained with a single C++ header file \em cl2.hpp and all
 * definitions are contained within the namespace \em cl. There is no additional
 * requirement to include \em cl.h and to use either the C++ or original C
 * bindings; it is enough to simply include \em cl.hpp.
 *
 * The bindings themselves are lightweight and correspond closely to the
 * underlying C API. Using the C++ bindings introduces no additional execution
 * overhead.
 *
 * For detail documentation on the bindings see:
 *
 * The OpenCL C++ Wrapper API 1.2 (revision 09)
 *  http://www.khronos.org/registry/cl/specs/opencl-cplusplus-1.2.pdf
 *
 * \section compatibility Compatibility
 * TODO: Describe support for OpenCL 1.1 and 1.2 platforms.
 *
 * \section parameterization Parameters
 * This header may be parameterized by a set of preprocessor macros.
 * CL_HPP_NO_STD_STRING
 * CL_HPP_NO_STD_VECTOR
 * CL_HPP_ENABLE_DEVICE_FISSION - Enables device fission for OpenCL 1.2 platforms
 * CL_HPP_ENABLE_EXCEPTIONS
 *
 * \section example Example
 *
 * The following example shows a general use case for the C++
 * bindings, including support for the optional exception feature and
 * also the supplied vector and string classes, see following sections for
 * decriptions of these features.
 * 
 * \code
 *  TODO: REDO EXAMPLE
 * 
 * \endcode
 *
 */
#ifndef CL_HPP_
#define CL_HPP_

/* Handle deprecated preprocessor definitions. In each case, we only check for
 * the old name if the new name is not defined, so that user code can define
 * both and hence work with either version of the bindings.
 */
#if !defined(CL_HPP_USE_DX_INTEROP) && defined(USE_DX_INTEROP)
# warning "USE_DX_INTEROP is deprecated. Define CL_HPP_USE_DX_INTEROP instead"
# define CL_HPP_USE_DX_INTEROP
#endif
#if !defined(CL_HPP_USE_CL_DEVICE_FISSION) && defined(USE_CL_DEVICE_FISSION)
# warning "USE_CL_DEVICE_FISSION is deprecated. Define CL_HPP_USE_CL_DEVICE_FISSION instead"
# define CL_HPP_USE_CL_DEVICE_FISSION
#endif
#if !defined(CL_HPP_ENABLE_EXCEPTIONS) && defined(__CL_ENABLE_EXCEPTIONS)
# warning "__CL_ENABLE_EXCEPTIONS is deprecated. Define CL_HPP_ENABLE_EXCEPTIONS instead"
# define CL_HPP_ENABLE_EXCEPTIONS
#endif
#if !defined(CL_HPP_NO_STD_VECTOR) && defined(__NO_STD_VECTOR)
# warning "__NO_STD_VECTOR is deprecated. Define CL_HPP_NO_STD_VECTOR instead"
# define CL_HPP_NO_STD_VECTOR
#endif
#if !defined(CL_HPP_NO_STD_STRING) && defined(__NO_STD_STRING)
# warning "__NO_STD_STRING is deprecated. Define CL_HPP_NO_STD_STRING instead"
# define CL_HPP_NO_STD_STRING
#endif
#if defined(VECTOR_CLASS)
# warning "VECTOR_CLASS is deprecated. Alias cl::vector_class instead"
#endif
#if defined(STRING_CLASS)
# warning "STRING_CLASS is deprecated. Alias cl::string_class instead."
#endif
#if !defined(CL_HPP_USER_OVERRIDE_ERROR_STRINGS) && defined(__CL_USER_OVERRIDE_ERROR_STRINGS)
# warning "__CL_USER_OVERRIDE_ERROR_STRINGS is deprecated. Define CL_HPP_USER_OVERRIDE_ERROR_STRINGS instead"
# define CL_HPP_USER_OVERRIDE_ERROR_STRINGS
#endif

/* Warn about features that are no longer supported
 */
#if defined(__USE_DEV_VECTOR)
# warning "__USE_DEV_VECTOR is no longer supported. Expect compilation errors"
#endif
#if defined(__USE_DEV_STRING)
# warning "__USE_DEV_STRING is no longer supported. Expect compilation errors"
#endif

/* Detect which version to target */
#if !defined(CL_HPP_TARGET_OPENCL_VERSION)
# warning "CL_HPP_TARGET_OPENCL_VERSION is not defined. It will default to 200 (OpenCL 2.0)"
# define CL_HPP_TARGET_OPENCL_VERSION 200
#endif
#if CL_HPP_TARGET_OPENCL_VERSION != 100 && CL_HPP_TARGET_OPENCL_VERSION != 110 && CL_HPP_TARGET_OPENCL_VERSION != 120 && CL_HPP_TARGET_OPENCL_VERSION != 200
# warning "CL_HPP_TARGET_OPENCL_VERSION is not a valid value (100, 110, 120 or 200). It will be set to 200"
# undef CL_HPP_TARGET_OPENCL_VERSION
# define CL_HPP_TARGET_OPENCL_VERSION 200
#endif

#if !defined(CL_HPP_MINIMUM_OPENCL_VERSION)
# define CL_HPP_MINIMUM_OPENCL_VERSION 100
#endif
#if CL_HPP_MINIMUM_OPENCL_VERSION != 100 && CL_HPP_MINIMUM_OPENCL_VERSION != 110 && CL_HPP_MINIMUM_OPENCL_VERSION != 120 && CL_HPP_MINIMUM_OPENCL_VERSION != 200
# warning "CL_HPP_MINIMUM_OPENCL_VERSION is not a valid value (100, 110, 120 or 200). It will be set to 100"
# undef CL_HPP_MINIMUM_OPENCL_VERSION
# define CL_HPP_MINIMUM_OPENCL_VERSION 100
#endif
#if CL_HPP_MINIMUM_OPENCL_VERSION > CL_HPP_TARGET_OPENCL_VERSION
# error "CL_HPP_MINIMUM_OPENCL_VERSION must not be greater than CL_HPP_TARGET_OPENCL_VERSION"
#endif

#if CL_HPP_MINIMUM_OPENCL_VERSION <= 100 && !defined(CL_USE_DEPRECATED_OPENCL_1_0_APIS)
# define CL_USE_DEPRECATED_OPENCL_1_0_APIS
#endif
#if CL_HPP_MINIMUM_OPENCL_VERSION <= 110 && !defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)
# define CL_USE_DEPRECATED_OPENCL_1_1_APIS
#endif
#if CL_HPP_MINIMUM_OPENCL_VERSION <= 120 && !defined(CL_USE_DEPRECATED_OPENCL_1_2_APIS)
# define CL_USE_DEPRECATED_OPENCL_1_2_APIS
#endif
#if CL_HPP_MINIMUM_OPENCL_VERSION <= 200 && !defined(CL_USE_DEPRECATED_OPENCL_2_0_APIS)
# define CL_USE_DEPRECATED_OPENCL_2_0_APIS
#endif

#ifdef _WIN32

#include <malloc.h>

#if defined(CL_HPP_USE_DX_INTEROP)
#include <CL/cl_d3d10.h>
#include <CL/cl_dx9_media_sharing.h>
#endif
#endif // _WIN32

#if defined(_MSC_VER)
#include <intrin.h>
#endif // _MSC_VER 
 
 // Check for a valid C++ version

// Need to do both tests here because for some reason __cplusplus is not 
// updated in visual studio
#if (!defined(_MSC_VER) && __cplusplus < 201103L) || (defined(_MSC_VER) && _MSC_VER < 1700)
#error Visual studio 2013 or another C++11-supporting compiler required
#endif

// 
#if defined(CL_HPP_USE_CL_DEVICE_FISSION)
#include <CL/cl_ext.h>
#endif

#if defined(__APPLE__) || defined(__MACOSX)
#include <OpenCL/opencl.h>
#else
#include <CL/opencl.h>
#endif // !__APPLE__

#if (__cplusplus >= 201103L)
#define CL_HPP_NOEXCEPT_ noexcept
#else
#define CL_HPP_NOEXCEPT_
#endif

#if defined(_MSC_VER)
# define CL_HPP_DEFINE_STATIC_MEMBER_ __declspec(selectany)
#else
# define CL_HPP_DEFINE_STATIC_MEMBER_ __attribute__((weak))
#endif // !_MSC_VER

// Define deprecated prefixes and suffixes to ensure compilation
// in case they are not pre-defined
#if !defined(CL_EXT_PREFIX__VERSION_1_1_DEPRECATED)
#define CL_EXT_PREFIX__VERSION_1_1_DEPRECATED  
#endif // #if !defined(CL_EXT_PREFIX__VERSION_1_1_DEPRECATED)
#if !defined(CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED)
#define CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
#endif // #if !defined(CL_EXT_PREFIX__VERSION_1_1_DEPRECATED)

#if !defined(CL_CALLBACK)
#define CL_CALLBACK
#endif //CL_CALLBACK

#include <utility>
#include <limits>
#include <iterator>
#include <mutex>
#include <cstring>

#if defined(CL_HPP_ENABLE_EXCEPTIONS)
#include <exception>
#endif // #if defined(CL_HPP_ENABLE_EXCEPTIONS)

#if !defined(CL_HPP_NO_STD_VECTOR)
#include <vector>
template < class T, class Alloc = std::allocator<T> >
using vector_class = std::vector<T, Alloc>;
#endif

#if !defined(CL_HPP_NO_STD_STRING)
#include <string>
using string_class = std::string;
#endif 


#if defined(__ANDROID__) || defined(linux) || defined(__APPLE__) || defined(__MACOSX)
#include <alloca.h>
#endif // linux

/*! \namespace cl
 *
 * \brief The OpenCL C++ bindings are defined within this namespace.
 *
 */
namespace cl {

class Memory;

#define CL_HPP_INIT_CL_EXT_FCN_PTR_(name) \
    if(!pfn_##name) { \
        pfn_##name = (PFN_##name) \
            clGetExtensionFunctionAddress(#name); \
        if(!pfn_##name) { \
        } \
    }

#define CL_HPP_INIT_CL_EXT_FCN_PTR_PLATFORM_(platform, name) \
    if(!pfn_##name) { \
        pfn_##name = (PFN_##name) \
            clGetExtensionFunctionAddressForPlatform(platform, #name); \
        if(!pfn_##name) { \
        } \
    }

class Program;
class Device;
class Context;
class CommandQueue;
class Memory;
class Buffer;

#if defined(CL_HPP_ENABLE_EXCEPTIONS)
/*! \brief Exception class 
 * 
 *  This may be thrown by API functions when CL_HPP_ENABLE_EXCEPTIONS is defined.
 */
class Error : public std::exception
{
private:
    cl_int err_;
    const char * errStr_;
public:
    /*! \brief Create a new CL error exception for a given error code
     *  and corresponding message.
     * 
     *  \param err error code value.
     *
     *  \param errStr a descriptive string that must remain in scope until
     *                handling of the exception has concluded.  If set, it
     *                will be returned by what().
     */
    Error(cl_int err, const char * errStr = NULL) : err_(err), errStr_(errStr)
    {}

    ~Error() throw() {}

    /*! \brief Get error string associated with exception
     *
     * \return A memory pointer to the error message string.
     */
    virtual const char * what() const throw ()
    {
        if (errStr_ == NULL) {
            return "empty";
        }
        else {
            return errStr_;
        }
    }

    /*! \brief Get error code associated with exception
     *
     *  \return The error code.
     */
    cl_int err(void) const { return err_; }
};

#define CL_HPP_ERR_STR_(x) #x
#else
#define CL_HPP_ERR_STR_(x) NULL
#endif // CL_HPP_ENABLE_EXCEPTIONS


namespace detail
{
#if defined(CL_HPP_ENABLE_EXCEPTIONS)
static inline cl_int errHandler (
    cl_int err,
    const char * errStr = NULL)
{
    if (err != CL_SUCCESS) {
        throw Error(err, errStr);
    }
    return err;
}
#else
static inline cl_int errHandler (cl_int err, const char * errStr = NULL)
{
    (void) errStr; // suppress unused variable warning
    return err;
}
#endif // CL_HPP_ENABLE_EXCEPTIONS
}



//! \cond DOXYGEN_DETAIL
#if !defined(CL_HPP_USER_OVERRIDE_ERROR_STRINGS)
#define __GET_DEVICE_INFO_ERR               CL_HPP_ERR_STR_(clGetDeviceInfo)
#define __GET_PLATFORM_INFO_ERR             CL_HPP_ERR_STR_(clGetPlatformInfo)
#define __GET_DEVICE_IDS_ERR                CL_HPP_ERR_STR_(clGetDeviceIDs)
#define __GET_PLATFORM_IDS_ERR              CL_HPP_ERR_STR_(clGetPlatformIDs)
#define __GET_CONTEXT_INFO_ERR              CL_HPP_ERR_STR_(clGetContextInfo)
#define __GET_EVENT_INFO_ERR                CL_HPP_ERR_STR_(clGetEventInfo)
#define __GET_EVENT_PROFILE_INFO_ERR        CL_HPP_ERR_STR_(clGetEventProfileInfo)
#define __GET_MEM_OBJECT_INFO_ERR           CL_HPP_ERR_STR_(clGetMemObjectInfo)
#define __GET_IMAGE_INFO_ERR                CL_HPP_ERR_STR_(clGetImageInfo)
#define __GET_SAMPLER_INFO_ERR              CL_HPP_ERR_STR_(clGetSamplerInfo)
#define __GET_KERNEL_INFO_ERR               CL_HPP_ERR_STR_(clGetKernelInfo)
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __GET_KERNEL_ARG_INFO_ERR           CL_HPP_ERR_STR_(clGetKernelArgInfo)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __GET_KERNEL_WORK_GROUP_INFO_ERR    CL_HPP_ERR_STR_(clGetKernelWorkGroupInfo)
#define __GET_PROGRAM_INFO_ERR              CL_HPP_ERR_STR_(clGetProgramInfo)
#define __GET_PROGRAM_BUILD_INFO_ERR        CL_HPP_ERR_STR_(clGetProgramBuildInfo)
#define __GET_COMMAND_QUEUE_INFO_ERR        CL_HPP_ERR_STR_(clGetCommandQueueInfo)

#define __CREATE_CONTEXT_ERR                CL_HPP_ERR_STR_(clCreateContext)
#define __CREATE_CONTEXT_FROM_TYPE_ERR      CL_HPP_ERR_STR_(clCreateContextFromType)
#define __GET_SUPPORTED_IMAGE_FORMATS_ERR   CL_HPP_ERR_STR_(clGetSupportedImageFormats)

#define __CREATE_BUFFER_ERR                 CL_HPP_ERR_STR_(clCreateBuffer)
#define __COPY_ERR                          CL_HPP_ERR_STR_(cl::copy)
#define __CREATE_SUBBUFFER_ERR              CL_HPP_ERR_STR_(clCreateSubBuffer)
#define __CREATE_GL_BUFFER_ERR              CL_HPP_ERR_STR_(clCreateFromGLBuffer)
#define __CREATE_GL_RENDER_BUFFER_ERR       CL_HPP_ERR_STR_(clCreateFromGLBuffer)
#define __GET_GL_OBJECT_INFO_ERR            CL_HPP_ERR_STR_(clGetGLObjectInfo)
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __CREATE_IMAGE_ERR                  CL_HPP_ERR_STR_(clCreateImage)
#define __CREATE_GL_TEXTURE_ERR             CL_HPP_ERR_STR_(clCreateFromGLTexture)
#define __IMAGE_DIMENSION_ERR               CL_HPP_ERR_STR_(Incorrect image dimensions)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __CREATE_SAMPLER_ERR                CL_HPP_ERR_STR_(clCreateSampler)
#define __SET_MEM_OBJECT_DESTRUCTOR_CALLBACK_ERR CL_HPP_ERR_STR_(clSetMemObjectDestructorCallback)

#define __CREATE_USER_EVENT_ERR             CL_HPP_ERR_STR_(clCreateUserEvent)
#define __SET_USER_EVENT_STATUS_ERR         CL_HPP_ERR_STR_(clSetUserEventStatus)
#define __SET_EVENT_CALLBACK_ERR            CL_HPP_ERR_STR_(clSetEventCallback)
#define __WAIT_FOR_EVENTS_ERR               CL_HPP_ERR_STR_(clWaitForEvents)

#define __CREATE_KERNEL_ERR                 CL_HPP_ERR_STR_(clCreateKernel)
#define __SET_KERNEL_ARGS_ERR               CL_HPP_ERR_STR_(clSetKernelArg)
#define __CREATE_PROGRAM_WITH_SOURCE_ERR    CL_HPP_ERR_STR_(clCreateProgramWithSource)
#define __CREATE_PROGRAM_WITH_BINARY_ERR    CL_HPP_ERR_STR_(clCreateProgramWithBinary)
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __CREATE_PROGRAM_WITH_BUILT_IN_KERNELS_ERR    CL_HPP_ERR_STR_(clCreateProgramWithBuiltInKernels)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __BUILD_PROGRAM_ERR                 CL_HPP_ERR_STR_(clBuildProgram)
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __COMPILE_PROGRAM_ERR               CL_HPP_ERR_STR_(clCompileProgram)
#define __LINK_PROGRAM_ERR                  CL_HPP_ERR_STR_(clLinkProgram)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __CREATE_KERNELS_IN_PROGRAM_ERR     CL_HPP_ERR_STR_(clCreateKernelsInProgram)

#define __CREATE_COMMAND_QUEUE_ERR          CL_HPP_ERR_STR_(clCreateCommandQueue)
#define __SET_COMMAND_QUEUE_PROPERTY_ERR    CL_HPP_ERR_STR_(clSetCommandQueueProperty)
#define __ENQUEUE_READ_BUFFER_ERR           CL_HPP_ERR_STR_(clEnqueueReadBuffer)
#define __ENQUEUE_READ_BUFFER_RECT_ERR      CL_HPP_ERR_STR_(clEnqueueReadBufferRect)
#define __ENQUEUE_WRITE_BUFFER_ERR          CL_HPP_ERR_STR_(clEnqueueWriteBuffer)
#define __ENQUEUE_WRITE_BUFFER_RECT_ERR     CL_HPP_ERR_STR_(clEnqueueWriteBufferRect)
#define __ENQEUE_COPY_BUFFER_ERR            CL_HPP_ERR_STR_(clEnqueueCopyBuffer)
#define __ENQEUE_COPY_BUFFER_RECT_ERR       CL_HPP_ERR_STR_(clEnqueueCopyBufferRect)
#define __ENQUEUE_FILL_BUFFER_ERR           CL_HPP_ERR_STR_(clEnqueueFillBuffer)
#define __ENQUEUE_READ_IMAGE_ERR            CL_HPP_ERR_STR_(clEnqueueReadImage)
#define __ENQUEUE_WRITE_IMAGE_ERR           CL_HPP_ERR_STR_(clEnqueueWriteImage)
#define __ENQUEUE_COPY_IMAGE_ERR            CL_HPP_ERR_STR_(clEnqueueCopyImage)
#define __ENQUEUE_FILL_IMAGE_ERR            CL_HPP_ERR_STR_(clEnqueueFillImage)
#define __ENQUEUE_COPY_IMAGE_TO_BUFFER_ERR  CL_HPP_ERR_STR_(clEnqueueCopyImageToBuffer)
#define __ENQUEUE_COPY_BUFFER_TO_IMAGE_ERR  CL_HPP_ERR_STR_(clEnqueueCopyBufferToImage)
#define __ENQUEUE_MAP_BUFFER_ERR            CL_HPP_ERR_STR_(clEnqueueMapBuffer)
#define __ENQUEUE_MAP_IMAGE_ERR             CL_HPP_ERR_STR_(clEnqueueMapImage)
#define __ENQUEUE_UNMAP_MEM_OBJECT_ERR      CL_HPP_ERR_STR_(clEnqueueUnMapMemObject)
#define __ENQUEUE_NDRANGE_KERNEL_ERR        CL_HPP_ERR_STR_(clEnqueueNDRangeKernel)
#define __ENQUEUE_TASK_ERR                  CL_HPP_ERR_STR_(clEnqueueTask)
#define __ENQUEUE_NATIVE_KERNEL             CL_HPP_ERR_STR_(clEnqueueNativeKernel)
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __ENQUEUE_MIGRATE_MEM_OBJECTS_ERR   CL_HPP_ERR_STR_(clEnqueueMigrateMemObjects)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

#define __ENQUEUE_ACQUIRE_GL_ERR            CL_HPP_ERR_STR_(clEnqueueAcquireGLObjects)
#define __ENQUEUE_RELEASE_GL_ERR            CL_HPP_ERR_STR_(clEnqueueReleaseGLObjects)


#define __RETAIN_ERR                        CL_HPP_ERR_STR_(Retain Object)
#define __RELEASE_ERR                       CL_HPP_ERR_STR_(Release Object)
#define __FLUSH_ERR                         CL_HPP_ERR_STR_(clFlush)
#define __FINISH_ERR                        CL_HPP_ERR_STR_(clFinish)
#define __VECTOR_CAPACITY_ERR               CL_HPP_ERR_STR_(Vector capacity error)

/**
 * CL 1.2 version that uses device fission.
 */
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __CREATE_SUB_DEVICES_ERR            CL_HPP_ERR_STR_(clCreateSubDevices)
#else
#define __CREATE_SUB_DEVICES_ERR            CL_HPP_ERR_STR_(clCreateSubDevicesEXT)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

/**
 * Deprecated APIs for 1.2
 */
#if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)
#define __ENQUEUE_MARKER_ERR                CL_HPP_ERR_STR_(clEnqueueMarker)
#define __ENQUEUE_WAIT_FOR_EVENTS_ERR       CL_HPP_ERR_STR_(clEnqueueWaitForEvents)
#define __ENQUEUE_BARRIER_ERR               CL_HPP_ERR_STR_(clEnqueueBarrier)
#define __UNLOAD_COMPILER_ERR               CL_HPP_ERR_STR_(clUnloadCompiler)
#define __CREATE_GL_TEXTURE_2D_ERR          CL_HPP_ERR_STR_(clCreateFromGLTexture2D)
#define __CREATE_GL_TEXTURE_3D_ERR          CL_HPP_ERR_STR_(clCreateFromGLTexture3D)
#define __CREATE_IMAGE2D_ERR                CL_HPP_ERR_STR_(clCreateImage2D)
#define __CREATE_IMAGE3D_ERR                CL_HPP_ERR_STR_(clCreateImage3D)
#endif // #if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)

#endif // CL_HPP_USER_OVERRIDE_ERROR_STRINGS
//! \endcond

/**
 * CL 1.2 marker and barrier commands
 */
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#define __ENQUEUE_MARKER_WAIT_LIST_ERR                CL_HPP_ERR_STR_(clEnqueueMarkerWithWaitList)
#define __ENQUEUE_BARRIER_WAIT_LIST_ERR               CL_HPP_ERR_STR_(clEnqueueBarrierWithWaitList)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

    
/*! \brief class used to interface between C++ and
 *  OpenCL C calls that require arrays of size_t values, whose
 *  size is known statically.
 */
template <int N>
class size_t
{ 
private:
    ::size_t data_[N];

public:
    //! \brief Initialize size_t to all 0s
    size_t()
    {
        for( int i = 0; i < N; ++i ) {
            data_[i] = 0;
        }
    }

    ::size_t& operator[](int index)
    {
        return data_[index];
    }

    const ::size_t& operator[](int index) const
    {
        return data_[index];
    }

    //! \brief Conversion operator to T*.
    operator ::size_t* ()             { return data_; }

    //! \brief Conversion operator to const T*.
    operator const ::size_t* () const { return data_; }
};

namespace detail {

// Generic getInfoHelper. The final parameter is used to guide overload
// resolution: the actual parameter passed is an int, which makes this
// a worse conversion sequence than a specialization that declares the
// parameter as an int.
template<typename Functor, typename T>
inline cl_int getInfoHelper(Functor f, cl_uint name, T* param, long)
{
    return f(name, sizeof(T), param, NULL);
}

// Specialized getInfoHelper for vector_class params
template <typename Func, typename T>
inline cl_int getInfoHelper(Func f, cl_uint name, vector_class<T>* param, long)
{
    ::size_t required;
    cl_int err = f(name, 0, NULL, &required);
    if (err != CL_SUCCESS) {
        return err;
    }

    T* value = (T*) alloca(required);
    err = f(name, required, value, NULL);
    if (err != CL_SUCCESS) {
        return err;
    }

    param->assign(&value[0], &value[required/sizeof(T)]);
    return CL_SUCCESS;
}

/* Specialization for reference-counted types. This depends on the
 * existence of Wrapper<T>::cl_type, and none of the other types having the
 * cl_type member. Note that simplify specifying the parameter as Wrapper<T>
 * does not work, because when using a derived type (e.g. Context) the generic
 * template will provide a better match.
 */
template <typename Func, typename T>
inline cl_int getInfoHelper(Func f, cl_uint name, vector_class<T>* param, int, typename T::cl_type = 0)
{
    ::size_t required;
    cl_int err = f(name, 0, NULL, &required);
    if (err != CL_SUCCESS) {
        return err;
    }

    typename T::cl_type * value = (typename T::cl_type *) alloca(required);
    err = f(name, required, value, NULL);
    if (err != CL_SUCCESS) {
        return err;
    }

    ::size_t elements = required / sizeof(typename T::cl_type);
    param->assign(&value[0], &value[elements]);
    for (::size_t i = 0; i < elements; i++)
    {
        if (value[i] != NULL)
        {
            err = (*param)[i].retain();
            if (err != CL_SUCCESS) {
                return err;
            }
        }
    }
    return CL_SUCCESS;
}

// Specialized for getInfo<CL_PROGRAM_BINARIES>
template <typename Func>
inline cl_int getInfoHelper(Func f, cl_uint name, vector_class<char *>* param, int)
{
    cl_int err = f(name, param->size() * sizeof(char *), &(*param)[0], NULL);

    if (err != CL_SUCCESS) {
        return err;
    }

    return CL_SUCCESS;
}

// Specialized GetInfoHelper for string_class params
template <typename Func>
inline cl_int getInfoHelper(Func f, cl_uint name, string_class* param, long)
{
    ::size_t required;
    cl_int err = f(name, 0, NULL, &required);
    if (err != CL_SUCCESS) {
        return err;
    }

    char* value = (char*) alloca(required);
    err = f(name, required, value, NULL);
    if (err != CL_SUCCESS) {
        return err;
    }

    *param = value;
    return CL_SUCCESS;
}

// Specialized GetInfoHelper for cl::size_t params
template <typename Func, ::size_t N>
inline cl_int getInfoHelper(Func f, cl_uint name, size_t<N>* param, long)
{
    ::size_t required;
    cl_int err = f(name, 0, NULL, &required);
    if (err != CL_SUCCESS) {
        return err;
    }

    ::size_t* value = (::size_t*) alloca(required);
    err = f(name, required, value, NULL);
    if (err != CL_SUCCESS) {
        return err;
    }

    for(int i = 0; i < N; ++i) {
        (*param)[i] = value[i];
    }

    return CL_SUCCESS;
}

template<typename T> struct ReferenceHandler;

/* Specialization for reference-counted types. This depends on the
 * existence of Wrapper<T>::cl_type, and none of the other types having the
 * cl_type member. Note that simplify specifying the parameter as Wrapper<T>
 * does not work, because when using a derived type (e.g. Context) the generic
 * template will provide a better match.
 */
template<typename Func, typename T>
inline cl_int getInfoHelper(Func f, cl_uint name, T* param, int, typename T::cl_type = 0)
{
    typename T::cl_type value;
    cl_int err = f(name, sizeof(value), &value, NULL);
    if (err != CL_SUCCESS) {
        return err;
    }
    *param = value;
    if (value != NULL)
    {
        err = param->retain();
        if (err != CL_SUCCESS) {
            return err;
        }
    }
    return CL_SUCCESS;
}

#define CL_HPP_PARAM_NAME_INFO_1_0_(F) \
    F(cl_platform_info, CL_PLATFORM_PROFILE, string_class) \
    F(cl_platform_info, CL_PLATFORM_VERSION, string_class) \
    F(cl_platform_info, CL_PLATFORM_NAME, string_class) \
    F(cl_platform_info, CL_PLATFORM_VENDOR, string_class) \
    F(cl_platform_info, CL_PLATFORM_EXTENSIONS, string_class) \
    \
    F(cl_device_info, CL_DEVICE_TYPE, cl_device_type) \
    F(cl_device_info, CL_DEVICE_VENDOR_ID, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_COMPUTE_UNITS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_WORK_GROUP_SIZE, ::size_t) \
    F(cl_device_info, CL_DEVICE_MAX_WORK_ITEM_SIZES, vector_class< ::size_t>) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_CLOCK_FREQUENCY, cl_uint) \
    F(cl_device_info, CL_DEVICE_ADDRESS_BITS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_READ_IMAGE_ARGS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_WRITE_IMAGE_ARGS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MAX_MEM_ALLOC_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_IMAGE2D_MAX_WIDTH, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE2D_MAX_HEIGHT, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE3D_MAX_WIDTH, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE3D_MAX_HEIGHT, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE3D_MAX_DEPTH, ::size_t) \
    F(cl_device_info, CL_DEVICE_IMAGE_SUPPORT, cl_bool) \
    F(cl_device_info, CL_DEVICE_MAX_PARAMETER_SIZE, ::size_t) \
    F(cl_device_info, CL_DEVICE_MAX_SAMPLERS, cl_uint) \
    F(cl_device_info, CL_DEVICE_MEM_BASE_ADDR_ALIGN, cl_uint) \
    F(cl_device_info, CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE, cl_uint) \
    F(cl_device_info, CL_DEVICE_SINGLE_FP_CONFIG, cl_device_fp_config) \
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_CACHE_TYPE, cl_device_mem_cache_type) \
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE, cl_uint)\
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_CACHE_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_GLOBAL_MEM_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_MAX_CONSTANT_ARGS, cl_uint) \
    F(cl_device_info, CL_DEVICE_LOCAL_MEM_TYPE, cl_device_local_mem_type) \
    F(cl_device_info, CL_DEVICE_LOCAL_MEM_SIZE, cl_ulong) \
    F(cl_device_info, CL_DEVICE_ERROR_CORRECTION_SUPPORT, cl_bool) \
    F(cl_device_info, CL_DEVICE_PROFILING_TIMER_RESOLUTION, ::size_t) \
    F(cl_device_info, CL_DEVICE_ENDIAN_LITTLE, cl_bool) \
    F(cl_device_info, CL_DEVICE_AVAILABLE, cl_bool) \
    F(cl_device_info, CL_DEVICE_COMPILER_AVAILABLE, cl_bool) \
    F(cl_device_info, CL_DEVICE_EXECUTION_CAPABILITIES, cl_device_exec_capabilities) \
    F(cl_device_info, CL_DEVICE_QUEUE_PROPERTIES, cl_command_queue_properties) \
    F(cl_device_info, CL_DEVICE_PLATFORM, cl_platform_id) \
    F(cl_device_info, CL_DEVICE_NAME, string_class) \
    F(cl_device_info, CL_DEVICE_VENDOR, string_class) \
    F(cl_device_info, CL_DRIVER_VERSION, string_class) \
    F(cl_device_info, CL_DEVICE_PROFILE, string_class) \
    F(cl_device_info, CL_DEVICE_VERSION, string_class) \
    F(cl_device_info, CL_DEVICE_EXTENSIONS, string_class) \
    \
    F(cl_context_info, CL_CONTEXT_REFERENCE_COUNT, cl_uint) \
    F(cl_context_info, CL_CONTEXT_DEVICES, vector_class<Device>) \
    F(cl_context_info, CL_CONTEXT_PROPERTIES, vector_class<cl_context_properties>) \
    \
    F(cl_event_info, CL_EVENT_COMMAND_QUEUE, cl::CommandQueue) \
    F(cl_event_info, CL_EVENT_COMMAND_TYPE, cl_command_type) \
    F(cl_event_info, CL_EVENT_REFERENCE_COUNT, cl_uint) \
    F(cl_event_info, CL_EVENT_COMMAND_EXECUTION_STATUS, cl_int) \
    \
    F(cl_profiling_info, CL_PROFILING_COMMAND_QUEUED, cl_ulong) \
    F(cl_profiling_info, CL_PROFILING_COMMAND_SUBMIT, cl_ulong) \
    F(cl_profiling_info, CL_PROFILING_COMMAND_START, cl_ulong) \
    F(cl_profiling_info, CL_PROFILING_COMMAND_END, cl_ulong) \
    \
    F(cl_mem_info, CL_MEM_TYPE, cl_mem_object_type) \
    F(cl_mem_info, CL_MEM_FLAGS, cl_mem_flags) \
    F(cl_mem_info, CL_MEM_SIZE, ::size_t) \
    F(cl_mem_info, CL_MEM_HOST_PTR, void*) \
    F(cl_mem_info, CL_MEM_MAP_COUNT, cl_uint) \
    F(cl_mem_info, CL_MEM_REFERENCE_COUNT, cl_uint) \
    F(cl_mem_info, CL_MEM_CONTEXT, cl::Context) \
    \
    F(cl_image_info, CL_IMAGE_FORMAT, cl_image_format) \
    F(cl_image_info, CL_IMAGE_ELEMENT_SIZE, ::size_t) \
    F(cl_image_info, CL_IMAGE_ROW_PITCH, ::size_t) \
    F(cl_image_info, CL_IMAGE_SLICE_PITCH, ::size_t) \
    F(cl_image_info, CL_IMAGE_WIDTH, ::size_t) \
    F(cl_image_info, CL_IMAGE_HEIGHT, ::size_t) \
    F(cl_image_info, CL_IMAGE_DEPTH, ::size_t) \
    \
    F(cl_sampler_info, CL_SAMPLER_REFERENCE_COUNT, cl_uint) \
    F(cl_sampler_info, CL_SAMPLER_CONTEXT, cl::Context) \
    F(cl_sampler_info, CL_SAMPLER_NORMALIZED_COORDS, cl_addressing_mode) \
    F(cl_sampler_info, CL_SAMPLER_ADDRESSING_MODE, cl_filter_mode) \
    F(cl_sampler_info, CL_SAMPLER_FILTER_MODE, cl_bool) \
    \
    F(cl_program_info, CL_PROGRAM_REFERENCE_COUNT, cl_uint) \
    F(cl_program_info, CL_PROGRAM_CONTEXT, cl::Context) \
    F(cl_program_info, CL_PROGRAM_NUM_DEVICES, cl_uint) \
    F(cl_program_info, CL_PROGRAM_DEVICES, vector_class<Device>) \
    F(cl_program_info, CL_PROGRAM_SOURCE, string_class) \
    F(cl_program_info, CL_PROGRAM_BINARY_SIZES, vector_class< ::size_t>) \
    F(cl_program_info, CL_PROGRAM_BINARIES, vector_class<char *>) \
    \
    F(cl_program_build_info, CL_PROGRAM_BUILD_STATUS, cl_build_status) \
    F(cl_program_build_info, CL_PROGRAM_BUILD_OPTIONS, string_class) \
    F(cl_program_build_info, CL_PROGRAM_BUILD_LOG, string_class) \
    \
    F(cl_kernel_info, CL_KERNEL_FUNCTION_NAME, string_class) \
    F(cl_kernel_info, CL_KERNEL_NUM_ARGS, cl_uint) \
    F(cl_kernel_info, CL_KERNEL_REFERENCE_COUNT, cl_uint) \
    F(cl_kernel_info, CL_KERNEL_CONTEXT, cl::Context) \
    F(cl_kernel_info, CL_KERNEL_PROGRAM, cl::Program) \
    \
    F(cl_kernel_work_group_info, CL_KERNEL_WORK_GROUP_SIZE, ::size_t) \
    F(cl_kernel_work_group_info, CL_KERNEL_COMPILE_WORK_GROUP_SIZE, cl::size_t<3>) \
    F(cl_kernel_work_group_info, CL_KERNEL_LOCAL_MEM_SIZE, cl_ulong) \
    \
    F(cl_command_queue_info, CL_QUEUE_CONTEXT, cl::Context) \
    F(cl_command_queue_info, CL_QUEUE_DEVICE, cl::Device) \
    F(cl_command_queue_info, CL_QUEUE_REFERENCE_COUNT, cl_uint) \
    F(cl_command_queue_info, CL_QUEUE_PROPERTIES, cl_command_queue_properties)

#define CL_HPP_PARAM_NAME_INFO_1_1_(F) \
    F(cl_context_info, CL_CONTEXT_NUM_DEVICES, cl_uint)\
    F(cl_device_info, CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF, cl_uint) \
    F(cl_device_info, CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR, cl_uint) \
    F(cl_device_info, CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT, cl_uint) \
    F(cl_device_info, CL_DEVICE_NATIVE_VECTOR_WIDTH_INT, cl_uint) \
    F(cl_device_info, CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG, cl_uint) \
    F(cl_device_info, CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT, cl_uint) \
    F(cl_device_info, CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE, cl_uint) \
    F(cl_device_info, CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF, cl_uint) \
    F(cl_device_info, CL_DEVICE_DOUBLE_FP_CONFIG, cl_device_fp_config) \
    F(cl_device_info, CL_DEVICE_HALF_FP_CONFIG, cl_device_fp_config) \
    F(cl_device_info, CL_DEVICE_HOST_UNIFIED_MEMORY, cl_bool) \
    F(cl_device_info, CL_DEVICE_OPENCL_C_VERSION, string_class) \
    \
    F(cl_mem_info, CL_MEM_ASSOCIATED_MEMOBJECT, cl::Memory) \
    F(cl_mem_info, CL_MEM_OFFSET, ::size_t) \
    \
    F(cl_kernel_work_group_info, CL_KERNEL_PREFERRED_WORK_GROUP_SIZE_MULTIPLE, ::size_t) \
    F(cl_kernel_work_group_info, CL_KERNEL_PRIVATE_MEM_SIZE, cl_ulong) \
    \
    F(cl_event_info, CL_EVENT_CONTEXT, cl::Context)

#define CL_HPP_PARAM_NAME_INFO_1_2_(F) \
    F(cl_image_info, CL_IMAGE_BUFFER, cl::Buffer) \
    \
    F(cl_program_info, CL_PROGRAM_NUM_KERNELS, ::size_t) \
    F(cl_program_info, CL_PROGRAM_KERNEL_NAMES, string_class) \
    \
    F(cl_program_build_info, CL_PROGRAM_BINARY_TYPE, cl_program_binary_type) \
    \
    F(cl_kernel_info, CL_KERNEL_ATTRIBUTES, string_class) \
    \
    F(cl_kernel_arg_info, CL_KERNEL_ARG_ADDRESS_QUALIFIER, cl_kernel_arg_address_qualifier) \
    F(cl_kernel_arg_info, CL_KERNEL_ARG_ACCESS_QUALIFIER, cl_kernel_arg_access_qualifier) \
    F(cl_kernel_arg_info, CL_KERNEL_ARG_TYPE_NAME, string_class) \
    F(cl_kernel_arg_info, CL_KERNEL_ARG_NAME, string_class) \
    \
    F(cl_device_info, CL_DEVICE_PARENT_DEVICE, cl_device_id) \
    F(cl_device_info, CL_DEVICE_PARTITION_PROPERTIES, vector_class<cl_device_partition_property>) \
    F(cl_device_info, CL_DEVICE_PARTITION_TYPE, vector_class<cl_device_partition_property>)  \
    F(cl_device_info, CL_DEVICE_REFERENCE_COUNT, cl_uint) \
    F(cl_device_info, CL_DEVICE_PREFERRED_INTEROP_USER_SYNC, ::size_t) \
    F(cl_device_info, CL_DEVICE_PARTITION_AFFINITY_DOMAIN, cl_device_affinity_domain) \
    F(cl_device_info, CL_DEVICE_BUILT_IN_KERNELS, string_class)

#define CL_HPP_PARAM_NAME_DEVICE_FISSION_(F) \
    F(cl_device_info, CL_DEVICE_PARENT_DEVICE_EXT, cl_device_id) \
    F(cl_device_info, CL_DEVICE_PARTITION_TYPES_EXT, vector_class<cl_device_partition_property_ext>) \
    F(cl_device_info, CL_DEVICE_AFFINITY_DOMAINS_EXT, vector_class<cl_device_partition_property_ext>) \
    F(cl_device_info, CL_DEVICE_REFERENCE_COUNT_EXT , cl_uint) \
    F(cl_device_info, CL_DEVICE_PARTITION_STYLE_EXT, vector_class<cl_device_partition_property_ext>)

template <typename enum_type, cl_int Name>
struct param_traits {};

#define CL_HPP_DECLARE_PARAM_TRAITS_(token, param_name, T) \
struct token;                                        \
template<>                                           \
struct param_traits<detail:: token,param_name>       \
{                                                    \
    enum { value = param_name };                     \
    typedef T param_type;                            \
};

CL_HPP_PARAM_NAME_INFO_1_0_(CL_HPP_DECLARE_PARAM_TRAITS_)
#if CL_HPP_TARGET_OPENCL_VERSION >= 110
CL_HPP_PARAM_NAME_INFO_1_1_(CL_HPP_DECLARE_PARAM_TRAITS_)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
CL_HPP_PARAM_NAME_INFO_1_2_(CL_HPP_DECLARE_PARAM_TRAITS_)
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110

#if defined(CL_HPP_USE_CL_DEVICE_FISSION)
CL_HPP_PARAM_NAME_DEVICE_FISSION_(CL_HPP_DECLARE_PARAM_TRAITS_);
#endif // CL_HPP_USE_CL_DEVICE_FISSION

#ifdef CL_PLATFORM_ICD_SUFFIX_KHR
CL_HPP_DECLARE_PARAM_TRAITS_(cl_platform_info, CL_PLATFORM_ICD_SUFFIX_KHR, string_class)
#endif

#ifdef CL_DEVICE_PROFILING_TIMER_OFFSET_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_PROFILING_TIMER_OFFSET_AMD, cl_ulong)
#endif

#ifdef CL_DEVICE_GLOBAL_FREE_MEMORY_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_GLOBAL_FREE_MEMORY_AMD, vector_class< ::size_t>)
#endif
#ifdef CL_DEVICE_SIMD_PER_COMPUTE_UNIT_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_SIMD_PER_COMPUTE_UNIT_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_SIMD_WIDTH_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_SIMD_WIDTH_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_SIMD_INSTRUCTION_WIDTH_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_SIMD_INSTRUCTION_WIDTH_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_WAVEFRONT_WIDTH_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_WAVEFRONT_WIDTH_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_GLOBAL_MEM_CHANNELS_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_GLOBAL_MEM_CHANNELS_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_GLOBAL_MEM_CHANNEL_BANKS_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_GLOBAL_MEM_CHANNEL_BANKS_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_GLOBAL_MEM_CHANNEL_BANK_WIDTH_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_GLOBAL_MEM_CHANNEL_BANK_WIDTH_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_LOCAL_MEM_SIZE_PER_COMPUTE_UNIT_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_LOCAL_MEM_SIZE_PER_COMPUTE_UNIT_AMD, cl_uint)
#endif
#ifdef CL_DEVICE_LOCAL_MEM_BANKS_AMD
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_LOCAL_MEM_BANKS_AMD, cl_uint)
#endif

#ifdef CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_COMPUTE_CAPABILITY_MAJOR_NV, cl_uint)
#endif
#ifdef CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_COMPUTE_CAPABILITY_MINOR_NV, cl_uint)
#endif
#ifdef CL_DEVICE_REGISTERS_PER_BLOCK_NV
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_REGISTERS_PER_BLOCK_NV, cl_uint)
#endif
#ifdef CL_DEVICE_WARP_SIZE_NV
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_WARP_SIZE_NV, cl_uint)
#endif
#ifdef CL_DEVICE_GPU_OVERLAP_NV
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_GPU_OVERLAP_NV, cl_bool)
#endif
#ifdef CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_KERNEL_EXEC_TIMEOUT_NV, cl_bool)
#endif
#ifdef CL_DEVICE_INTEGRATED_MEMORY_NV
CL_HPP_DECLARE_PARAM_TRAITS_(cl_device_info, CL_DEVICE_INTEGRATED_MEMORY_NV, cl_bool)
#endif

// Convenience functions

template <typename Func, typename T>
inline cl_int
getInfo(Func f, cl_uint name, T* param)
{
    return getInfoHelper(f, name, param, 0);
}

template <typename Func, typename Arg0>
struct GetInfoFunctor0
{
    Func f_; const Arg0& arg0_;
    cl_int operator ()(
        cl_uint param, ::size_t size, void* value, ::size_t* size_ret)
    { return f_(arg0_, param, size, value, size_ret); }
};

template <typename Func, typename Arg0, typename Arg1>
struct GetInfoFunctor1
{
    Func f_; const Arg0& arg0_; const Arg1& arg1_;
    cl_int operator ()(
        cl_uint param, ::size_t size, void* value, ::size_t* size_ret)
    { return f_(arg0_, arg1_, param, size, value, size_ret); }
};

template <typename Func, typename Arg0, typename T>
inline cl_int
getInfo(Func f, const Arg0& arg0, cl_uint name, T* param)
{
    GetInfoFunctor0<Func, Arg0> f0 = { f, arg0 };
    return getInfoHelper(f0, name, param, 0);
}

template <typename Func, typename Arg0, typename Arg1, typename T>
inline cl_int
getInfo(Func f, const Arg0& arg0, const Arg1& arg1, cl_uint name, T* param)
{
    GetInfoFunctor1<Func, Arg0, Arg1> f0 = { f, arg0, arg1 };
    return getInfoHelper(f0, name, param, 0);
}

template<typename T>
struct ReferenceHandler
{ };

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
/**
 * OpenCL 1.2 devices do have retain/release.
 */
template <>
struct ReferenceHandler<cl_device_id>
{
    /**
     * Retain the device.
     * \param device A valid device created using createSubDevices
     * \return 
     *   CL_SUCCESS if the function executed successfully.
     *   CL_INVALID_DEVICE if device was not a valid subdevice
     *   CL_OUT_OF_RESOURCES
     *   CL_OUT_OF_HOST_MEMORY
     */
    static cl_int retain(cl_device_id device)
    { return ::clRetainDevice(device); }
    /**
     * Retain the device.
     * \param device A valid device created using createSubDevices
     * \return 
     *   CL_SUCCESS if the function executed successfully.
     *   CL_INVALID_DEVICE if device was not a valid subdevice
     *   CL_OUT_OF_RESOURCES
     *   CL_OUT_OF_HOST_MEMORY
     */
    static cl_int release(cl_device_id device)
    { return ::clReleaseDevice(device); }
};
#else // CL_HPP_TARGET_OPENCL_VERSION >= 120
/**
 * OpenCL 1.1 devices do not have retain/release.
 */
template <>
struct ReferenceHandler<cl_device_id>
{
    // cl_device_id does not have retain().
    static cl_int retain(cl_device_id)
    { return CL_SUCCESS; }
    // cl_device_id does not have release().
    static cl_int release(cl_device_id)
    { return CL_SUCCESS; }
};
#endif // ! (CL_HPP_TARGET_OPENCL_VERSION >= 120)

template <>
struct ReferenceHandler<cl_platform_id>
{
    // cl_platform_id does not have retain().
    static cl_int retain(cl_platform_id)
    { return CL_SUCCESS; }
    // cl_platform_id does not have release().
    static cl_int release(cl_platform_id)
    { return CL_SUCCESS; }
};

template <>
struct ReferenceHandler<cl_context>
{
    static cl_int retain(cl_context context)
    { return ::clRetainContext(context); }
    static cl_int release(cl_context context)
    { return ::clReleaseContext(context); }
};

template <>
struct ReferenceHandler<cl_command_queue>
{
    static cl_int retain(cl_command_queue queue)
    { return ::clRetainCommandQueue(queue); }
    static cl_int release(cl_command_queue queue)
    { return ::clReleaseCommandQueue(queue); }
};

template <>
struct ReferenceHandler<cl_mem>
{
    static cl_int retain(cl_mem memory)
    { return ::clRetainMemObject(memory); }
    static cl_int release(cl_mem memory)
    { return ::clReleaseMemObject(memory); }
};

template <>
struct ReferenceHandler<cl_sampler>
{
    static cl_int retain(cl_sampler sampler)
    { return ::clRetainSampler(sampler); }
    static cl_int release(cl_sampler sampler)
    { return ::clReleaseSampler(sampler); }
};

template <>
struct ReferenceHandler<cl_program>
{
    static cl_int retain(cl_program program)
    { return ::clRetainProgram(program); }
    static cl_int release(cl_program program)
    { return ::clReleaseProgram(program); }
};

template <>
struct ReferenceHandler<cl_kernel>
{
    static cl_int retain(cl_kernel kernel)
    { return ::clRetainKernel(kernel); }
    static cl_int release(cl_kernel kernel)
    { return ::clReleaseKernel(kernel); }
};

template <>
struct ReferenceHandler<cl_event>
{
    static cl_int retain(cl_event event)
    { return ::clRetainEvent(event); }
    static cl_int release(cl_event event)
    { return ::clReleaseEvent(event); }
};


// Extracts version number with major in the upper 16 bits, minor in the lower 16
static cl_uint getVersion(const char *versionInfo)
{
    int highVersion = 0;
    int lowVersion = 0;
    int index = 7;
    while(versionInfo[index] != '.' ) {
        highVersion *= 10;
        highVersion += versionInfo[index]-'0';
        ++index;
    }
    ++index;
    while(versionInfo[index] != ' ' &&  versionInfo[index] != '\0') {
        lowVersion *= 10;
        lowVersion += versionInfo[index]-'0';
        ++index;
    }
    return (highVersion << 16) | lowVersion;
}

#if CL_HPP_TARGET_OPENCL_VERSION >= 120 && CL_HPP_MINIMUM_OPENCL_VERSION < 120
static cl_uint getPlatformVersion(cl_platform_id platform)
{
    ::size_t size = 0;
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, 0, NULL, &size);
    char *versionInfo = (char *) alloca(size);
    clGetPlatformInfo(platform, CL_PLATFORM_VERSION, size, &versionInfo[0], &size);
    return getVersion(versionInfo);
}

static cl_uint getDevicePlatformVersion(cl_device_id device)
{
    cl_platform_id platform;
    clGetDeviceInfo(device, CL_DEVICE_PLATFORM, sizeof(platform), &platform, NULL);
    return getPlatformVersion(platform);
}

static cl_uint getContextPlatformVersion(cl_context context)
{
    // The platform cannot be queried directly, so we first have to grab a
    // device and obtain its context
    ::size_t size = 0;
    clGetContextInfo(context, CL_CONTEXT_DEVICES, 0, NULL, &size);
    if (size == 0)
        return 0;
    cl_device_id *devices = (cl_device_id *) alloca(size);
    clGetContextInfo(context, CL_CONTEXT_DEVICES, size, devices, NULL);
    return getDevicePlatformVersion(devices[0]);
}
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120 && CL_HPP_MINIMUM_OPENCL_VERSION < 120

template <typename T>
class Wrapper
{
public:
    typedef T cl_type;

protected:
    cl_type object_;

public:
    Wrapper() : object_(NULL) { }

    Wrapper(const cl_type &obj) : object_(obj) { }

    ~Wrapper()
    {
        if (object_ != NULL) { release(); }
    }

    Wrapper(const Wrapper<cl_type>& rhs)
    {
        object_ = rhs.object_;
        if (object_ != NULL) { detail::errHandler(retain(), __RETAIN_ERR); }
    }

    Wrapper(Wrapper<cl_type>&& rhs) CL_HPP_NOEXCEPT_
    {
        object_ = rhs.object_;
        rhs.object_ = NULL;
    }

    Wrapper<cl_type>& operator = (const Wrapper<cl_type>& rhs)
    {
        if (this != &rhs) {
            if (object_ != NULL) { detail::errHandler(release(), __RELEASE_ERR); }
            object_ = rhs.object_;
            if (object_ != NULL) { detail::errHandler(retain(), __RETAIN_ERR); }
        }
        return *this;
    }

    Wrapper<cl_type>& operator = (Wrapper<cl_type>&& rhs)
    {
        if (this != &rhs) {
            if (object_ != NULL) { detail::errHandler(release(), __RELEASE_ERR); }
            object_ = rhs.object_;
            rhs.object_ = NULL;
        }
        return *this;
    }

    Wrapper<cl_type>& operator = (const cl_type &rhs)
    {
        if (object_ != NULL) { detail::errHandler(release(), __RELEASE_ERR); }
        object_ = rhs;
        return *this;
    }

    cl_type operator ()() const { return object_; }

    cl_type& operator ()() { return object_; }

protected:
    template<typename Func, typename U>
    friend inline cl_int getInfoHelper(Func, cl_uint, U*, int, typename U::cl_type);

    cl_int retain() const
    {
        return ReferenceHandler<cl_type>::retain(object_);
    }

    cl_int release() const
    {
        return ReferenceHandler<cl_type>::release(object_);
    }
};

template <>
class Wrapper<cl_device_id>
{
public:
    typedef cl_device_id cl_type;

protected:
    cl_type object_;
    bool referenceCountable_;

    static bool isReferenceCountable(cl_device_id device)
    {
        bool retVal = false;
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
#if CL_HPP_MINIMUM_OPENCL_VERSION < 120
        if (device != NULL) {
            int version = getDevicePlatformVersion(device);
            if(version > ((1 << 16) + 1)) {
                retVal = true;
            }
        }
#else // CL_HPP_MINIMUM_OPENCL_VERSION < 120
        retVal = true;
#endif // CL_HPP_MINIMUM_OPENCL_VERSION < 120
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
        return retVal;
    }

public:
    Wrapper() : object_(NULL), referenceCountable_(false) 
    { 
    }
    
    Wrapper(const cl_type &obj) : object_(obj), referenceCountable_(false) 
    {
        referenceCountable_ = isReferenceCountable(obj); 
    }

    ~Wrapper()
    {
        if (object_ != NULL) { release(); }
    }
    
    Wrapper(const Wrapper<cl_type>& rhs)
    {
        object_ = rhs.object_;
        referenceCountable_ = isReferenceCountable(object_); 
        if (object_ != NULL) { detail::errHandler(retain(), __RETAIN_ERR); }
    }

    Wrapper(Wrapper<cl_type>&& rhs) CL_HPP_NOEXCEPT_
    {
        object_ = rhs.object_;
        referenceCountable_ = rhs.referenceCountable_;
        rhs.object_ = NULL;
        rhs.referenceCountable_ = false;
    }

    Wrapper<cl_type>& operator = (const Wrapper<cl_type>& rhs)
    {
        if (this != &rhs) {
            if (object_ != NULL) { detail::errHandler(release(), __RELEASE_ERR); }
            object_ = rhs.object_;
            referenceCountable_ = rhs.referenceCountable_;
            if (object_ != NULL) { detail::errHandler(retain(), __RETAIN_ERR); }
        }
        return *this;
    }

    Wrapper<cl_type>& operator = (Wrapper<cl_type>&& rhs)
    {
        if (this != &rhs) {
            if (object_ != NULL) { detail::errHandler(release(), __RELEASE_ERR); }
            object_ = rhs.object_;
            referenceCountable_ = rhs.referenceCountable_;
            rhs.object_ = NULL;
            rhs.referenceCountable_ = false;
        }
        return *this;
    }

    Wrapper<cl_type>& operator = (const cl_type &rhs)
    {
        if (object_ != NULL) { detail::errHandler(release(), __RELEASE_ERR); }
        object_ = rhs;
        referenceCountable_ = isReferenceCountable(object_); 
        return *this;
    }

    cl_type operator ()() const { return object_; }

    cl_type& operator ()() { return object_; }

protected:
    template<typename Func, typename U>
    friend inline cl_int getInfoHelper(Func, cl_uint, U*, int, typename U::cl_type);

    template<typename Func, typename U>
    friend inline cl_int getInfoHelper(Func, cl_uint, vector_class<U>*, int, typename U::cl_type);

    cl_int retain() const
    {
        if( referenceCountable_ ) {
            return ReferenceHandler<cl_type>::retain(object_);
        }
        else {
            return CL_SUCCESS;
        }
    }

    cl_int release() const
    {
        if( referenceCountable_ ) {
            return ReferenceHandler<cl_type>::release(object_);
        }
        else {
            return CL_SUCCESS;
        }
    }
};

} // namespace detail
//! \endcond

/*! \stuct ImageFormat
 *  \brief Adds constructors and member functions for cl_image_format.
 *
 *  \see cl_image_format
 */
struct ImageFormat : public cl_image_format
{
    //! \brief Default constructor - performs no initialization.
    ImageFormat(){}

    //! \brief Initializing constructor.
    ImageFormat(cl_channel_order order, cl_channel_type type)
    {
        image_channel_order = order;
        image_channel_data_type = type;
    }

    //! \brief Assignment operator.
    ImageFormat& operator = (const ImageFormat& rhs)
    {
        if (this != &rhs) {
            this->image_channel_data_type = rhs.image_channel_data_type;
            this->image_channel_order     = rhs.image_channel_order;
        }
        return *this;
    }
};

/*! \brief Class interface for cl_device_id.
 *
 *  \note Copies of these objects are inexpensive, since they don't 'own'
 *        any underlying resources or data structures.
 *
 *  \see cl_device_id
 */
class Device : public detail::Wrapper<cl_device_id>
{
public:
    //! \brief Default constructor - initializes to NULL.
    Device() : detail::Wrapper<cl_type>() { }

    /*! \brief Constructor from cl_device_id.
     * 
     *  This simply copies the device ID value, which is an inexpensive operation.
     */
    Device(const cl_device_id &device) : detail::Wrapper<cl_type>(device) { }

    /*! \brief Returns the first device on the default context.
     *
     *  \see Context::getDefault()
     */
    static Device getDefault(cl_int * err = NULL);

    /*! \brief Assignment operator from cl_device_id.
     * 
     *  This simply copies the device ID value, which is an inexpensive operation.
     */
    Device& operator = (const cl_device_id& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
    * Required for MSVC.
    */
    Device(const Device& dev) : detail::Wrapper<cl_type>(dev) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
    * Required for MSVC.
    */
    Device& operator = (const Device &dev)
    {
        detail::Wrapper<cl_type>::operator=(dev);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
    * Required for MSVC.
    */
    Device(Device&& dev) CL_HPP_NOEXCEPT_ : detail::Wrapper<cl_type>(std::move(dev)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
    * Required for MSVC.
    */
    Device& operator = (Device &&dev)
    {
        detail::Wrapper<cl_type>::operator=(std::move(dev));
        return *this;
    }

    //! \brief Wrapper for clGetDeviceInfo().
    template <typename T>
    cl_int getInfo(cl_device_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetDeviceInfo, object_, name, param),
            __GET_DEVICE_INFO_ERR);
    }

    //! \brief Wrapper for clGetDeviceInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_device_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_device_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    /**
     * CL 1.2 version
     */
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    //! \brief Wrapper for clCreateSubDevices().
    cl_int createSubDevices(
        const cl_device_partition_property * properties,
        vector_class<Device>* devices)
    {
        cl_uint n = 0;
        cl_int err = clCreateSubDevices(object_, properties, 0, NULL, &n);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __CREATE_SUB_DEVICES_ERR);
        }

        cl_device_id* ids = (cl_device_id*) alloca(n * sizeof(cl_device_id));
        err = clCreateSubDevices(object_, properties, n, ids, NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __CREATE_SUB_DEVICES_ERR);
        }

        devices->assign(&ids[0], &ids[n]);
        return CL_SUCCESS;
    }
#elif defined(CL_HPP_USE_CL_DEVICE_FISSION)

/**
 * CL 1.1 version that uses device fission extension.
 */
    cl_int createSubDevices(
        const cl_device_partition_property_ext * properties,
        vector_class<Device>* devices)
    {
        typedef CL_API_ENTRY cl_int 
            ( CL_API_CALL * PFN_clCreateSubDevicesEXT)(
                cl_device_id /*in_device*/,
                const cl_device_partition_property_ext * /* properties */,
                cl_uint /*num_entries*/,
                cl_device_id * /*out_devices*/,
                cl_uint * /*num_devices*/ ) CL_EXT_SUFFIX__VERSION_1_1;

        static PFN_clCreateSubDevicesEXT pfn_clCreateSubDevicesEXT = NULL;
        CL_HPP_INIT_CL_EXT_FCN_PTR_(clCreateSubDevicesEXT);

        cl_uint n = 0;
        cl_int err = pfn_clCreateSubDevicesEXT(object_, properties, 0, NULL, &n);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __CREATE_SUB_DEVICES_ERR);
        }

        cl_device_id* ids = (cl_device_id*) alloca(n * sizeof(cl_device_id));
        err = pfn_clCreateSubDevicesEXT(object_, properties, n, ids, NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __CREATE_SUB_DEVICES_ERR);
        }

        devices->assign(&ids[0], &ids[n]);
        return CL_SUCCESS;
    }
#endif // defined(CL_HPP_USE_CL_DEVICE_FISSION)
};

/*! \brief Class interface for cl_platform_id.
 *
 *  \note Copies of these objects are inexpensive, since they don't 'own'
 *        any underlying resources or data structures.
 *
 *  \see cl_platform_id
 */
class Platform : public detail::Wrapper<cl_platform_id>
{
public:
    //! \brief Default constructor - initializes to NULL.
    Platform() : detail::Wrapper<cl_type>()  { }

    /*! \brief Constructor from cl_platform_id.
     * 
     *  This simply copies the platform ID value, which is an inexpensive operation.
     */
    Platform(const cl_platform_id &platform) : detail::Wrapper<cl_type>(platform) { }

    /*! \brief Assignment operator from cl_platform_id.
     * 
     *  This simply copies the platform ID value, which is an inexpensive operation.
     */
    Platform& operator = (const cl_platform_id& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    //! \brief Wrapper for clGetPlatformInfo().
    cl_int getInfo(cl_platform_info name, string_class* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetPlatformInfo, object_, name, param),
            __GET_PLATFORM_INFO_ERR);
    }

    //! \brief Wrapper for clGetPlatformInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_platform_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_platform_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    /*! \brief Gets a list of devices for this platform.
     * 
     *  Wraps clGetDeviceIDs().
     */
    cl_int getDevices(
        cl_device_type type,
        vector_class<Device>* devices) const
    {
        cl_uint n = 0;
        if( devices == NULL ) {
            return detail::errHandler(CL_INVALID_ARG_VALUE, __GET_DEVICE_IDS_ERR);
        }
        cl_int err = ::clGetDeviceIDs(object_, type, 0, NULL, &n);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_DEVICE_IDS_ERR);
        }

        cl_device_id* ids = (cl_device_id*) alloca(n * sizeof(cl_device_id));
        err = ::clGetDeviceIDs(object_, type, n, ids, NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_DEVICE_IDS_ERR);
        }

        devices->assign(&ids[0], &ids[n]);
        return CL_SUCCESS;
    }

#if defined(CL_HPP_USE_DX_INTEROP)
   /*! \brief Get the list of available D3D10 devices.
     *
     *  \param d3d_device_source.
     *
     *  \param d3d_object.
     *
     *  \param d3d_device_set.
     *
     *  \param devices returns a vector of OpenCL D3D10 devices found. The cl::Device
     *  values returned in devices can be used to identify a specific OpenCL
     *  device. If \a devices argument is NULL, this argument is ignored.
     *
     *  \return One of the following values:
     *    - CL_SUCCESS if the function is executed successfully.
     *
     *  The application can query specific capabilities of the OpenCL device(s)
     *  returned by cl::getDevices. This can be used by the application to
     *  determine which device(s) to use.
     *
     * \note In the case that exceptions are enabled and a return value
     * other than CL_SUCCESS is generated, then cl::Error exception is
     * generated.
     */
    cl_int getDevices(
        cl_d3d10_device_source_khr d3d_device_source,
        void *                     d3d_object,
        cl_d3d10_device_set_khr    d3d_device_set,
        vector_class<Device>* devices) const
    {
        typedef CL_API_ENTRY cl_int (CL_API_CALL *PFN_clGetDeviceIDsFromD3D10KHR)(
            cl_platform_id platform, 
            cl_d3d10_device_source_khr d3d_device_source, 
            void * d3d_object,
            cl_d3d10_device_set_khr d3d_device_set,
            cl_uint num_entries,
            cl_device_id * devices,
            cl_uint* num_devices);

        if( devices == NULL ) {
            return detail::errHandler(CL_INVALID_ARG_VALUE, __GET_DEVICE_IDS_ERR);
        }

        static PFN_clGetDeviceIDsFromD3D10KHR pfn_clGetDeviceIDsFromD3D10KHR = NULL;
        CL_HPP_INIT_CL_EXT_FCN_PTR_PLATFORM_(object_, clGetDeviceIDsFromD3D10KHR);

        cl_uint n = 0;
        cl_int err = pfn_clGetDeviceIDsFromD3D10KHR(
            object_, 
            d3d_device_source, 
            d3d_object,
            d3d_device_set, 
            0, 
            NULL, 
            &n);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_DEVICE_IDS_ERR);
        }

        cl_device_id* ids = (cl_device_id*) alloca(n * sizeof(cl_device_id));
        err = pfn_clGetDeviceIDsFromD3D10KHR(
            object_, 
            d3d_device_source, 
            d3d_object,
            d3d_device_set,
            n, 
            ids, 
            NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_DEVICE_IDS_ERR);
        }

        devices->assign(&ids[0], &ids[n]);
        return CL_SUCCESS;
    }
#endif

    /*! \brief Gets a list of available platforms.
     * 
     *  Wraps clGetPlatformIDs().
     */
    static cl_int get(
        vector_class<Platform>* platforms)
    {
        cl_uint n = 0;

        if( platforms == NULL ) {
            return detail::errHandler(CL_INVALID_ARG_VALUE, __GET_PLATFORM_IDS_ERR);
        }

        cl_int err = ::clGetPlatformIDs(0, NULL, &n);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_PLATFORM_IDS_ERR);
        }

        cl_platform_id* ids = (cl_platform_id*) alloca(
            n * sizeof(cl_platform_id));
        err = ::clGetPlatformIDs(n, ids, NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_PLATFORM_IDS_ERR);
        }

        platforms->assign(&ids[0], &ids[n]);
        return CL_SUCCESS;
    }

    /*! \brief Gets the first available platform.
     * 
     *  Wraps clGetPlatformIDs(), returning the first result.
     */
    static cl_int get(
        Platform * platform)
    {
        cl_uint n = 0;

        if( platform == NULL ) {
            return detail::errHandler(CL_INVALID_ARG_VALUE, __GET_PLATFORM_IDS_ERR);
        }

        cl_int err = ::clGetPlatformIDs(0, NULL, &n);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_PLATFORM_IDS_ERR);
        }

        cl_platform_id* ids = (cl_platform_id*) alloca(
            n * sizeof(cl_platform_id));
        err = ::clGetPlatformIDs(n, ids, NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_PLATFORM_IDS_ERR);
        }

        *platform = ids[0];
        return CL_SUCCESS;
    }

    /*! \brief Gets the first available platform, returning it by value.
     * 
     *  Wraps clGetPlatformIDs(), returning the first result.
     */
    static Platform get(
        cl_int * errResult = NULL)
    {
        Platform platform;
        cl_uint n = 0;
        cl_int err = ::clGetPlatformIDs(0, NULL, &n);
        if (err != CL_SUCCESS) {
            detail::errHandler(err, __GET_PLATFORM_IDS_ERR);
            if (errResult != NULL) {
                *errResult = err;
            }
        }

        cl_platform_id* ids = (cl_platform_id*) alloca(
            n * sizeof(cl_platform_id));
        err = ::clGetPlatformIDs(n, ids, NULL);

        if (err != CL_SUCCESS) {
            detail::errHandler(err, __GET_PLATFORM_IDS_ERR);
        }

        if (errResult != NULL) {
            *errResult = err;
        }
        
        return ids[0];
    }

    static Platform getDefault( 
        cl_int *errResult = NULL )
    {
        return get(errResult);
    }

    
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    //! \brief Wrapper for clUnloadCompiler().
    cl_int
    unloadCompiler()
    {
        return ::clUnloadPlatformCompiler(object_);
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
}; // class Platform

/**
 * Deprecated APIs for 1.2
 */
#if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)
/**
 * Unload the OpenCL compiler.
 * \note Deprecated for OpenCL 1.2. Use Platform::unloadCompiler instead.
 */
inline CL_EXT_PREFIX__VERSION_1_1_DEPRECATED cl_int
UnloadCompiler() CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED;
inline cl_int
UnloadCompiler()
{
    return ::clUnloadCompiler();
}
#endif // #if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)

/*! \brief Class interface for cl_context.
 *
 *  \note Copies of these objects are shallow, meaning that the copy will refer
 *        to the same underlying cl_context as the original.  For details, see
 *        clRetainContext() and clReleaseContext().
 *
 *  \see cl_context
 */
class Context 
    : public detail::Wrapper<cl_context>
{
private:
    static std::once_flag default_initialized_;
    static Context default_;
    static cl_int default_error_;

    /*! \brief Create the default context.
     *
     * This sets @c default_ and @c default_error_. It does not throw
     * @c cl::Error.
     */
    static void makeDefault() {
        /* Throwing an exception from a call_once invocation does not do
         * what we wish, so we catch it and save the error.
         */
#if defined(CL_HPP_ENABLE_EXCEPTIONS)
        try
#endif
        {
            default_ = Context(
                CL_DEVICE_TYPE_DEFAULT,
                NULL,
                NULL,
                NULL,
                &default_error_);
        }
#if defined(CL_HPP_ENABLE_EXCEPTIONS)
        catch (cl::Error &e) {
            default_error_ = e.err();
        }
#endif
    }
public:
    /*! \brief Constructs a context including a list of specified devices.
     *
     *  Wraps clCreateContext().
     */
    Context(
        const vector_class<Device>& devices,
        cl_context_properties* properties = NULL,
        void (CL_CALLBACK * notifyFptr)(
            const char *,
            const void *,
            ::size_t,
            void *) = NULL,
        void* data = NULL,
        cl_int* err = NULL)
    {
        cl_int error;

        ::size_t numDevices = devices.size();
        cl_device_id* deviceIDs = (cl_device_id*) alloca(numDevices * sizeof(cl_device_id));
        for( ::size_t deviceIndex = 0; deviceIndex < numDevices; ++deviceIndex ) {
            deviceIDs[deviceIndex] = (devices[deviceIndex])();
        }

        object_ = ::clCreateContext(
            properties, (cl_uint) numDevices,
            deviceIDs,
            notifyFptr, data, &error);

        detail::errHandler(error, __CREATE_CONTEXT_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    Context(
        const Device& device,
        cl_context_properties* properties = NULL,
        void (CL_CALLBACK * notifyFptr)(
            const char *,
            const void *,
            ::size_t,
            void *) = NULL,
        void* data = NULL,
        cl_int* err = NULL)
    {
        cl_int error;

        cl_device_id deviceID = device();

        object_ = ::clCreateContext(
            properties, 1,
            &deviceID,
            notifyFptr, data, &error);

        detail::errHandler(error, __CREATE_CONTEXT_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    /*! \brief Constructs a context including all or a subset of devices of a specified type.
     *
     *  Wraps clCreateContextFromType().
     */
    Context(
        cl_device_type type,
        cl_context_properties* properties = NULL,
        void (CL_CALLBACK * notifyFptr)(
            const char *,
            const void *,
            ::size_t,
            void *) = NULL,
        void* data = NULL,
        cl_int* err = NULL)
    {
        cl_int error;

#if !defined(__APPLE__) && !defined(__MACOS)
        cl_context_properties prop[4] = {CL_CONTEXT_PLATFORM, 0, 0, 0 };

        if (properties == NULL) {
            // Get a valid platform ID as we cannot send in a blank one
            vector_class<Platform> platforms;
            error = Platform::get(&platforms);
            if (error != CL_SUCCESS) {
                detail::errHandler(error, __CREATE_CONTEXT_FROM_TYPE_ERR);
                if (err != NULL) {
                    *err = error;
                }
                return;
            }

            // Check the platforms we found for a device of our specified type
            cl_context_properties platform_id = 0;
            for (unsigned int i = 0; i < platforms.size(); i++) {

                vector_class<Device> devices;

#if defined(CL_HPP_ENABLE_EXCEPTIONS)
                try {
#endif

                    error = platforms[i].getDevices(type, &devices);

#if defined(CL_HPP_ENABLE_EXCEPTIONS)
                } catch (Error) {}
    // Catch if exceptions are enabled as we don't want to exit if first platform has no devices of type
    // We do error checking next anyway, and can throw there if needed
#endif

                // Only squash CL_SUCCESS and CL_DEVICE_NOT_FOUND
                if (error != CL_SUCCESS && error != CL_DEVICE_NOT_FOUND) {
                    detail::errHandler(error, __CREATE_CONTEXT_FROM_TYPE_ERR);
                    if (err != NULL) {
                        *err = error;
                    }
                }

                if (devices.size() > 0) {
                    platform_id = (cl_context_properties)platforms[i]();
                    break;
                }
            }

            if (platform_id == 0) {
                detail::errHandler(CL_DEVICE_NOT_FOUND, __CREATE_CONTEXT_FROM_TYPE_ERR);
                if (err != NULL) {
                    *err = CL_DEVICE_NOT_FOUND;
                }
                return;
            }

            prop[1] = platform_id;
            properties = &prop[0];
        }
#endif
        object_ = ::clCreateContextFromType(
            properties, type, notifyFptr, data, &error);

        detail::errHandler(error, __CREATE_CONTEXT_FROM_TYPE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Context(const Context& ctx) : detail::Wrapper<cl_type>(ctx) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Context& operator = (const Context &ctx)
    {
        detail::Wrapper<cl_type>::operator=(ctx);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Context(Context&& ctx) CL_HPP_NOEXCEPT_ : detail::Wrapper<cl_type>(std::move(ctx)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Context& operator = (Context &&ctx)
    {
        detail::Wrapper<cl_type>::operator=(std::move(ctx));
        return *this;
    }


    /*! \brief Returns a singleton context including all devices of CL_DEVICE_TYPE_DEFAULT.
     *
     *  \note All calls to this function return the same cl_context as the first.
     */
    static Context getDefault(cl_int * err = NULL) 
    {
        std::call_once(default_initialized_, makeDefault);
        detail::errHandler(default_error_);
        if (err != NULL) {
            *err = default_error_;
        }
        return default_;
    }

    //! \brief Default constructor - initializes to NULL.
    Context() : detail::Wrapper<cl_type>() { }

    /*! \brief Constructor from cl_context - takes ownership.
     * 
     *  This effectively transfers ownership of a refcount on the cl_context
     *  into the new Context object.
     */
    explicit Context(const cl_context& context) : detail::Wrapper<cl_type>(context) { }

    /*! \brief Assignment operator from cl_context - takes ownership.
     * 
     *  This effectively transfers ownership of a refcount on the rhs and calls
     *  clReleaseContext() on the value previously held by this instance.
     */
    Context& operator = (const cl_context& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    //! \brief Wrapper for clGetContextInfo().
    template <typename T>
    cl_int getInfo(cl_context_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetContextInfo, object_, name, param),
            __GET_CONTEXT_INFO_ERR);
    }

    //! \brief Wrapper for clGetContextInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_context_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_context_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    /*! \brief Gets a list of supported image formats.
     *  
     *  Wraps clGetSupportedImageFormats().
     */
    cl_int getSupportedImageFormats(
        cl_mem_flags flags,
        cl_mem_object_type type,
        vector_class<ImageFormat>* formats) const
    {
        cl_uint numEntries;
        cl_int err = ::clGetSupportedImageFormats(
           object_, 
           flags,
           type, 
           0, 
           NULL, 
           &numEntries);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_SUPPORTED_IMAGE_FORMATS_ERR);
        }

        ImageFormat* value = (ImageFormat*)
            alloca(numEntries * sizeof(ImageFormat));
        err = ::clGetSupportedImageFormats(
            object_, 
            flags, 
            type, 
            numEntries,
            (cl_image_format*) value, 
            NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __GET_SUPPORTED_IMAGE_FORMATS_ERR);
        }

        formats->assign(&value[0], &value[numEntries]);
        return CL_SUCCESS;
    }
};

inline Device Device::getDefault(cl_int * err)
{
    cl_int error;
    Device device;

    Context context = Context::getDefault(&error);
    detail::errHandler(error, __CREATE_COMMAND_QUEUE_ERR);

    if (error != CL_SUCCESS) {
        if (err != NULL) {
            *err = error;
        }
    }
    else {
        device = context.getInfo<CL_CONTEXT_DEVICES>()[0];
        if (err != NULL) {
            *err = CL_SUCCESS;
        }
    }

    return device;
}



CL_HPP_DEFINE_STATIC_MEMBER_ std::once_flag Context::default_initialized_;
CL_HPP_DEFINE_STATIC_MEMBER_ Context Context::default_;
CL_HPP_DEFINE_STATIC_MEMBER_ cl_int Context::default_error_ = CL_SUCCESS;

/*! \brief Class interface for cl_event.
 *
 *  \note Copies of these objects are shallow, meaning that the copy will refer
 *        to the same underlying cl_event as the original.  For details, see
 *        clRetainEvent() and clReleaseEvent().
 *
 *  \see cl_event
 */
class Event : public detail::Wrapper<cl_event>
{
public:
    //! \brief Default constructor - initializes to NULL.
    Event() : detail::Wrapper<cl_type>() { }

    /*! \brief Constructor from cl_event - takes ownership.
     * 
     *  This effectively transfers ownership of a refcount on the cl_event
     *  into the new Event object.
     */
    Event(const cl_event& event) : detail::Wrapper<cl_type>(event) { }

    /*! \brief Assignment operator from cl_event - takes ownership.
     *
     *  This effectively transfers ownership of a refcount on the rhs and calls
     *  clReleaseEvent() on the value previously held by this instance.
     */
    Event& operator = (const cl_event& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    //! \brief Wrapper for clGetEventInfo().
    template <typename T>
    cl_int getInfo(cl_event_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetEventInfo, object_, name, param),
            __GET_EVENT_INFO_ERR);
    }

    //! \brief Wrapper for clGetEventInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_event_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_event_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    //! \brief Wrapper for clGetEventProfilingInfo().
    template <typename T>
    cl_int getProfilingInfo(cl_profiling_info name, T* param) const
    {
        return detail::errHandler(detail::getInfo(
            &::clGetEventProfilingInfo, object_, name, param),
            __GET_EVENT_PROFILE_INFO_ERR);
    }

    //! \brief Wrapper for clGetEventProfilingInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_profiling_info, name>::param_type
    getProfilingInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_profiling_info, name>::param_type param;
        cl_int result = getProfilingInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    /*! \brief Blocks the calling thread until this event completes.
     * 
     *  Wraps clWaitForEvents().
     */
    cl_int wait() const
    {
        return detail::errHandler(
            ::clWaitForEvents(1, &object_),
            __WAIT_FOR_EVENTS_ERR);
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 110
    /*! \brief Registers a user callback function for a specific command execution status.
     *
     *  Wraps clSetEventCallback().
     */
    cl_int setCallback(
        cl_int type,
        void (CL_CALLBACK * pfn_notify)(cl_event, cl_int, void *),		
        void * user_data = NULL)
    {
        return detail::errHandler(
            ::clSetEventCallback(
                object_,
                type,
                pfn_notify,
                user_data), 
            __SET_EVENT_CALLBACK_ERR);
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110

    /*! \brief Blocks the calling thread until every event specified is complete.
     * 
     *  Wraps clWaitForEvents().
     */
    static cl_int
    waitForEvents(const vector_class<Event>& events)
    {
        return detail::errHandler(
            ::clWaitForEvents(
                (cl_uint) events.size(), (events.size() > 0) ? (cl_event*)&events.front() : NULL),
            __WAIT_FOR_EVENTS_ERR);
    }
};

#if CL_HPP_TARGET_OPENCL_VERSION >= 110
/*! \brief Class interface for user events (a subset of cl_event's).
 * 
 *  See Event for details about copy semantics, etc.
 */
class UserEvent : public Event
{
public:
    /*! \brief Constructs a user event on a given context.
     *
     *  Wraps clCreateUserEvent().
     */
    UserEvent(
        const Context& context,
        cl_int * err = NULL)
    {
        cl_int error;
        object_ = ::clCreateUserEvent(
            context(),
            &error);

        detail::errHandler(error, __CREATE_USER_EVENT_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    //! \brief Default constructor - initializes to NULL.
    UserEvent() : Event() { }

    /*! \brief Sets the execution status of a user event object.
     *
     *  Wraps clSetUserEventStatus().
     */
    cl_int setStatus(cl_int status)
    {
        return detail::errHandler(
            ::clSetUserEventStatus(object_,status), 
            __SET_USER_EVENT_STATUS_ERR);
    }
};
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110

/*! \brief Blocks the calling thread until every event specified is complete.
 * 
 *  Wraps clWaitForEvents().
 */
inline static cl_int
WaitForEvents(const vector_class<Event>& events)
{
    return detail::errHandler(
        ::clWaitForEvents(
            (cl_uint) events.size(), (events.size() > 0) ? (cl_event*)&events.front() : NULL),
        __WAIT_FOR_EVENTS_ERR);
}

/*! \brief Class interface for cl_mem.
 *
 *  \note Copies of these objects are shallow, meaning that the copy will refer
 *        to the same underlying cl_mem as the original.  For details, see
 *        clRetainMemObject() and clReleaseMemObject().
 *
 *  \see cl_mem
 */
class Memory : public detail::Wrapper<cl_mem>
{
public:
    //! \brief Default constructor - initializes to NULL.
    Memory() : detail::Wrapper<cl_type>() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     * 
     *  This effectively transfers ownership of a refcount on the cl_mem
     *  into the new Memory object.
     */
    explicit Memory(const cl_mem& memory) : detail::Wrapper<cl_type>(memory) { }

    /*! \brief Assignment operator from cl_mem - takes ownership.
     *
     *  This effectively transfers ownership of a refcount on the rhs and calls
     *  clReleaseMemObject() on the value previously held by this instance.
     */
    Memory& operator = (const cl_mem& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Memory(const Memory& mem) : detail::Wrapper<cl_type>(mem) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Memory& operator = (const Memory &mem)
    {
        detail::Wrapper<cl_type>::operator=(mem);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Memory(Memory&& mem) CL_HPP_NOEXCEPT_ : detail::Wrapper<cl_type>(std::move(mem)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Memory& operator = (Memory &&mem)
    {
        detail::Wrapper<cl_type>::operator=(std::move(mem));
        return *this;
    }


    //! \brief Wrapper for clGetMemObjectInfo().
    template <typename T>
    cl_int getInfo(cl_mem_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetMemObjectInfo, object_, name, param),
            __GET_MEM_OBJECT_INFO_ERR);
    }

    //! \brief Wrapper for clGetMemObjectInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_mem_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_mem_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 110
    /*! \brief Registers a callback function to be called when the memory object
     *         is no longer needed.
     *
     *  Wraps clSetMemObjectDestructorCallback().
     *
     *  Repeated calls to this function, for a given cl_mem value, will append
     *  to the list of functions called (in reverse order) when memory object's
     *  resources are freed and the memory object is deleted.
     *
     *  \note
     *  The registered callbacks are associated with the underlying cl_mem
     *  value - not the Memory class instance.
     */
    cl_int setDestructorCallback(
        void (CL_CALLBACK * pfn_notify)(cl_mem, void *),		
        void * user_data = NULL)
    {
        return detail::errHandler(
            ::clSetMemObjectDestructorCallback(
                object_,
                pfn_notify,
                user_data), 
            __SET_MEM_OBJECT_DESTRUCTOR_CALLBACK_ERR);
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110

};

// Pre-declare copy functions
class Buffer;
template< typename IteratorType >
cl_int copy( IteratorType startIterator, IteratorType endIterator, cl::Buffer &buffer );
template< typename IteratorType >
cl_int copy( const cl::Buffer &buffer, IteratorType startIterator, IteratorType endIterator );
template< typename IteratorType >
cl_int copy( const CommandQueue &queue, IteratorType startIterator, IteratorType endIterator, cl::Buffer &buffer );
template< typename IteratorType >
cl_int copy( const CommandQueue &queue, const cl::Buffer &buffer, IteratorType startIterator, IteratorType endIterator );


/*! \brief Class interface for Buffer Memory Objects.
 * 
 *  See Memory for details about copy semantics, etc.
 *
 *  \see Memory
 */
class Buffer : public Memory
{
public:

    /*! \brief Constructs a Buffer in a specified context.
     *
     *  Wraps clCreateBuffer().
     *
     *  \param host_ptr Storage to be used if the CL_MEM_USE_HOST_PTR flag was
     *                  specified.  Note alignment & exclusivity requirements.
     */
    Buffer(
        const Context& context,
        cl_mem_flags flags,
        ::size_t size,
        void* host_ptr = NULL,
        cl_int* err = NULL)
    {
        cl_int error;
        object_ = ::clCreateBuffer(context(), flags, size, host_ptr, &error);

        detail::errHandler(error, __CREATE_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    /*! \brief Constructs a Buffer in the default context.
     *
     *  Wraps clCreateBuffer().
     *
     *  \param host_ptr Storage to be used if the CL_MEM_USE_HOST_PTR flag was
     *                  specified.  Note alignment & exclusivity requirements.
     *
     *  \see Context::getDefault()
     */
    Buffer(
         cl_mem_flags flags,
        ::size_t size,
        void* host_ptr = NULL,
        cl_int* err = NULL)
    {
        cl_int error;

        Context context = Context::getDefault(err);

        object_ = ::clCreateBuffer(context(), flags, size, host_ptr, &error);

        detail::errHandler(error, __CREATE_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    /*!
     * \brief Construct a Buffer from a host container via iterators.
     * IteratorType must be random access.
     * If useHostPtr is specified iterators must represent contiguous data.
     */
    template< typename IteratorType >
    Buffer(
        IteratorType startIterator,
        IteratorType endIterator,
        bool readOnly,
        bool useHostPtr = false,
        cl_int* err = NULL)
    {
        typedef typename std::iterator_traits<IteratorType>::value_type DataType;
        cl_int error;

        cl_mem_flags flags = 0;
        if( readOnly ) {
            flags |= CL_MEM_READ_ONLY;
        }
        else {
            flags |= CL_MEM_READ_WRITE;
        }
        if( useHostPtr ) {
            flags |= CL_MEM_USE_HOST_PTR;
        }
        
        ::size_t size = sizeof(DataType)*(endIterator - startIterator);

        Context context = Context::getDefault(err);

        if( useHostPtr ) {
            object_ = ::clCreateBuffer(context(), flags, size, static_cast<DataType*>(&*startIterator), &error);
        } else {
            object_ = ::clCreateBuffer(context(), flags, size, 0, &error);
        }

        detail::errHandler(error, __CREATE_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }

        if( !useHostPtr ) {
            error = cl::copy(startIterator, endIterator, *this);
            detail::errHandler(error, __CREATE_BUFFER_ERR);
            if (err != NULL) {
                *err = error;
            }
        }
    }

    /*!
     * \brief Construct a Buffer from a host container via iterators using a specified context.
     * IteratorType must be random access.
     * If useHostPtr is specified iterators must represent contiguous data.
     */
    template< typename IteratorType >
    Buffer(const Context &context, IteratorType startIterator, IteratorType endIterator,
        bool readOnly, bool useHostPtr = false, cl_int* err = NULL);
    
    /*!
    * \brief Construct a Buffer from a host container via iterators using a specified queue.
    * If useHostPtr is specified iterators must be random access.
    */
    template< typename IteratorType >
    Buffer(const CommandQueue &queue, IteratorType startIterator, IteratorType endIterator,
        bool readOnly, bool useHostPtr = false, cl_int* err = NULL);

    //! \brief Default constructor - initializes to NULL.
    Buffer() : Memory() { }

    /*! \brief Constructor from cl_mem - takes ownership.
    *
    *  See Memory for further details.
    */
    explicit Buffer(const cl_mem& buffer) : Memory(buffer) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
    *
    *  See Memory for further details.
    */
    Buffer& operator = (const cl_mem& rhs)
    {
        Memory::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Buffer(const Buffer& buf) : Memory(buf) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Buffer& operator = (const Buffer &buf)
    {
        Memory::operator=(buf);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Buffer(Buffer&& buf) CL_HPP_NOEXCEPT_ : Memory(std::move(buf)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Buffer& operator = (Buffer &&buf)
    {
        Memory::operator=(std::move(buf));
        return *this;
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 110
    /*! \brief Creates a new buffer object from this.
     *
     *  Wraps clCreateSubBuffer().
     */
    Buffer createSubBuffer(
        cl_mem_flags flags,
        cl_buffer_create_type buffer_create_type,
        const void * buffer_create_info,
        cl_int * err = NULL)
    {
        Buffer result;
        cl_int error;
        result.object_ = ::clCreateSubBuffer(
            object_, 
            flags, 
            buffer_create_type, 
            buffer_create_info, 
            &error);

        detail::errHandler(error, __CREATE_SUBBUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }

        return result;
    }		
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110
};

#if defined (CL_HPP_USE_DX_INTEROP)
/*! \brief Class interface for creating OpenCL buffers from ID3D10Buffer's.
 *
 *  This is provided to facilitate interoperability with Direct3D.
 * 
 *  See Memory for details about copy semantics, etc.
 *
 *  \see Memory
 */
class BufferD3D10 : public Buffer
{
public:
    typedef CL_API_ENTRY cl_mem (CL_API_CALL *PFN_clCreateFromD3D10BufferKHR)(
    cl_context context, cl_mem_flags flags, ID3D10Buffer*  buffer,
    cl_int* errcode_ret);

    /*! \brief Constructs a BufferD3D10, in a specified context, from a
     *         given ID3D10Buffer.
     *
     *  Wraps clCreateFromD3D10BufferKHR().
     */
    BufferD3D10(
        const Context& context,
        cl_mem_flags flags,
        ID3D10Buffer* bufobj,
        cl_int * err = NULL)
    {
        static PFN_clCreateFromD3D10BufferKHR pfn_clCreateFromD3D10BufferKHR = NULL;

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
        vector<cl_context_properties> props = context.getInfo<CL_CONTEXT_PROPERTIES>();
        cl_platform platform = -1;
        for( int i = 0; i < props.size(); ++i ) {
            if( props[i] == CL_CONTEXT_PLATFORM ) {
                platform = props[i+1];
            }
        }
        CL_HPP_INIT_CL_EXT_FCN_PTR_PLATFORM_(platform, clCreateFromD3D10BufferKHR);
#endif
#if CL_HPP_TARGET_OPENCL_VERSION >= 110
        CL_HPP_INIT_CL_EXT_FCN_PTR_(clCreateFromD3D10BufferKHR);
#endif

        cl_int error;
        object_ = pfn_clCreateFromD3D10BufferKHR(
            context(),
            flags,
            bufobj,
            &error);

        detail::errHandler(error, __CREATE_GL_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    //! \brief Default constructor - initializes to NULL.
    BufferD3D10() : Buffer() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit BufferD3D10(const cl_mem& buffer) : Buffer(buffer) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    BufferD3D10& operator = (const cl_mem& rhs)
    {
        Buffer::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    BufferD3D10(const BufferD3D10& buf) : Buffer(buf) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    BufferD3D10& operator = (const BufferD3D10 &buf)
    {
        Buffer::operator=(buf);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    BufferD3D10(BufferD3D10&& buf) CL_HPP_NOEXCEPT_ : Buffer(std::move(buf)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    BufferD3D10& operator = (BufferD3D10 &&buf)
    {
        Buffer::operator=(std::move(buf));
        return *this;
    }
};
#endif

/*! \brief Class interface for GL Buffer Memory Objects.
 *
 *  This is provided to facilitate interoperability with OpenGL.
 * 
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 */
class BufferGL : public Buffer
{
public:
    /*! \brief Constructs a BufferGL in a specified context, from a given
     *         GL buffer.
     *
     *  Wraps clCreateFromGLBuffer().
     */
    BufferGL(
        const Context& context,
        cl_mem_flags flags,
        cl_GLuint bufobj,
        cl_int * err = NULL)
    {
        cl_int error;
        object_ = ::clCreateFromGLBuffer(
            context(),
            flags,
            bufobj,
            &error);

        detail::errHandler(error, __CREATE_GL_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    //! \brief Default constructor - initializes to NULL.
    BufferGL() : Buffer() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit BufferGL(const cl_mem& buffer) : Buffer(buffer) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    BufferGL& operator = (const cl_mem& rhs)
    {
        Buffer::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    BufferGL(const BufferGL& buf) : Buffer(buf) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    BufferGL& operator = (const BufferGL &buf)
    {
        Buffer::operator=(buf);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    BufferGL(BufferGL&& buf) CL_HPP_NOEXCEPT_ : Buffer(std::move(buf)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    BufferGL& operator = (BufferGL &&buf)
    {
        Buffer::operator=(std::move(buf));
        return *this;
    }

    //! \brief Wrapper for clGetGLObjectInfo().
    cl_int getObjectInfo(
        cl_gl_object_type *type,
        cl_GLuint * gl_object_name)
    {
        return detail::errHandler(
            ::clGetGLObjectInfo(object_,type,gl_object_name),
            __GET_GL_OBJECT_INFO_ERR);
    }
};

/*! \brief Class interface for GL Render Buffer Memory Objects.
 *
 *  This is provided to facilitate interoperability with OpenGL.
 * 
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 */
class BufferRenderGL : public Buffer
{
public:
    /*! \brief Constructs a BufferRenderGL in a specified context, from a given
     *         GL Renderbuffer.
     *
     *  Wraps clCreateFromGLRenderbuffer().
     */
    BufferRenderGL(
        const Context& context,
        cl_mem_flags flags,
        cl_GLuint bufobj,
        cl_int * err = NULL)
    {
        cl_int error;
        object_ = ::clCreateFromGLRenderbuffer(
            context(),
            flags,
            bufobj,
            &error);

        detail::errHandler(error, __CREATE_GL_RENDER_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    //! \brief Default constructor - initializes to NULL.
    BufferRenderGL() : Buffer() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit BufferRenderGL(const cl_mem& buffer) : Buffer(buffer) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    BufferRenderGL& operator = (const cl_mem& rhs)
    {
        Buffer::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    BufferRenderGL(const BufferRenderGL& buf) : Buffer(buf) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    BufferRenderGL& operator = (const BufferRenderGL &buf)
    {
        Buffer::operator=(buf);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    BufferRenderGL(BufferRenderGL&& buf) CL_HPP_NOEXCEPT_ : Buffer(std::move(buf)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    BufferRenderGL& operator = (BufferRenderGL &&buf)
    {
        Buffer::operator=(std::move(buf));
        return *this;
    }

    //! \brief Wrapper for clGetGLObjectInfo().
    cl_int getObjectInfo(
        cl_gl_object_type *type,
        cl_GLuint * gl_object_name)
    {
        return detail::errHandler(
            ::clGetGLObjectInfo(object_,type,gl_object_name),
            __GET_GL_OBJECT_INFO_ERR);
    }
};

/*! \brief C++ base class for Image Memory objects.
 *
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 */
class Image : public Memory
{
protected:
    //! \brief Default constructor - initializes to NULL.
    Image() : Memory() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit Image(const cl_mem& image) : Memory(image) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    Image& operator = (const cl_mem& rhs)
    {
        Memory::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image(const Image& img) : Memory(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image& operator = (const Image &img)
    {
        Memory::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image(Image&& img) CL_HPP_NOEXCEPT_ : Memory(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image& operator = (Image &&img)
    {
        Memory::operator=(std::move(img));
        return *this;
    }


public:
    //! \brief Wrapper for clGetImageInfo().
    template <typename T>
    cl_int getImageInfo(cl_image_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetImageInfo, object_, name, param),
            __GET_IMAGE_INFO_ERR);
    }
    
    //! \brief Wrapper for clGetImageInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_image_info, name>::param_type
    getImageInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_image_info, name>::param_type param;
        cl_int result = getImageInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }
};

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
/*! \brief Class interface for 1D Image Memory objects.
 *
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 */
class Image1D : public Image
{
public:
    /*! \brief Constructs a 1D Image in a specified context.
     *
     *  Wraps clCreateImage().
     */
    Image1D(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        void* host_ptr = NULL,
        cl_int* err = NULL)
    {
        cl_int error;
        cl_image_desc desc =
        {
            CL_MEM_OBJECT_IMAGE1D,
            width,
            0, 0, 0, 0, 0, 0, 0, 0
        };
        object_ = ::clCreateImage(
            context(), 
            flags, 
            &format, 
            &desc, 
            host_ptr, 
            &error);

        detail::errHandler(error, __CREATE_IMAGE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    //! \brief Default constructor - initializes to NULL.
    Image1D() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit Image1D(const cl_mem& image1D) : Image(image1D) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    Image1D& operator = (const cl_mem& rhs)
    {
        Image::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image1D(const Image1D& img) : Image(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image1D& operator = (const Image1D &img)
    {
        Image::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image1D(Image1D&& img) CL_HPP_NOEXCEPT_ : Image(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image1D& operator = (Image1D &&img)
    {
        Image::operator=(std::move(img));
        return *this;
    }

};

/*! \class Image1DBuffer
 * \brief Image interface for 1D buffer images.
 */
class Image1DBuffer : public Image
{
public:
    Image1DBuffer(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        const Buffer &buffer,
        cl_int* err = NULL)
    {
        cl_int error;
        cl_image_desc desc =
        {
            CL_MEM_OBJECT_IMAGE1D_BUFFER,
            width,
            0, 0, 0, 0, 0, 0, 0,
            buffer()
        };
        object_ = ::clCreateImage(
            context(), 
            flags, 
            &format, 
            &desc, 
            NULL, 
            &error);

        detail::errHandler(error, __CREATE_IMAGE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    Image1DBuffer() { }

    explicit Image1DBuffer(const cl_mem& image1D) : Image(image1D) { }

    Image1DBuffer& operator = (const cl_mem& rhs)
    {
        Image::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image1DBuffer(const Image1DBuffer& img) : Image(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image1DBuffer& operator = (const Image1DBuffer &img)
    {
        Image::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image1DBuffer(Image1DBuffer&& img) CL_HPP_NOEXCEPT_ : Image(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image1DBuffer& operator = (Image1DBuffer &&img)
    {
        Image::operator=(std::move(img));
        return *this;
    }

};

/*! \class Image1DArray
 * \brief Image interface for arrays of 1D images.
 */
class Image1DArray : public Image
{
public:
    Image1DArray(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t arraySize,
        ::size_t width,
        ::size_t rowPitch,
        void* host_ptr = NULL,
        cl_int* err = NULL)
    {
        cl_int error;
        cl_image_desc desc =
        {
            CL_MEM_OBJECT_IMAGE1D_ARRAY,
            width,
            0, 0,  // height, depth (unused)
            arraySize,
            rowPitch,
            0, 0, 0, 0
        };
        object_ = ::clCreateImage(
            context(), 
            flags, 
            &format, 
            &desc, 
            host_ptr, 
            &error);

        detail::errHandler(error, __CREATE_IMAGE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    Image1DArray() { }

    explicit Image1DArray(const cl_mem& imageArray) : Image(imageArray) { }

    Image1DArray& operator = (const cl_mem& rhs)
    {
        Image::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image1DArray(const Image1DArray& img) : Image(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image1DArray& operator = (const Image1DArray &img)
    {
        Image::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image1DArray(Image1DArray&& img) CL_HPP_NOEXCEPT_ : Image(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image1DArray& operator = (Image1DArray &&img)
    {
        Image::operator=(std::move(img));
        return *this;
    }

};
#endif // #if CL_HPP_TARGET_OPENCL_VERSION >= 120


/*! \brief Class interface for 2D Image Memory objects.
 *
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 */
class Image2D : public Image
{
public:
    /*! \brief Constructs a 1D Image in a specified context.
     *
     *  Wraps clCreateImage().
     */
    Image2D(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        ::size_t height,
        ::size_t row_pitch = 0,
        void* host_ptr = NULL,
        cl_int* err = NULL)
    {
        cl_int error;
        bool useCreateImage;

#if CL_HPP_TARGET_OPENCL_VERSION >= 120 && CL_HPP_MINIMUM_OPENCL_VERSION < 120
        // Run-time decision based on the actual platform
        {
            cl_uint version = detail::getContextPlatformVersion(context());
            useCreateImage = (version >= 0x10002); // OpenCL 1.2 or above
        }
#elif CL_HPP_TARGET_OPENCL_VERSION >= 120
        useCreateImage = true;
#else
        useCreateImage = false;
#endif

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
        if (useCreateImage)
        {
            cl_image_desc desc =
            {
                CL_MEM_OBJECT_IMAGE2D,
                width,
                height,
                0, 0, // depth, array size (unused)
                row_pitch,
                0, 0, 0, 0
            };
            object_ = ::clCreateImage(
                context(),
                flags,
                &format,
                &desc,
                host_ptr,
                &error);

            detail::errHandler(error, __CREATE_IMAGE_ERR);
            if (err != NULL) {
                *err = error;
            }
        }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
#if CL_HPP_MINIMUM_OPENCL_VERSION < 120
        if (!useCreateImage)
        {
            object_ = ::clCreateImage2D(
                context(), flags,&format, width, height, row_pitch, host_ptr, &error);

            detail::errHandler(error, __CREATE_IMAGE2D_ERR);
            if (err != NULL) {
                *err = error;
            }
        }
#endif // CL_HPP_MINIMUM_OPENCL_VERSION < 120
    }

    //! \brief Default constructor - initializes to NULL.
    Image2D() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit Image2D(const cl_mem& image2D) : Image(image2D) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    Image2D& operator = (const cl_mem& rhs)
    {
        Image::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image2D(const Image2D& img) : Image(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image2D& operator = (const Image2D &img)
    {
        Image::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image2D(Image2D&& img) CL_HPP_NOEXCEPT_ : Image(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image2D& operator = (Image2D &&img)
    {
        Image::operator=(std::move(img));
        return *this;
    }

};


#if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)
/*! \brief Class interface for GL 2D Image Memory objects.
 *
 *  This is provided to facilitate interoperability with OpenGL.
 * 
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 *  \note Deprecated for OpenCL 1.2. Please use ImageGL instead.
 */
class CL_EXT_PREFIX__VERSION_1_1_DEPRECATED Image2DGL CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED : public Image2D
{
public:
    /*! \brief Constructs an Image2DGL in a specified context, from a given
     *         GL Texture.
     *
     *  Wraps clCreateFromGLTexture2D().
     */
    Image2DGL(
        const Context& context,
        cl_mem_flags flags,
        cl_GLenum target,
        cl_GLint  miplevel,
        cl_GLuint texobj,
        cl_int * err = NULL)
    {
        cl_int error;
        object_ = ::clCreateFromGLTexture2D(
            context(),
            flags,
            target,
            miplevel,
            texobj,
            &error);

        detail::errHandler(error, __CREATE_GL_TEXTURE_2D_ERR);
        if (err != NULL) {
            *err = error;
        }

    }
    
    //! \brief Default constructor - initializes to NULL.
    Image2DGL() : Image2D() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit Image2DGL(const cl_mem& image) : Image2D(image) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *c
     *  See Memory for further details.
     */
    Image2DGL& operator = (const cl_mem& rhs)
    {
        Image2D::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image2DGL(const Image2DGL& img) : Image2D(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image2DGL& operator = (const Image2DGL &img)
    {
        Image2D::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image2DGL(Image2DGL&& img) CL_HPP_NOEXCEPT_ : Image2D(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image2DGL& operator = (Image2DGL &&img)
    {
        Image2D::operator=(std::move(img));
        return *this;
    }

};
#endif // CL_USE_DEPRECATED_OPENCL_1_1_APIS

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
/*! \class Image2DArray
 * \brief Image interface for arrays of 2D images.
 */
class Image2DArray : public Image
{
public:
    Image2DArray(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t arraySize,
        ::size_t width,
        ::size_t height,
        ::size_t rowPitch,
        ::size_t slicePitch,
        void* host_ptr = NULL,
        cl_int* err = NULL)
    {
        cl_int error;
        cl_image_desc desc =
        {
            CL_MEM_OBJECT_IMAGE2D_ARRAY,
            width,
            height,
            0,       // depth (unused)
            arraySize,
            rowPitch,
            slicePitch,
            0, 0, 0
        };
        object_ = ::clCreateImage(
            context(), 
            flags, 
            &format, 
            &desc, 
            host_ptr, 
            &error);

        detail::errHandler(error, __CREATE_IMAGE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    Image2DArray() { }

    explicit Image2DArray(const cl_mem& imageArray) : Image(imageArray) { }

    Image2DArray& operator = (const cl_mem& rhs)
    {
        Image::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image2DArray(const Image2DArray& img) : Image(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image2DArray& operator = (const Image2DArray &img)
    {
        Image::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image2DArray(Image2DArray&& img) CL_HPP_NOEXCEPT_ : Image(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image2DArray& operator = (Image2DArray &&img)
    {
        Image::operator=(std::move(img));
        return *this;
    }
};
#endif // #if CL_HPP_TARGET_OPENCL_VERSION >= 120

/*! \brief Class interface for 3D Image Memory objects.
 *
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 */
class Image3D : public Image
{
public:
    /*! \brief Constructs a 3D Image in a specified context.
     *
     *  Wraps clCreateImage().
     */
    Image3D(
        const Context& context,
        cl_mem_flags flags,
        ImageFormat format,
        ::size_t width,
        ::size_t height,
        ::size_t depth,
        ::size_t row_pitch = 0,
        ::size_t slice_pitch = 0,
        void* host_ptr = NULL,
        cl_int* err = NULL)
    {
        cl_int error;
        bool useCreateImage;

#if CL_HPP_TARGET_OPENCL_VERSION >= 120 && CL_HPP_MINIMUM_OPENCL_VERSION < 120
        // Run-time decision based on the actual platform
        {
            cl_uint version = detail::getContextPlatformVersion(context());
            useCreateImage = (version >= 0x10002); // OpenCL 1.2 or above
        }
#elif CL_HPP_TARGET_OPENCL_VERSION >= 120
        useCreateImage = true;
#else
        useCreateImage = false;
#endif

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
        if (useCreateImage)
        {
            cl_image_desc desc =
            {
                CL_MEM_OBJECT_IMAGE3D,
                width,
                height,
                depth,
                0,      // array size (unused)
                row_pitch,
                slice_pitch,
                0, 0, 0
            };
            object_ = ::clCreateImage(
                context(), 
                flags, 
                &format, 
                &desc, 
                host_ptr, 
                &error);

            detail::errHandler(error, __CREATE_IMAGE_ERR);
            if (err != NULL) {
                *err = error;
            }
        }
#endif  // CL_HPP_TARGET_OPENCL_VERSION >= 120
#if CL_HPP_MINIMUM_OPENCL_VERSION < 120
        if (!useCreateImage)
        {
            object_ = ::clCreateImage3D(
                context(), flags, &format, width, height, depth, row_pitch,
                slice_pitch, host_ptr, &error);

            detail::errHandler(error, __CREATE_IMAGE3D_ERR);
            if (err != NULL) {
                *err = error;
            }
        }
#endif // CL_HPP_MINIMUM_OPENCL_VERSION < 120
    }

    //! \brief Default constructor - initializes to NULL.
    Image3D() : Image() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit Image3D(const cl_mem& image3D) : Image(image3D) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    Image3D& operator = (const cl_mem& rhs)
    {
        Image::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image3D(const Image3D& img) : Image(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image3D& operator = (const Image3D &img)
    {
        Image::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image3D(Image3D&& img) CL_HPP_NOEXCEPT_ : Image(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image3D& operator = (Image3D &&img)
    {
        Image::operator=(std::move(img));
        return *this;
    }
};

#if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)
/*! \brief Class interface for GL 3D Image Memory objects.
 *
 *  This is provided to facilitate interoperability with OpenGL.
 * 
 *  See Memory for details about copy semantics, etc.
 * 
 *  \see Memory
 */
class Image3DGL : public Image3D
{
public:
    /*! \brief Constructs an Image3DGL in a specified context, from a given
     *         GL Texture.
     *
     *  Wraps clCreateFromGLTexture3D().
     */
    Image3DGL(
        const Context& context,
        cl_mem_flags flags,
        cl_GLenum target,
        cl_GLint  miplevel,
        cl_GLuint texobj,
        cl_int * err = NULL)
    {
        cl_int error;
        object_ = ::clCreateFromGLTexture3D(
            context(),
            flags,
            target,
            miplevel,
            texobj,
            &error);

        detail::errHandler(error, __CREATE_GL_TEXTURE_3D_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    //! \brief Default constructor - initializes to NULL.
    Image3DGL() : Image3D() { }

    /*! \brief Constructor from cl_mem - takes ownership.
     *
     *  See Memory for further details.
     */
    explicit Image3DGL(const cl_mem& image) : Image3D(image) { }

    /*! \brief Assignment from cl_mem - performs shallow copy.
     *
     *  See Memory for further details.
     */
    Image3DGL& operator = (const cl_mem& rhs)
    {
        Image3D::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image3DGL(const Image3DGL& img) : Image3D(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Image3DGL& operator = (const Image3DGL &img)
    {
        Image3D::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image3DGL(Image3DGL&& img) CL_HPP_NOEXCEPT_ : Image3D(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Image3DGL& operator = (Image3DGL &&img)
    {
        Image3D::operator=(std::move(img));
        return *this;
    }
};
#endif // CL_USE_DEPRECATED_OPENCL_1_1_APIS

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
/*! \class ImageGL
 * \brief general image interface for GL interop.
 * We abstract the 2D and 3D GL images into a single instance here
 * that wraps all GL sourced images on the grounds that setup information
 * was performed by OpenCL anyway.
 */
class ImageGL : public Image
{
public:
    ImageGL(
        const Context& context,
        cl_mem_flags flags,
        cl_GLenum target,
        cl_GLint  miplevel,
        cl_GLuint texobj,
        cl_int * err = NULL)
    {
        cl_int error;
        object_ = ::clCreateFromGLTexture(
            context(), 
            flags, 
            target,
            miplevel,
            texobj,
            &error);

        detail::errHandler(error, __CREATE_GL_TEXTURE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    ImageGL() : Image() { }

    explicit ImageGL(const cl_mem& image) : Image(image) { }

    ImageGL& operator = (const cl_mem& rhs)
    {
        Image::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    ImageGL(const ImageGL& img) : Image(img) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    ImageGL& operator = (const ImageGL &img)
    {
        Image::operator=(img);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    ImageGL(ImageGL&& img) CL_HPP_NOEXCEPT_ : Image(std::move(img)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    ImageGL& operator = (ImageGL &&img)
    {
        Image::operator=(std::move(img));
        return *this;
    }
};
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

/*! \brief Class interface for cl_sampler.
 *
 *  \note Copies of these objects are shallow, meaning that the copy will refer
 *        to the same underlying cl_sampler as the original.  For details, see
 *        clRetainSampler() and clReleaseSampler().
 *
 *  \see cl_sampler 
 */
class Sampler : public detail::Wrapper<cl_sampler>
{
public:
    //! \brief Default constructor - initializes to NULL.
    Sampler() { }

    /*! \brief Constructs a Sampler in a specified context.
     *
     *  Wraps clCreateSampler().
     */
    Sampler(
        const Context& context,
        cl_bool normalized_coords,
        cl_addressing_mode addressing_mode,
        cl_filter_mode filter_mode,
        cl_int* err = NULL)
    {
        cl_int error;
        object_ = ::clCreateSampler(
            context(), 
            normalized_coords,
            addressing_mode,
            filter_mode,
            &error);

        detail::errHandler(error, __CREATE_SAMPLER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    /*! \brief Constructor from cl_sampler - takes ownership.
     * 
     *  This effectively transfers ownership of a refcount on the cl_sampler
     *  into the new Sampler object.
     */
    Sampler(const cl_sampler& sampler) : detail::Wrapper<cl_type>(sampler) { }

    /*! \brief Assignment operator from cl_sampler - takes ownership.
     *
     *  This effectively transfers ownership of a refcount on the rhs and calls
     *  clReleaseSampler() on the value previously held by this instance.
     */
    Sampler& operator = (const cl_sampler& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Sampler(const Sampler& sam) : detail::Wrapper<cl_type>(sam) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Sampler& operator = (const Sampler &sam)
    {
        detail::Wrapper<cl_type>::operator=(sam);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Sampler(Sampler&& sam) CL_HPP_NOEXCEPT_ : detail::Wrapper<cl_type>(std::move(sam)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Sampler& operator = (Sampler &&sam)
    {
        detail::Wrapper<cl_type>::operator=(std::move(sam));
        return *this;
    }

    //! \brief Wrapper for clGetSamplerInfo().
    template <typename T>
    cl_int getInfo(cl_sampler_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetSamplerInfo, object_, name, param),
            __GET_SAMPLER_INFO_ERR);
    }

    //! \brief Wrapper for clGetSamplerInfo() that returns by value.
    template <cl_int name> typename
    detail::param_traits<detail::cl_sampler_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_sampler_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }
};

class Program;
class CommandQueue;
class Kernel;

//! \brief Class interface for specifying NDRange values.
class NDRange
{
private:
    size_t<3> sizes_;
    cl_uint dimensions_;

public:
    //! \brief Default constructor - resulting range has zero dimensions.
    NDRange()
        : dimensions_(0)
    { }

    //! \brief Constructs one-dimensional range.
    NDRange(::size_t size0)
        : dimensions_(1)
    {
        sizes_[0] = size0;
    }

    //! \brief Constructs two-dimensional range.
    NDRange(::size_t size0, ::size_t size1)
        : dimensions_(2)
    {
        sizes_[0] = size0;
        sizes_[1] = size1;
    }

    //! \brief Constructs three-dimensional range.
    NDRange(::size_t size0, ::size_t size1, ::size_t size2)
        : dimensions_(3)
    {
        sizes_[0] = size0;
        sizes_[1] = size1;
        sizes_[2] = size2;
    }

    /*! \brief Conversion operator to const ::size_t *.
     *  
     *  \returns a pointer to the size of the first dimension.
     */
    operator const ::size_t*() const { 
        return (const ::size_t*) sizes_; 
    }

    //! \brief Queries the number of dimensions in the range.
    ::size_t dimensions() const { return dimensions_; }
};

//! \brief A zero-dimensional range.
static const NDRange NullRange;

//! \brief Local address wrapper for use with Kernel::setArg
struct LocalSpaceArg
{
    ::size_t size_;
};

namespace detail {

template <typename T>
struct KernelArgumentHandler
{
    static ::size_t size(const T&) { return sizeof(T); }
    static const T* ptr(const T& value) { return &value; }
};

template <>
struct KernelArgumentHandler<LocalSpaceArg>
{
    static ::size_t size(const LocalSpaceArg& value) { return value.size_; }
    static const void* ptr(const LocalSpaceArg&) { return NULL; }
};

} 
//! \endcond

/*! Local
 * \brief Helper function for generating LocalSpaceArg objects.
 */
inline LocalSpaceArg
Local(::size_t size)
{
    LocalSpaceArg ret = { size };
    return ret;
}

//class KernelFunctor;

/*! \brief Class interface for cl_kernel.
 *
 *  \note Copies of these objects are shallow, meaning that the copy will refer
 *        to the same underlying cl_kernel as the original.  For details, see
 *        clRetainKernel() and clReleaseKernel().
 *
 *  \see cl_kernel
 */
class Kernel : public detail::Wrapper<cl_kernel>
{
public:
    inline Kernel(const Program& program, const char* name, cl_int* err = NULL);

    //! \brief Default constructor - initializes to NULL.
    Kernel() { }

    /*! \brief Constructor from cl_kernel - takes ownership.
     * 
     *  This effectively transfers ownership of a refcount on the cl_kernel
     *  into the new Kernel object.
     */
    explicit Kernel(const cl_kernel& kernel) : detail::Wrapper<cl_type>(kernel) { }

    /*! \brief Assignment operator from cl_kernel - takes ownership.
     *
     *  This effectively transfers ownership of a refcount on the rhs and calls
     *  clReleaseKernel() on the value previously held by this instance.
     */
    Kernel& operator = (const cl_kernel& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Kernel(const Kernel& kernel) : detail::Wrapper<cl_type>(kernel) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Kernel& operator = (const Kernel &kernel)
    {
        detail::Wrapper<cl_type>::operator=(kernel);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Kernel(Kernel&& kernel) CL_HPP_NOEXCEPT_ : detail::Wrapper<cl_type>(std::move(kernel)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Kernel& operator = (Kernel &&kernel)
    {
        detail::Wrapper<cl_type>::operator=(std::move(kernel));
        return *this;
    }

    template <typename T>
    cl_int getInfo(cl_kernel_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetKernelInfo, object_, name, param),
            __GET_KERNEL_INFO_ERR);
    }

    template <cl_int name> typename
    detail::param_traits<detail::cl_kernel_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_kernel_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    template <typename T>
    cl_int getArgInfo(cl_uint argIndex, cl_kernel_arg_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetKernelArgInfo, object_, argIndex, name, param),
            __GET_KERNEL_ARG_INFO_ERR);
    }

    template <cl_int name> typename
    detail::param_traits<detail::cl_kernel_arg_info, name>::param_type
    getArgInfo(cl_uint argIndex, cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_kernel_arg_info, name>::param_type param;
        cl_int result = getArgInfo(argIndex, name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

    template <typename T>
    cl_int getWorkGroupInfo(
        const Device& device, cl_kernel_work_group_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(
                &::clGetKernelWorkGroupInfo, object_, device(), name, param),
                __GET_KERNEL_WORK_GROUP_INFO_ERR);
    }

    template <cl_int name> typename
    detail::param_traits<detail::cl_kernel_work_group_info, name>::param_type
        getWorkGroupInfo(const Device& device, cl_int* err = NULL) const
    {
        typename detail::param_traits<
        detail::cl_kernel_work_group_info, name>::param_type param;
        cl_int result = getWorkGroupInfo(device, name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    template <typename T>
    cl_int setArg(cl_uint index, const T &value)
    {
        return detail::errHandler(
            ::clSetKernelArg(
                object_,
                index,
                detail::KernelArgumentHandler<T>::size(value),
                detail::KernelArgumentHandler<T>::ptr(value)),
            __SET_KERNEL_ARGS_ERR);
    }

    cl_int setArg(cl_uint index, ::size_t size, const void* argPtr)
    {
        return detail::errHandler(
            ::clSetKernelArg(object_, index, size, argPtr),
            __SET_KERNEL_ARGS_ERR);
    }
};

/*! \class Program
 * \brief Program interface that implements cl_program.
 */
class Program : public detail::Wrapper<cl_program>
{
public:
    typedef vector_class<std::pair<const void*, ::size_t> > Binaries;
    typedef vector_class<std::pair<const char*, ::size_t> > Sources;

    Program(
        const string_class& source,
        bool build = false,
        cl_int* err = NULL)
    {
        cl_int error;

        const char * strings = source.c_str();
        const ::size_t length  = source.size();

        Context context = Context::getDefault(err);

        object_ = ::clCreateProgramWithSource(
            context(), (cl_uint)1, &strings, &length, &error);

        detail::errHandler(error, __CREATE_PROGRAM_WITH_SOURCE_ERR);

        if (error == CL_SUCCESS && build) {

            error = ::clBuildProgram(
                object_,
                0,
                NULL,
                "",
                NULL,
                NULL);

            detail::errHandler(error, __BUILD_PROGRAM_ERR);
        }

        if (err != NULL) {
            *err = error;
        }
    }

    Program(
        const Context& context,
        const string_class& source,
        bool build = false,
        cl_int* err = NULL)
    {
        cl_int error;

        const char * strings = source.c_str();
        const ::size_t length  = source.size();

        object_ = ::clCreateProgramWithSource(
            context(), (cl_uint)1, &strings, &length, &error);

        detail::errHandler(error, __CREATE_PROGRAM_WITH_SOURCE_ERR);

        if (error == CL_SUCCESS && build) {

            error = ::clBuildProgram(
                object_,
                0,
                NULL,
                "",
                NULL,
                NULL);

            detail::errHandler(error, __BUILD_PROGRAM_ERR);
        }

        if (err != NULL) {
            *err = error;
        }
    }

    Program(
        const Context& context,
        const Sources& sources,
        cl_int* err = NULL)
    {
        cl_int error;

        const ::size_t n = (::size_t)sources.size();
        ::size_t* lengths = (::size_t*) alloca(n * sizeof(::size_t));
        const char** strings = (const char**) alloca(n * sizeof(const char*));

        for (::size_t i = 0; i < n; ++i) {
            strings[i] = sources[(int)i].first;
            lengths[i] = sources[(int)i].second;
        }

        object_ = ::clCreateProgramWithSource(
            context(), (cl_uint)n, strings, lengths, &error);

        detail::errHandler(error, __CREATE_PROGRAM_WITH_SOURCE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    /**
     * Construct a program object from a list of devices and a per-device list of binaries.
     * \param context A valid OpenCL context in which to construct the program.
     * \param devices A vector of OpenCL device objects for which the program will be created.
     * \param binaries A vector of pairs of a pointer to a binary object and its length.
     * \param binaryStatus An optional vector that on completion will be resized to
     *   match the size of binaries and filled with values to specify if each binary
     *   was successfully loaded.
     *   Set to CL_SUCCESS if the binary was successfully loaded.
     *   Set to CL_INVALID_VALUE if the length is 0 or the binary pointer is NULL.
     *   Set to CL_INVALID_BINARY if the binary provided is not valid for the matching device.
     * \param err if non-NULL will be set to CL_SUCCESS on successful operation or one of the following errors:
     *   CL_INVALID_CONTEXT if context is not a valid context.
     *   CL_INVALID_VALUE if the length of devices is zero; or if the length of binaries does not match the length of devices; 
     *     or if any entry in binaries is NULL or has length 0.
     *   CL_INVALID_DEVICE if OpenCL devices listed in devices are not in the list of devices associated with context.
     *   CL_INVALID_BINARY if an invalid program binary was encountered for any device. binaryStatus will return specific status for each device.
     *   CL_OUT_OF_HOST_MEMORY if there is a failure to allocate resources required by the OpenCL implementation on the host.
     */
    Program(
        const Context& context,
        const vector_class<Device>& devices,
        const Binaries& binaries,
        vector_class<cl_int>* binaryStatus = NULL,
        cl_int* err = NULL)
    {
        cl_int error;
        
        const ::size_t numDevices = devices.size();
        
        // Catch size mismatch early and return
        if(binaries.size() != numDevices) {
            error = CL_INVALID_VALUE;
            detail::errHandler(error, __CREATE_PROGRAM_WITH_BINARY_ERR);
            if (err != NULL) {
                *err = error;
            }
            return;
        }

        ::size_t* lengths = (::size_t*) alloca(numDevices * sizeof(::size_t));
        const unsigned char** images = (const unsigned char**) alloca(numDevices * sizeof(const unsigned char**));

        for (::size_t i = 0; i < numDevices; ++i) {
            images[i] = (const unsigned char*)binaries[i].first;
            lengths[i] = binaries[(int)i].second;
        }

        cl_device_id* deviceIDs = (cl_device_id*) alloca(numDevices * sizeof(cl_device_id));
        for( ::size_t deviceIndex = 0; deviceIndex < numDevices; ++deviceIndex ) {
            deviceIDs[deviceIndex] = (devices[deviceIndex])();
        }

        if(binaryStatus) {
            binaryStatus->resize(numDevices);
        }
        
        object_ = ::clCreateProgramWithBinary(
            context(), (cl_uint) devices.size(),
            deviceIDs,
            lengths, images, (binaryStatus != NULL && numDevices > 0)
               ? &binaryStatus->front()
               : NULL, &error);

        detail::errHandler(error, __CREATE_PROGRAM_WITH_BINARY_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    /**
     * Create program using builtin kernels.
     * \param kernelNames Semi-colon separated list of builtin kernel names
     */
    Program(
        const Context& context,
        const vector_class<Device>& devices,
        const string_class& kernelNames,
        cl_int* err = NULL)
    {
        cl_int error;


        ::size_t numDevices = devices.size();
        cl_device_id* deviceIDs = (cl_device_id*) alloca(numDevices * sizeof(cl_device_id));
        for( ::size_t deviceIndex = 0; deviceIndex < numDevices; ++deviceIndex ) {
            deviceIDs[deviceIndex] = (devices[deviceIndex])();
        }
        
        object_ = ::clCreateProgramWithBuiltInKernels(
            context(), 
            (cl_uint) devices.size(),
            deviceIDs,
            kernelNames.c_str(), 
            &error);

        detail::errHandler(error, __CREATE_PROGRAM_WITH_BUILT_IN_KERNELS_ERR);
        if (err != NULL) {
            *err = error;
        }
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

    Program() { }

    explicit Program(const cl_program& program) : detail::Wrapper<cl_type>(program) { }

    Program& operator = (const cl_program& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Program(const Program& program) : detail::Wrapper<cl_type>(program) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    Program& operator = (const Program &program)
    {
        detail::Wrapper<cl_type>::operator=(program);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Program(Program&& program) CL_HPP_NOEXCEPT_ : detail::Wrapper<cl_type>(std::move(program)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    Program& operator = (Program &&program)
    {
        detail::Wrapper<cl_type>::operator=(std::move(program));
        return *this;
    }

    cl_int build(
        const vector_class<Device>& devices,
        const char* options = NULL,
        void (CL_CALLBACK * notifyFptr)(cl_program, void *) = NULL,
        void* data = NULL) const
    {
        ::size_t numDevices = devices.size();
        cl_device_id* deviceIDs = (cl_device_id*) alloca(numDevices * sizeof(cl_device_id));
        for( ::size_t deviceIndex = 0; deviceIndex < numDevices; ++deviceIndex ) {
            deviceIDs[deviceIndex] = (devices[deviceIndex])();
        }

        return detail::errHandler(
            ::clBuildProgram(
                object_,
                (cl_uint)
                devices.size(),
                deviceIDs,
                options,
                notifyFptr,
                data),
                __BUILD_PROGRAM_ERR);
    }

    cl_int build(
        const char* options = NULL,
        void (CL_CALLBACK * notifyFptr)(cl_program, void *) = NULL,
        void* data = NULL) const
    {
        return detail::errHandler(
            ::clBuildProgram(
                object_,
                0,
                NULL,
                options,
                notifyFptr,
                data),
                __BUILD_PROGRAM_ERR);
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    cl_int compile(
        const char* options = NULL,
        void (CL_CALLBACK * notifyFptr)(cl_program, void *) = NULL,
        void* data = NULL) const
    {
        return detail::errHandler(
            ::clCompileProgram(
                object_,
                0,
                NULL,
                options,
                0,
                NULL,
                NULL,
                notifyFptr,
                data),
                __COMPILE_PROGRAM_ERR);
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

    template <typename T>
    cl_int getInfo(cl_program_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(&::clGetProgramInfo, object_, name, param),
            __GET_PROGRAM_INFO_ERR);
    }

    template <cl_int name> typename
    detail::param_traits<detail::cl_program_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_program_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    template <typename T>
    cl_int getBuildInfo(
        const Device& device, cl_program_build_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(
                &::clGetProgramBuildInfo, object_, device(), name, param),
                __GET_PROGRAM_BUILD_INFO_ERR);
    }

    template <cl_int name> typename
    detail::param_traits<detail::cl_program_build_info, name>::param_type
    getBuildInfo(const Device& device, cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_program_build_info, name>::param_type param;
        cl_int result = getBuildInfo(device, name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    cl_int createKernels(vector_class<Kernel>* kernels)
    {
        cl_uint numKernels;
        cl_int err = ::clCreateKernelsInProgram(object_, 0, NULL, &numKernels);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __CREATE_KERNELS_IN_PROGRAM_ERR);
        }

        Kernel* value = (Kernel*) alloca(numKernels * sizeof(Kernel));
        err = ::clCreateKernelsInProgram(
            object_, numKernels, (cl_kernel*) value, NULL);
        if (err != CL_SUCCESS) {
            return detail::errHandler(err, __CREATE_KERNELS_IN_PROGRAM_ERR);
        }

        kernels->assign(&value[0], &value[numKernels]);
        return CL_SUCCESS;
    }
};

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
inline Program linkProgram(
    Program input1,
    Program input2,
    const char* options = NULL,
    void (CL_CALLBACK * notifyFptr)(cl_program, void *) = NULL,
    void* data = NULL,
    cl_int* err = NULL) 
{
    cl_int error_local = CL_SUCCESS;

    cl_program programs[2] = { input1(), input2() };

    Context ctx = input1.getInfo<CL_PROGRAM_CONTEXT>(&error_local);
    if(error_local!=CL_SUCCESS) {
        detail::errHandler(error_local, __LINK_PROGRAM_ERR);
    }

    cl_program prog = ::clLinkProgram(
        ctx(),
        0,
        NULL,
        options,
        2,
        programs,
        notifyFptr,
        data,
        &error_local);

    detail::errHandler(error_local,__COMPILE_PROGRAM_ERR);
    if (err != NULL) {
        *err = error_local;
    }

    return Program(prog);
}

inline Program linkProgram(
    vector_class<Program> inputPrograms,
    const char* options = NULL,
    void (CL_CALLBACK * notifyFptr)(cl_program, void *) = NULL,
    void* data = NULL,
    cl_int* err = NULL) 
{
    cl_int error_local = CL_SUCCESS;

    cl_program * programs = (cl_program*) alloca(inputPrograms.size() * sizeof(cl_program));

    if (programs != NULL) {
        for (unsigned int i = 0; i < inputPrograms.size(); i++) {
          programs[i] = inputPrograms[i]();
        }
    } 

    Context ctx;
    if(inputPrograms.size() > 0) {
        ctx = inputPrograms[0].getInfo<CL_PROGRAM_CONTEXT>(&error_local);
        if(error_local!=CL_SUCCESS) {
            detail::errHandler(error_local, __LINK_PROGRAM_ERR);
        }
	}
	cl_program prog = ::clLinkProgram(
		ctx(),
        0,
        NULL,
        options,
        (cl_uint)inputPrograms.size(),
        programs,
        notifyFptr,
        data,
        &error_local);

    detail::errHandler(error_local,__COMPILE_PROGRAM_ERR);
    if (err != NULL) {
        *err = error_local;
    }

    return Program(prog);
}
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

template<>
inline vector_class<char *> cl::Program::getInfo<CL_PROGRAM_BINARIES>(cl_int* err) const
{
    vector_class< ::size_t> sizes = getInfo<CL_PROGRAM_BINARY_SIZES>();
    vector_class<char *> binaries;
    for (vector_class< ::size_t>::iterator s = sizes.begin(); s != sizes.end(); ++s) 
    {
        char *ptr = NULL;
        if (*s != 0) 
            ptr = new char[*s];
        binaries.push_back(ptr);
    }
    
    cl_int result = getInfo(CL_PROGRAM_BINARIES, &binaries);
    if (err != NULL) {
        *err = result;
    }
    return binaries;
}

inline Kernel::Kernel(const Program& program, const char* name, cl_int* err)
{
    cl_int error;

    object_ = ::clCreateKernel(program(), name, &error);
    detail::errHandler(error, __CREATE_KERNEL_ERR);

    if (err != NULL) {
        *err = error;
    }

}

/*! \class CommandQueue
 * \brief CommandQueue interface for cl_command_queue.
 */
class CommandQueue : public detail::Wrapper<cl_command_queue>
{
private:
    static std::once_flag default_initialized_;
    static CommandQueue default_;
    static cl_int default_error_;

    /*! \brief Create the default command queue returned by @ref getDefault.
     *
     * It sets default_error_ to indicate success or failure. It does not throw
     * @c cl::Error.
     */
    static void makeDefault()
    {
        /* We don't want to throw an error from this function, so we have to
         * catch and set the error flag.
         */
#if defined(CL_HPP_ENABLE_EXCEPTIONS)
        try
#endif
        {
            int error;
            Context context = Context::getDefault(&error);

            if (error != CL_SUCCESS) {
                default_error_ = error;
            }
            else {
                Device device = context.getInfo<CL_CONTEXT_DEVICES>()[0];
                default_ = CommandQueue(context, device, 0, &default_error_);
            }
        }
#if defined(CL_HPP_ENABLE_EXCEPTIONS)
        catch (cl::Error &e) {
            default_error_ = e.err();
        }
#endif
    }

public:
   CommandQueue(
        cl_command_queue_properties properties,
        cl_int* err = NULL)
    {
        cl_int error;

        Context context = Context::getDefault(&error);
        detail::errHandler(error, __CREATE_COMMAND_QUEUE_ERR);

        if (error != CL_SUCCESS) {
            if (err != NULL) {
                *err = error;
            }
        }
        else {
            Device device = context.getInfo<CL_CONTEXT_DEVICES>()[0];

            object_ = ::clCreateCommandQueue(
                context(), device(), properties, &error);

            detail::errHandler(error, __CREATE_COMMAND_QUEUE_ERR);
            if (err != NULL) {
                *err = error;
            }
        }
    }
    /*!
    * \brief Constructs a CommandQueue for an implementation defined device in the given context
    */
    explicit CommandQueue(
        const Context& context,
        cl_command_queue_properties properties = 0,
        cl_int* err = NULL)
    {
        cl_int error;
        vector_class<cl::Device> devices;
        error = context.getInfo(CL_CONTEXT_DEVICES, &devices);

        detail::errHandler(error, __CREATE_COMMAND_QUEUE_ERR);

        if (error != CL_SUCCESS)
        {
            if (err != NULL) {
                *err = error;
            }
            return;
        }

        object_ = ::clCreateCommandQueue(context(), devices[0](), properties, &error);

        detail::errHandler(error, __CREATE_COMMAND_QUEUE_ERR);

        if (err != NULL) {
            *err = error;
        }

    }

    CommandQueue(
        const Context& context,
        const Device& device,
        cl_command_queue_properties properties = 0,
        cl_int* err = NULL)
    {
        cl_int error;
        object_ = ::clCreateCommandQueue(
            context(), device(), properties, &error);

        detail::errHandler(error, __CREATE_COMMAND_QUEUE_ERR);
        if (err != NULL) {
            *err = error;
        }
    }

    static CommandQueue getDefault(cl_int * err = NULL) 
    {
        std::call_once(default_initialized_, makeDefault);
        detail::errHandler(default_error_, __CREATE_COMMAND_QUEUE_ERR);
        if (err != NULL) {
            *err = default_error_;
        }
        return default_;
    }

    CommandQueue() { }

    CommandQueue(const cl_command_queue& commandQueue) : detail::Wrapper<cl_type>(commandQueue) { }

    CommandQueue& operator = (const cl_command_queue& rhs)
    {
        detail::Wrapper<cl_type>::operator=(rhs);
        return *this;
    }

    /*! \brief Copy constructor to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    CommandQueue(const CommandQueue& queue) : detail::Wrapper<cl_type>(queue) {}

    /*! \brief Copy assignment to forward copy to the superclass correctly.
     * Required for MSVC.
     */
    CommandQueue& operator = (const CommandQueue &queue)
    {
        detail::Wrapper<cl_type>::operator=(queue);
        return *this;
    }

    /*! \brief Move constructor to forward move to the superclass correctly.
     * Required for MSVC.
     */
    CommandQueue(CommandQueue&& queue) CL_HPP_NOEXCEPT_ : detail::Wrapper<cl_type>(std::move(queue)) {}

    /*! \brief Move assignment to forward move to the superclass correctly.
     * Required for MSVC.
     */
    CommandQueue& operator = (CommandQueue &&queue)
    {
        detail::Wrapper<cl_type>::operator=(std::move(queue));
        return *this;
    }

    template <typename T>
    cl_int getInfo(cl_command_queue_info name, T* param) const
    {
        return detail::errHandler(
            detail::getInfo(
                &::clGetCommandQueueInfo, object_, name, param),
                __GET_COMMAND_QUEUE_INFO_ERR);
    }

    template <cl_int name> typename
    detail::param_traits<detail::cl_command_queue_info, name>::param_type
    getInfo(cl_int* err = NULL) const
    {
        typename detail::param_traits<
            detail::cl_command_queue_info, name>::param_type param;
        cl_int result = getInfo(name, &param);
        if (err != NULL) {
            *err = result;
        }
        return param;
    }

    cl_int enqueueReadBuffer(
        const Buffer& buffer,
        cl_bool blocking,
        ::size_t offset,
        ::size_t size,
        void* ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueReadBuffer(
                object_, buffer(), blocking, offset, size,
                ptr,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_READ_BUFFER_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueWriteBuffer(
        const Buffer& buffer,
        cl_bool blocking,
        ::size_t offset,
        ::size_t size,
        const void* ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueWriteBuffer(
                object_, buffer(), blocking, offset, size,
                ptr,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
                __ENQUEUE_WRITE_BUFFER_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueCopyBuffer(
        const Buffer& src,
        const Buffer& dst,
        ::size_t src_offset,
        ::size_t dst_offset,
        ::size_t size,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueCopyBuffer(
                object_, src(), dst(), src_offset, dst_offset, size,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQEUE_COPY_BUFFER_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueReadBufferRect(
        const Buffer& buffer,
        cl_bool blocking,
        const size_t<3>& buffer_offset,
        const size_t<3>& host_offset,
        const size_t<3>& region,
        ::size_t buffer_row_pitch,
        ::size_t buffer_slice_pitch,
        ::size_t host_row_pitch,
        ::size_t host_slice_pitch,
        void *ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueReadBufferRect(
                object_, 
                buffer(), 
                blocking, 
                (const ::size_t *)buffer_offset,
                (const ::size_t *)host_offset,
                (const ::size_t *)region,
                buffer_row_pitch,
                buffer_slice_pitch,
                host_row_pitch,
                host_slice_pitch,
                ptr,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
                __ENQUEUE_READ_BUFFER_RECT_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueWriteBufferRect(
        const Buffer& buffer,
        cl_bool blocking,
        const size_t<3>& buffer_offset,
        const size_t<3>& host_offset,
        const size_t<3>& region,
        ::size_t buffer_row_pitch,
        ::size_t buffer_slice_pitch,
        ::size_t host_row_pitch,
        ::size_t host_slice_pitch,
        void *ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueWriteBufferRect(
                object_, 
                buffer(), 
                blocking, 
                (const ::size_t *)buffer_offset,
                (const ::size_t *)host_offset,
                (const ::size_t *)region,
                buffer_row_pitch,
                buffer_slice_pitch,
                host_row_pitch,
                host_slice_pitch,
                ptr,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
                __ENQUEUE_WRITE_BUFFER_RECT_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueCopyBufferRect(
        const Buffer& src,
        const Buffer& dst,
        const size_t<3>& src_origin,
        const size_t<3>& dst_origin,
        const size_t<3>& region,
        ::size_t src_row_pitch,
        ::size_t src_slice_pitch,
        ::size_t dst_row_pitch,
        ::size_t dst_slice_pitch,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueCopyBufferRect(
                object_, 
                src(), 
                dst(), 
                (const ::size_t *)src_origin, 
                (const ::size_t *)dst_origin, 
                (const ::size_t *)region,
                src_row_pitch,
                src_slice_pitch,
                dst_row_pitch,
                dst_slice_pitch,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQEUE_COPY_BUFFER_RECT_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    /**
     * Enqueue a command to fill a buffer object with a pattern
     * of a given size. The pattern is specified a as vector.
     * \tparam PatternType The datatype of the pattern field. 
     *     The pattern type must be an accepted OpenCL data type.
     */
    template<typename PatternType>
    cl_int enqueueFillBuffer(
        const Buffer& buffer,
        PatternType pattern,
        ::size_t offset,
        ::size_t size,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueFillBuffer(
                object_, 
                buffer(),
                static_cast<void*>(&pattern),
                sizeof(PatternType), 
                offset, 
                size,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
                __ENQUEUE_FILL_BUFFER_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

    cl_int enqueueReadImage(
        const Image& image,
        cl_bool blocking,
        const size_t<3>& origin,
        const size_t<3>& region,
        ::size_t row_pitch,
        ::size_t slice_pitch,
        void* ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueReadImage(
                object_, image(), blocking, (const ::size_t *) origin,
                (const ::size_t *) region, row_pitch, slice_pitch, ptr,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_READ_IMAGE_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueWriteImage(
        const Image& image,
        cl_bool blocking,
        const size_t<3>& origin,
        const size_t<3>& region,
        ::size_t row_pitch,
        ::size_t slice_pitch,
        void* ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueWriteImage(
                object_, image(), blocking, (const ::size_t *) origin,
                (const ::size_t *) region, row_pitch, slice_pitch, ptr,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_WRITE_IMAGE_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueCopyImage(
        const Image& src,
        const Image& dst,
        const size_t<3>& src_origin,
        const size_t<3>& dst_origin,
        const size_t<3>& region,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueCopyImage(
                object_, src(), dst(), (const ::size_t *) src_origin,
                (const ::size_t *)dst_origin, (const ::size_t *) region,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_COPY_IMAGE_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    /**
     * Enqueue a command to fill an image object with a specified color.
     * \param fillColor is the color to use to fill the image.
     *     This is a four component RGBA floating-point color value if
     *     the image channel data type is not an unnormalized signed or
     *     unsigned data type.
     */
    cl_int enqueueFillImage(
        const Image& image,
        cl_float4 fillColor,
        const size_t<3>& origin,
        const size_t<3>& region,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueFillImage(
                object_, 
                image(),
                static_cast<void*>(&fillColor), 
                (const ::size_t *) origin, 
                (const ::size_t *) region,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
                __ENQUEUE_FILL_IMAGE_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    /**
     * Enqueue a command to fill an image object with a specified color.
     * \param fillColor is the color to use to fill the image.
     *     This is a four component RGBA signed integer color value if
     *     the image channel data type is an unnormalized signed integer
     *     type.
     */
    cl_int enqueueFillImage(
        const Image& image,
        cl_int4 fillColor,
        const size_t<3>& origin,
        const size_t<3>& region,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueFillImage(
                object_, 
                image(),
                static_cast<void*>(&fillColor), 
                (const ::size_t *) origin, 
                (const ::size_t *) region,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
                __ENQUEUE_FILL_IMAGE_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    /**
     * Enqueue a command to fill an image object with a specified color.
     * \param fillColor is the color to use to fill the image.
     *     This is a four component RGBA unsigned integer color value if
     *     the image channel data type is an unnormalized unsigned integer
     *     type.
     */
    cl_int enqueueFillImage(
        const Image& image,
        cl_uint4 fillColor,
        const size_t<3>& origin,
        const size_t<3>& region,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueFillImage(
                object_, 
                image(),
                static_cast<void*>(&fillColor), 
                (const ::size_t *) origin, 
                (const ::size_t *) region,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
                __ENQUEUE_FILL_IMAGE_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

    cl_int enqueueCopyImageToBuffer(
        const Image& src,
        const Buffer& dst,
        const size_t<3>& src_origin,
        const size_t<3>& region,
        ::size_t dst_offset,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueCopyImageToBuffer(
                object_, src(), dst(), (const ::size_t *) src_origin,
                (const ::size_t *) region, dst_offset,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_COPY_IMAGE_TO_BUFFER_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueCopyBufferToImage(
        const Buffer& src,
        const Image& dst,
        ::size_t src_offset,
        const size_t<3>& dst_origin,
        const size_t<3>& region,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueCopyBufferToImage(
                object_, src(), dst(), src_offset,
                (const ::size_t *) dst_origin, (const ::size_t *) region,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_COPY_BUFFER_TO_IMAGE_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    void* enqueueMapBuffer(
        const Buffer& buffer,
        cl_bool blocking,
        cl_map_flags flags,
        ::size_t offset,
        ::size_t size,
        const vector_class<Event>* events = NULL,
        Event* event = NULL,
        cl_int* err = NULL) const
    {
        cl_event tmp;
        cl_int error;
        void * result = ::clEnqueueMapBuffer(
            object_, buffer(), blocking, flags, offset, size,
            (events != NULL) ? (cl_uint) events->size() : 0,
            (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
            (event != NULL) ? &tmp : NULL,
            &error);

        detail::errHandler(error, __ENQUEUE_MAP_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
        if (event != NULL && error == CL_SUCCESS)
            *event = tmp;

        return result;
    }

    void* enqueueMapImage(
        const Image& buffer,
        cl_bool blocking,
        cl_map_flags flags,
        const size_t<3>& origin,
        const size_t<3>& region,
        ::size_t * row_pitch,
        ::size_t * slice_pitch,
        const vector_class<Event>* events = NULL,
        Event* event = NULL,
        cl_int* err = NULL) const
    {
        cl_event tmp;
        cl_int error;
        void * result = ::clEnqueueMapImage(
            object_, buffer(), blocking, flags,
            (const ::size_t *) origin, (const ::size_t *) region,
            row_pitch, slice_pitch,
            (events != NULL) ? (cl_uint) events->size() : 0,
            (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
            (event != NULL) ? &tmp : NULL,
            &error);

        detail::errHandler(error, __ENQUEUE_MAP_IMAGE_ERR);
        if (err != NULL) {
              *err = error;
        }
        if (event != NULL && error == CL_SUCCESS)
            *event = tmp;
        return result;
    }

    cl_int enqueueUnmapMemObject(
        const Memory& memory,
        void* mapped_ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueUnmapMemObject(
                object_, memory(), mapped_ptr,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_UNMAP_MEM_OBJECT_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

#if CL_HPP_TARGET_OPENCL_VERSION >= 120
    /**
     * Enqueues a marker command which waits for either a list of events to complete, 
     * or all previously enqueued commands to complete.
     *
     * Enqueues a marker command which waits for either a list of events to complete, 
     * or if the list is empty it waits for all commands previously enqueued in command_queue 
     * to complete before it completes. This command returns an event which can be waited on, 
     * i.e. this event can be waited on to insure that all events either in the event_wait_list 
     * or all previously enqueued commands, queued before this command to command_queue, 
     * have completed.
     */
    cl_int enqueueMarkerWithWaitList(
        const vector_class<Event> *events = 0,
        Event *event = 0)
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueMarkerWithWaitList(
                object_,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_MARKER_WAIT_LIST_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    /**
     * A synchronization point that enqueues a barrier operation.
     *
     * Enqueues a barrier command which waits for either a list of events to complete, 
     * or if the list is empty it waits for all commands previously enqueued in command_queue 
     * to complete before it completes. This command blocks command execution, that is, any 
     * following commands enqueued after it do not execute until it completes. This command 
     * returns an event which can be waited on, i.e. this event can be waited on to insure that 
     * all events either in the event_wait_list or all previously enqueued commands, queued 
     * before this command to command_queue, have completed.
     */
    cl_int enqueueBarrierWithWaitList(
        const vector_class<Event> *events = 0,
        Event *event = 0)
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueBarrierWithWaitList(
                object_,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_BARRIER_WAIT_LIST_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }
    
    /**
     * Enqueues a command to indicate with which device a set of memory objects
     * should be associated.
     */
    cl_int enqueueMigrateMemObjects(
        const vector_class<Memory> &memObjects,
        cl_mem_migration_flags flags,
        const vector_class<Event>* events = NULL,
        Event* event = NULL
        )
    {
        cl_event tmp;
        
        cl_mem* localMemObjects = static_cast<cl_mem*>(alloca(memObjects.size() * sizeof(cl_mem)));
        for( int i = 0; i < (int)memObjects.size(); ++i ) {
            localMemObjects[i] = memObjects[i]();
        }


        cl_int err = detail::errHandler(
            ::clEnqueueMigrateMemObjects(
                object_, 
                (cl_uint)memObjects.size(), 
                static_cast<const cl_mem*>(localMemObjects),
                flags,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_UNMAP_MEM_OBJECT_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120

    cl_int enqueueNDRangeKernel(
        const Kernel& kernel,
        const NDRange& offset,
        const NDRange& global,
        const NDRange& local = NullRange,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueNDRangeKernel(
                object_, kernel(), (cl_uint) global.dimensions(),
                offset.dimensions() != 0 ? (const ::size_t*) offset : NULL,
                (const ::size_t*) global,
                local.dimensions() != 0 ? (const ::size_t*) local : NULL,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_NDRANGE_KERNEL_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueTask(
        const Kernel& kernel,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueTask(
                object_, kernel(),
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_TASK_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    cl_int enqueueNativeKernel(
        void (CL_CALLBACK *userFptr)(void *),
        std::pair<void*, ::size_t> args,
        const vector_class<Memory>* mem_objects = NULL,
        const vector_class<const void*>* mem_locs = NULL,
        const vector_class<Event>* events = NULL,
        Event* event = NULL) const
    {
        cl_mem * mems = (mem_objects != NULL && mem_objects->size() > 0) 
            ? (cl_mem*) alloca(mem_objects->size() * sizeof(cl_mem))
            : NULL;

        if (mems != NULL) {
            for (unsigned int i = 0; i < mem_objects->size(); i++) {
                mems[i] = ((*mem_objects)[i])();
            }
        }

        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueNativeKernel(
                object_, userFptr, args.first, args.second,
                (mem_objects != NULL) ? (cl_uint) mem_objects->size() : 0,
                mems,
                (mem_locs != NULL && mem_locs->size() > 0) ? (const void **) &mem_locs->front() : NULL,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_NATIVE_KERNEL);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

/**
 * Deprecated APIs for 1.2
 */
#if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)
    CL_EXT_PREFIX__VERSION_1_1_DEPRECATED 
    cl_int enqueueMarker(Event* event = NULL) const CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
    {
        cl_event tmp;
        cl_int err = detail::errHandler(
            ::clEnqueueMarker(
                object_, 
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_MARKER_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }

    CL_EXT_PREFIX__VERSION_1_1_DEPRECATED
    cl_int enqueueWaitForEvents(const vector_class<Event>& events) const CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
    {
        return detail::errHandler(
            ::clEnqueueWaitForEvents(
                object_,
                (cl_uint) events.size(),
                events.size() > 0 ? (const cl_event*) &events.front() : NULL),
            __ENQUEUE_WAIT_FOR_EVENTS_ERR);
    }
#endif // defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)

    cl_int enqueueAcquireGLObjects(
         const vector_class<Memory>* mem_objects = NULL,
         const vector_class<Event>* events = NULL,
         Event* event = NULL) const
     {
        cl_event tmp;
        cl_int err = detail::errHandler(
             ::clEnqueueAcquireGLObjects(
                 object_,
                 (mem_objects != NULL) ? (cl_uint) mem_objects->size() : 0,
                 (mem_objects != NULL && mem_objects->size() > 0) ? (const cl_mem *) &mem_objects->front(): NULL,
                 (events != NULL) ? (cl_uint) events->size() : 0,
                 (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                 (event != NULL) ? &tmp : NULL),
             __ENQUEUE_ACQUIRE_GL_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
     }

    cl_int enqueueReleaseGLObjects(
         const vector_class<Memory>* mem_objects = NULL,
         const vector_class<Event>* events = NULL,
         Event* event = NULL) const
     {
        cl_event tmp;
        cl_int err = detail::errHandler(
             ::clEnqueueReleaseGLObjects(
                 object_,
                 (mem_objects != NULL) ? (cl_uint) mem_objects->size() : 0,
                 (mem_objects != NULL && mem_objects->size() > 0) ? (const cl_mem *) &mem_objects->front(): NULL,
                 (events != NULL) ? (cl_uint) events->size() : 0,
                 (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                 (event != NULL) ? &tmp : NULL),
             __ENQUEUE_RELEASE_GL_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
     }

#if defined (CL_HPP_USE_DX_INTEROP)
typedef CL_API_ENTRY cl_int (CL_API_CALL *PFN_clEnqueueAcquireD3D10ObjectsKHR)(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects, cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list, cl_event* event);
typedef CL_API_ENTRY cl_int (CL_API_CALL *PFN_clEnqueueReleaseD3D10ObjectsKHR)(
    cl_command_queue command_queue, cl_uint num_objects,
    const cl_mem* mem_objects,  cl_uint num_events_in_wait_list,
    const cl_event* event_wait_list, cl_event* event);

    cl_int enqueueAcquireD3D10Objects(
         const vector_class<Memory>* mem_objects = NULL,
         const vector_class<Event>* events = NULL,
         Event* event = NULL) const
    {
        static PFN_clEnqueueAcquireD3D10ObjectsKHR pfn_clEnqueueAcquireD3D10ObjectsKHR = NULL;
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
        cl_context context = getInfo<CL_QUEUE_CONTEXT>();
        cl::Device device(getInfo<CL_QUEUE_DEVICE>());
        cl_platform_id platform = device.getInfo<CL_DEVICE_PLATFORM>();
        CL_HPP_INIT_CL_EXT_FCN_PTR_PLATFORM_(platform, clEnqueueAcquireD3D10ObjectsKHR);
#endif
#if CL_HPP_TARGET_OPENCL_VERSION >= 110
        CL_HPP_INIT_CL_EXT_FCN_PTR_(clEnqueueAcquireD3D10ObjectsKHR);
#endif
        
        cl_event tmp;
        cl_int err = detail::errHandler(
             pfn_clEnqueueAcquireD3D10ObjectsKHR(
                 object_,
                 (mem_objects != NULL) ? (cl_uint) mem_objects->size() : 0,
                 (mem_objects != NULL && mem_objects->size() > 0) ? (const cl_mem *) &mem_objects->front(): NULL,
                 (events != NULL) ? (cl_uint) events->size() : 0,
                 (events != NULL) ? (cl_event*) &events->front() : NULL,
                 (event != NULL) ? &tmp : NULL),
             __ENQUEUE_ACQUIRE_GL_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
     }

    cl_int enqueueReleaseD3D10Objects(
         const vector_class<Memory>* mem_objects = NULL,
         const vector_class<Event>* events = NULL,
         Event* event = NULL) const
    {
        static PFN_clEnqueueReleaseD3D10ObjectsKHR pfn_clEnqueueReleaseD3D10ObjectsKHR = NULL;
#if CL_HPP_TARGET_OPENCL_VERSION >= 120
        cl_context context = getInfo<CL_QUEUE_CONTEXT>();
        cl::Device device(getInfo<CL_QUEUE_DEVICE>());
        cl_platform_id platform = device.getInfo<CL_DEVICE_PLATFORM>();
        CL_HPP_INIT_CL_EXT_FCN_PTR_PLATFORM_(platform, clEnqueueReleaseD3D10ObjectsKHR);
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 120
#if CL_HPP_TARGET_OPENCL_VERSION >= 110
        CL_HPP_INIT_CL_EXT_FCN_PTR_(clEnqueueReleaseD3D10ObjectsKHR);
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110

        cl_event tmp;
        cl_int err = detail::errHandler(
            pfn_clEnqueueReleaseD3D10ObjectsKHR(
                object_,
                (mem_objects != NULL) ? (cl_uint) mem_objects->size() : 0,
                (mem_objects != NULL && mem_objects->size() > 0) ? (const cl_mem *) &mem_objects->front(): NULL,
                (events != NULL) ? (cl_uint) events->size() : 0,
                (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
                (event != NULL) ? &tmp : NULL),
            __ENQUEUE_RELEASE_GL_ERR);

        if (event != NULL && err == CL_SUCCESS)
            *event = tmp;

        return err;
    }
#endif

/**
 * Deprecated APIs for 1.2
 */
#if defined(CL_USE_DEPRECATED_OPENCL_1_1_APIS)
    CL_EXT_PREFIX__VERSION_1_1_DEPRECATED
    cl_int enqueueBarrier() const CL_EXT_SUFFIX__VERSION_1_1_DEPRECATED
    {
        return detail::errHandler(
            ::clEnqueueBarrier(object_),
            __ENQUEUE_BARRIER_ERR);
    }
#endif // CL_USE_DEPRECATED_OPENCL_1_1_APIS

    cl_int flush() const
    {
        return detail::errHandler(::clFlush(object_), __FLUSH_ERR);
    }

    cl_int finish() const
    {
        return detail::errHandler(::clFinish(object_), __FINISH_ERR);
    }
};

CL_HPP_DEFINE_STATIC_MEMBER_ std::once_flag CommandQueue::default_initialized_;
CL_HPP_DEFINE_STATIC_MEMBER_ CommandQueue CommandQueue::default_;
CL_HPP_DEFINE_STATIC_MEMBER_ cl_int CommandQueue::default_error_ = CL_SUCCESS;

template< typename IteratorType >
Buffer::Buffer(
    const Context &context,
    IteratorType startIterator,
    IteratorType endIterator,
    bool readOnly,
    bool useHostPtr,
    cl_int* err)
{
    typedef typename std::iterator_traits<IteratorType>::value_type DataType;
    cl_int error;

    cl_mem_flags flags = 0;
    if( readOnly ) {
        flags |= CL_MEM_READ_ONLY;
    }
    else {
        flags |= CL_MEM_READ_WRITE;
    }
    if( useHostPtr ) {
        flags |= CL_MEM_USE_HOST_PTR;
    }
    
    ::size_t size = sizeof(DataType)*(endIterator - startIterator);

    if( useHostPtr ) {
        object_ = ::clCreateBuffer(context(), flags, size, static_cast<DataType*>(&*startIterator), &error);
    } else {
        object_ = ::clCreateBuffer(context(), flags, size, 0, &error);
    }

    detail::errHandler(error, __CREATE_BUFFER_ERR);
    if (err != NULL) {
        *err = error;
    }

    if( !useHostPtr ) {
        CommandQueue queue(context, 0, &error);
        detail::errHandler(error, __CREATE_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }

        error = cl::copy(queue, startIterator, endIterator, *this);
        detail::errHandler(error, __CREATE_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }
}

template< typename IteratorType >
Buffer::Buffer(
    const CommandQueue &queue,
    IteratorType startIterator,
    IteratorType endIterator,
    bool readOnly,
    bool useHostPtr,
    cl_int* err)
{
    typedef typename std::iterator_traits<IteratorType>::value_type DataType;
    cl_int error;

    cl_mem_flags flags = 0;
    if (readOnly) {
        flags |= CL_MEM_READ_ONLY;
    }
    else {
        flags |= CL_MEM_READ_WRITE;
    }
    if (useHostPtr) {
        flags |= CL_MEM_USE_HOST_PTR;
    }

    ::size_t size = sizeof(DataType)*(endIterator - startIterator);

    Context context = queue.getInfo<CL_QUEUE_CONTEXT>();

    if (useHostPtr) {
        object_ = ::clCreateBuffer(context(), flags, size, static_cast<DataType*>(&*startIterator), &error);
    }
    else {
        object_ = ::clCreateBuffer(context(), flags, size, 0, &error);
    }

    detail::errHandler(error, __CREATE_BUFFER_ERR);
    if (err != NULL) {
        *err = error;
    }

    if (!useHostPtr) {
        error = cl::copy(queue, startIterator, endIterator, *this);
        detail::errHandler(error, __CREATE_BUFFER_ERR);
        if (err != NULL) {
            *err = error;
        }
    }
}

inline cl_int enqueueReadBuffer(
    const Buffer& buffer,
    cl_bool blocking,
    ::size_t offset,
    ::size_t size,
    void* ptr,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueReadBuffer(buffer, blocking, offset, size, ptr, events, event);
}

inline cl_int enqueueWriteBuffer(
        const Buffer& buffer,
        cl_bool blocking,
        ::size_t offset,
        ::size_t size,
        const void* ptr,
        const vector_class<Event>* events = NULL,
        Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueWriteBuffer(buffer, blocking, offset, size, ptr, events, event);
}

inline void* enqueueMapBuffer(
        const Buffer& buffer,
        cl_bool blocking,
        cl_map_flags flags,
        ::size_t offset,
        ::size_t size,
        const vector_class<Event>* events = NULL,
        Event* event = NULL,
        cl_int* err = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);
    detail::errHandler(error, __ENQUEUE_MAP_BUFFER_ERR);
    if (err != NULL) {
        *err = error;
    }

    void * result = ::clEnqueueMapBuffer(
            queue(), buffer(), blocking, flags, offset, size,
            (events != NULL) ? (cl_uint) events->size() : 0,
            (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
            (cl_event*) event,
            &error);

    detail::errHandler(error, __ENQUEUE_MAP_BUFFER_ERR);
    if (err != NULL) {
        *err = error;
    }
    return result;
}

inline cl_int enqueueUnmapMemObject(
    const Memory& memory,
    void* mapped_ptr,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);
    detail::errHandler(error, __ENQUEUE_MAP_BUFFER_ERR);
    if (error != CL_SUCCESS) {
        return error;
    }

    cl_event tmp;
    cl_int err = detail::errHandler(
        ::clEnqueueUnmapMemObject(
            queue(), memory(), mapped_ptr,
            (events != NULL) ? (cl_uint) events->size() : 0,
            (events != NULL && events->size() > 0) ? (cl_event*) &events->front() : NULL,
            (event != NULL) ? &tmp : NULL),
        __ENQUEUE_UNMAP_MEM_OBJECT_ERR);

    if (event != NULL && err == CL_SUCCESS)
        *event = tmp;

    return err;
}

inline cl_int enqueueCopyBuffer(
        const Buffer& src,
        const Buffer& dst,
        ::size_t src_offset,
        ::size_t dst_offset,
        ::size_t size,
        const vector_class<Event>* events = NULL,
        Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueCopyBuffer(src, dst, src_offset, dst_offset, size, events, event);
}

/**
 * Blocking copy operation between iterators and a buffer.
 * Host to Device.
 * Uses default command queue.
 */
template< typename IteratorType >
inline cl_int copy( IteratorType startIterator, IteratorType endIterator, cl::Buffer &buffer )
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);
    if (error != CL_SUCCESS)
        return error;

    return cl::copy(queue, startIterator, endIterator, buffer);
}

/**
 * Blocking copy operation between iterators and a buffer.
 * Device to Host.
 * Uses default command queue.
 */
template< typename IteratorType >
inline cl_int copy( const cl::Buffer &buffer, IteratorType startIterator, IteratorType endIterator )
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);
    if (error != CL_SUCCESS)
        return error;

    return cl::copy(queue, buffer, startIterator, endIterator);
}

/**
 * Blocking copy operation between iterators and a buffer.
 * Host to Device.
 * Uses specified queue.
 */
template< typename IteratorType >
inline cl_int copy( const CommandQueue &queue, IteratorType startIterator, IteratorType endIterator, cl::Buffer &buffer )
{
    typedef typename std::iterator_traits<IteratorType>::value_type DataType;
    cl_int error;
    
    ::size_t length = endIterator-startIterator;
    ::size_t byteLength = length*sizeof(DataType);

    DataType *pointer = 
        static_cast<DataType*>(queue.enqueueMapBuffer(buffer, CL_TRUE, CL_MAP_WRITE, 0, byteLength, 0, 0, &error));
    // if exceptions enabled, enqueueMapBuffer will throw
    if( error != CL_SUCCESS ) {
        return error;
    }
#if defined(_MSC_VER)
    std::copy(
        startIterator, 
        endIterator, 
        stdext::checked_array_iterator<DataType*>(
            pointer, length));
#else
    std::copy(startIterator, endIterator, pointer);
#endif
    Event endEvent;
    error = queue.enqueueUnmapMemObject(buffer, pointer, 0, &endEvent);
    // if exceptions enabled, enqueueUnmapMemObject will throw
    if( error != CL_SUCCESS ) { 
        return error;
    }
    endEvent.wait();
    return CL_SUCCESS;
}

/**
 * Blocking copy operation between iterators and a buffer.
 * Device to Host.
 * Uses specified queue.
 */
template< typename IteratorType >
inline cl_int copy( const CommandQueue &queue, const cl::Buffer &buffer, IteratorType startIterator, IteratorType endIterator )
{
    typedef typename std::iterator_traits<IteratorType>::value_type DataType;
    cl_int error;
        
    ::size_t length = endIterator-startIterator;
    ::size_t byteLength = length*sizeof(DataType);

    DataType *pointer = 
        static_cast<DataType*>(queue.enqueueMapBuffer(buffer, CL_TRUE, CL_MAP_READ, 0, byteLength, 0, 0, &error));
    // if exceptions enabled, enqueueMapBuffer will throw
    if( error != CL_SUCCESS ) {
        return error;
    }
    std::copy(pointer, pointer + length, startIterator);
    Event endEvent;
    error = queue.enqueueUnmapMemObject(buffer, pointer, 0, &endEvent);
    // if exceptions enabled, enqueueUnmapMemObject will throw
    if( error != CL_SUCCESS ) { 
        return error;
    }
    endEvent.wait();
    return CL_SUCCESS;
}

#if CL_HPP_TARGET_OPENCL_VERSION >= 110
inline cl_int enqueueReadBufferRect(
    const Buffer& buffer,
    cl_bool blocking,
    const size_t<3>& buffer_offset,
    const size_t<3>& host_offset,
    const size_t<3>& region,
    ::size_t buffer_row_pitch,
    ::size_t buffer_slice_pitch,
    ::size_t host_row_pitch,
    ::size_t host_slice_pitch,
    void *ptr,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueReadBufferRect(
        buffer, 
        blocking, 
        buffer_offset, 
        host_offset,
        region,
        buffer_row_pitch,
        buffer_slice_pitch,
        host_row_pitch,
        host_slice_pitch,
        ptr, 
        events, 
        event);
}

inline cl_int enqueueWriteBufferRect(
    const Buffer& buffer,
    cl_bool blocking,
    const size_t<3>& buffer_offset,
    const size_t<3>& host_offset,
    const size_t<3>& region,
    ::size_t buffer_row_pitch,
    ::size_t buffer_slice_pitch,
    ::size_t host_row_pitch,
    ::size_t host_slice_pitch,
    void *ptr,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueWriteBufferRect(
        buffer, 
        blocking, 
        buffer_offset, 
        host_offset,
        region,
        buffer_row_pitch,
        buffer_slice_pitch,
        host_row_pitch,
        host_slice_pitch,
        ptr, 
        events, 
        event);
}

inline cl_int enqueueCopyBufferRect(
    const Buffer& src,
    const Buffer& dst,
    const size_t<3>& src_origin,
    const size_t<3>& dst_origin,
    const size_t<3>& region,
    ::size_t src_row_pitch,
    ::size_t src_slice_pitch,
    ::size_t dst_row_pitch,
    ::size_t dst_slice_pitch,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueCopyBufferRect(
        src,
        dst,
        src_origin,
        dst_origin,
        region,
        src_row_pitch,
        src_slice_pitch,
        dst_row_pitch,
        dst_slice_pitch,
        events, 
        event);
}
#endif // CL_HPP_TARGET_OPENCL_VERSION >= 110

inline cl_int enqueueReadImage(
    const Image& image,
    cl_bool blocking,
    const size_t<3>& origin,
    const size_t<3>& region,
    ::size_t row_pitch,
    ::size_t slice_pitch,
    void* ptr,
    const vector_class<Event>* events = NULL,
    Event* event = NULL) 
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueReadImage(
        image,
        blocking,
        origin,
        region,
        row_pitch,
        slice_pitch,
        ptr,
        events, 
        event);
}

inline cl_int enqueueWriteImage(
    const Image& image,
    cl_bool blocking,
    const size_t<3>& origin,
    const size_t<3>& region,
    ::size_t row_pitch,
    ::size_t slice_pitch,
    void* ptr,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueWriteImage(
        image,
        blocking,
        origin,
        region,
        row_pitch,
        slice_pitch,
        ptr,
        events, 
        event);
}

inline cl_int enqueueCopyImage(
    const Image& src,
    const Image& dst,
    const size_t<3>& src_origin,
    const size_t<3>& dst_origin,
    const size_t<3>& region,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueCopyImage(
        src,
        dst,
        src_origin,
        dst_origin,
        region,
        events,
        event);
}

inline cl_int enqueueCopyImageToBuffer(
    const Image& src,
    const Buffer& dst,
    const size_t<3>& src_origin,
    const size_t<3>& region,
    ::size_t dst_offset,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueCopyImageToBuffer(
        src,
        dst,
        src_origin,
        region,
        dst_offset,
        events,
        event);
}

inline cl_int enqueueCopyBufferToImage(
    const Buffer& src,
    const Image& dst,
    ::size_t src_offset,
    const size_t<3>& dst_origin,
    const size_t<3>& region,
    const vector_class<Event>* events = NULL,
    Event* event = NULL)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.enqueueCopyBufferToImage(
        src,
        dst,
        src_offset,
        dst_origin,
        region,
        events,
        event);
}


inline cl_int flush(void)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    }

    return queue.flush();
}

inline cl_int finish(void)
{
    cl_int error;
    CommandQueue queue = CommandQueue::getDefault(&error);

    if (error != CL_SUCCESS) {
        return error;
    } 


    return queue.finish();
}

struct EnqueueArgs
{
    CommandQueue queue_;
    const NDRange offset_;
    const NDRange global_;
    const NDRange local_;
    vector_class<Event> events_;

    EnqueueArgs(NDRange global) : 
      queue_(CommandQueue::getDefault()),
      offset_(NullRange), 
      global_(global),
      local_(NullRange)
    {

    }

    EnqueueArgs(NDRange global, NDRange local) : 
      queue_(CommandQueue::getDefault()),
      offset_(NullRange), 
      global_(global),
      local_(local)
    {

    }

    EnqueueArgs(NDRange offset, NDRange global, NDRange local) : 
      queue_(CommandQueue::getDefault()),
      offset_(offset), 
      global_(global),
      local_(local)
    {

    }

    EnqueueArgs(Event e, NDRange global) : 
      queue_(CommandQueue::getDefault()),
      offset_(NullRange), 
      global_(global),
      local_(NullRange)
    {
        events_.push_back(e);
    }

    EnqueueArgs(Event e, NDRange global, NDRange local) : 
      queue_(CommandQueue::getDefault()),
      offset_(NullRange), 
      global_(global),
      local_(local)
    {
        events_.push_back(e);
    }

    EnqueueArgs(Event e, NDRange offset, NDRange global, NDRange local) : 
      queue_(CommandQueue::getDefault()),
      offset_(offset), 
      global_(global),
      local_(local)
    {
        events_.push_back(e);
    }

    EnqueueArgs(const vector_class<Event> &events, NDRange global) : 
      queue_(CommandQueue::getDefault()),
      offset_(NullRange), 
      global_(global),
      local_(NullRange),
      events_(events)
    {

    }

    EnqueueArgs(const vector_class<Event> &events, NDRange global, NDRange local) : 
      queue_(CommandQueue::getDefault()),
      offset_(NullRange), 
      global_(global),
      local_(local),
      events_(events)
    {

    }

    EnqueueArgs(const vector_class<Event> &events, NDRange offset, NDRange global, NDRange local) : 
      queue_(CommandQueue::getDefault()),
      offset_(offset), 
      global_(global),
      local_(local),
      events_(events)
    {

    }

    EnqueueArgs(CommandQueue &queue, NDRange global) : 
      queue_(queue),
      offset_(NullRange), 
      global_(global),
      local_(NullRange)
    {

    }

    EnqueueArgs(CommandQueue &queue, NDRange global, NDRange local) : 
      queue_(queue),
      offset_(NullRange), 
      global_(global),
      local_(local)
    {

    }

    EnqueueArgs(CommandQueue &queue, NDRange offset, NDRange global, NDRange local) : 
      queue_(queue),
      offset_(offset), 
      global_(global),
      local_(local)
    {

    }

    EnqueueArgs(CommandQueue &queue, Event e, NDRange global) : 
      queue_(queue),
      offset_(NullRange), 
      global_(global),
      local_(NullRange)
    {
        events_.push_back(e);
    }

    EnqueueArgs(CommandQueue &queue, Event e, NDRange global, NDRange local) : 
      queue_(queue),
      offset_(NullRange), 
      global_(global),
      local_(local)
    {
        events_.push_back(e);
    }

    EnqueueArgs(CommandQueue &queue, Event e, NDRange offset, NDRange global, NDRange local) : 
      queue_(queue),
      offset_(offset), 
      global_(global),
      local_(local)
    {
        events_.push_back(e);
    }

    EnqueueArgs(CommandQueue &queue, const vector_class<Event> &events, NDRange global) : 
      queue_(queue),
      offset_(NullRange), 
      global_(global),
      local_(NullRange),
      events_(events)
    {

    }

    EnqueueArgs(CommandQueue &queue, const vector_class<Event> &events, NDRange global, NDRange local) : 
      queue_(queue),
      offset_(NullRange), 
      global_(global),
      local_(local),
      events_(events)
    {

    }

    EnqueueArgs(CommandQueue &queue, const vector_class<Event> &events, NDRange offset, NDRange global, NDRange local) : 
      queue_(queue),
      offset_(offset), 
      global_(global),
      local_(local),
      events_(events)
    {

    }
};


//----------------------------------------------------------------------------------------------


/**
 * Type safe kernel functor.
 * 
 */
template<typename T, typename... Ts>
class KernelFunctor
{
private:
	Kernel kernel_;

	template<int index, typename T0, typename... T1s>
	void setArgs(T0 t0, T1s... t1s)
	{
		kernel_.setArg(index, t0);
		setArgs<index + 1, T1s...>(t1s...);
	}

	template<int index, typename T0>
	void setArgs(T0 t0)
	{
		kernel_.setArg(index, t0);
	}

public:
	KernelFunctor(Kernel kernel) : kernel_(kernel)
	{}

	KernelFunctor(
		const Program& program,
        const string_class name,
		cl_int * err = NULL) :
		kernel_(program, name.c_str(), err)
	{}

	//! \brief Return type of the functor
	typedef Event result_type;

	/**
	 * Enqueue kernel.
	 * @param args Launch parameters of the kernel.
	 * @param t0... List of kernel arguments based on the template type of the functor.
	 */
	Event operator() (
		const EnqueueArgs& args,
		T t0,
		Ts... ts)
	{
		Event event;
		setArgs<0, T, Ts...>(t0, ts...);
		
		int err = args.queue_.enqueueNDRangeKernel(
			kernel_,
			args.offset_,
			args.global_,
			args.local_,
			&args.events_,
			&event);

		return event;
	}

	/**
	* Enqueue kernel with support for error code.
	* @param args Launch parameters of the kernel.
	* @param t0... List of kernel arguments based on the template type of the functor.
	* @param error Out parameter returning the error code from the execution.
	*/
	Event operator() (
		const EnqueueArgs& args,
		T t0,
		Ts... ts,
		cl_int &error)
	{
		Event event;
		setArgs<0, T, Ts...>(t0, ts...);

		error = args.queue_.enqueueNDRangeKernel(
			kernel_,
			args.offset_,
			args.global_,
			args.local_,
			&args.events_,
			&event);
		
		return event;
	}
};

/**
 * Backward compatibility class to ensure that cl.hpp code works with cl2.hpp.
 * Please use KernelFunctor directly.
 */
template<typename T, typename... Ts>
struct make_kernel
{
	typedef KernelFunctor<T, Ts...> FunctorType;

	FunctorType functor_;

	make_kernel(
		const Program& program,
        const string_class name,
		cl_int * err = NULL) :
		functor_(FunctorType(program, name, err))
	{}

	make_kernel(
		const Kernel kernel) :
		functor_(FunctorType(kernel))
	{}

	//! \brief Return type of the functor
	typedef Event result_type;

	//! \brief Function signature of kernel functor with no event dependency.
	typedef Event type_(
		const EnqueueArgs&,
		T, Ts...);

	Event operator()(
		const EnqueueArgs& enqueueArgs,
		T arg0, Ts... args)
	{
		return functor_(
			enqueueArgs, arg0, args...);
	}
};


//----------------------------------------------------------------------------------------------------------------------

#undef CL_HPP_ERR_STR_
#if !defined(CL_HPP_USER_OVERRIDE_ERROR_STRINGS)
#undef __GET_DEVICE_INFO_ERR
#undef __GET_PLATFORM_INFO_ERR
#undef __GET_DEVICE_IDS_ERR
#undef __GET_CONTEXT_INFO_ERR
#undef __GET_EVENT_INFO_ERR
#undef __GET_EVENT_PROFILE_INFO_ERR
#undef __GET_MEM_OBJECT_INFO_ERR
#undef __GET_IMAGE_INFO_ERR
#undef __GET_SAMPLER_INFO_ERR
#undef __GET_KERNEL_INFO_ERR
#undef __GET_KERNEL_ARG_INFO_ERR
#undef __GET_KERNEL_WORK_GROUP_INFO_ERR
#undef __GET_PROGRAM_INFO_ERR
#undef __GET_PROGRAM_BUILD_INFO_ERR
#undef __GET_COMMAND_QUEUE_INFO_ERR

#undef __CREATE_CONTEXT_ERR
#undef __CREATE_CONTEXT_FROM_TYPE_ERR
#undef __GET_SUPPORTED_IMAGE_FORMATS_ERR

#undef __CREATE_BUFFER_ERR
#undef __CREATE_SUBBUFFER_ERR
#undef __CREATE_IMAGE2D_ERR
#undef __CREATE_IMAGE3D_ERR
#undef __CREATE_SAMPLER_ERR
#undef __SET_MEM_OBJECT_DESTRUCTOR_CALLBACK_ERR

#undef __CREATE_USER_EVENT_ERR
#undef __SET_USER_EVENT_STATUS_ERR
#undef __SET_EVENT_CALLBACK_ERR
#undef __SET_PRINTF_CALLBACK_ERR

#undef __WAIT_FOR_EVENTS_ERR

#undef __CREATE_KERNEL_ERR
#undef __SET_KERNEL_ARGS_ERR
#undef __CREATE_PROGRAM_WITH_SOURCE_ERR
#undef __CREATE_PROGRAM_WITH_BINARY_ERR
#undef __CREATE_PROGRAM_WITH_BUILT_IN_KERNELS_ERR
#undef __BUILD_PROGRAM_ERR
#undef __CREATE_KERNELS_IN_PROGRAM_ERR

#undef __CREATE_COMMAND_QUEUE_ERR
#undef __SET_COMMAND_QUEUE_PROPERTY_ERR
#undef __ENQUEUE_READ_BUFFER_ERR
#undef __ENQUEUE_WRITE_BUFFER_ERR
#undef __ENQUEUE_READ_BUFFER_RECT_ERR
#undef __ENQUEUE_WRITE_BUFFER_RECT_ERR
#undef __ENQEUE_COPY_BUFFER_ERR
#undef __ENQEUE_COPY_BUFFER_RECT_ERR
#undef __ENQUEUE_READ_IMAGE_ERR
#undef __ENQUEUE_WRITE_IMAGE_ERR
#undef __ENQUEUE_COPY_IMAGE_ERR
#undef __ENQUEUE_COPY_IMAGE_TO_BUFFER_ERR
#undef __ENQUEUE_COPY_BUFFER_TO_IMAGE_ERR
#undef __ENQUEUE_MAP_BUFFER_ERR
#undef __ENQUEUE_MAP_IMAGE_ERR
#undef __ENQUEUE_UNMAP_MEM_OBJECT_ERR
#undef __ENQUEUE_NDRANGE_KERNEL_ERR
#undef __ENQUEUE_TASK_ERR
#undef __ENQUEUE_NATIVE_KERNEL

#undef __UNLOAD_COMPILER_ERR
#undef __CREATE_SUB_DEVICES_ERR
#endif //CL_HPP_USER_OVERRIDE_ERROR_STRINGS

// Extensions
#undef CL_HPP_INIT_CL_EXT_FCN_PTR_
#undef CL_HPP_INIT_CL_EXT_FCN_PTR_PLATFORM_

#if defined(CL_HPP_USE_CL_DEVICE_FISSION)
#undef CL_HPP_PARAM_NAME_DEVICE_FISSION_
#endif // CL_HPP_USE_CL_DEVICE_FISSION

#undef CL_HPP_NOEXCEPT_
#undef CL_HPP_DEFINE_STATIC_MEMBER_

} // namespace cl

#endif // CL_HPP_
