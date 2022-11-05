#pragma once
// Macros for OpenCL versions
#define OPENCL_VERSION_1_2  1.2f
#define OPENCL_VERSION_2_0  2.0f
#define OPENCL_VERSION_2_2  2.2f

// Suppress a compiler warning about undefined CL_TARGET_OPENCL_VERSION
// Khronos ICD supports only latest OpenCL version
//#define CL_TARGET_OPENCL_VERSION 120

// Suppress a compiler warning about 'clCreateCommandQueue': was declared deprecated
// for OpenCL 2.0
#define CL_USE_DEPRECATED_OPENCL_1_2_APIS

#include "CL\cl.h"
#include "utils.h"

//for perf. counters
#include <Windows.h>


struct ocl_args_d_t
{
    ocl_args_d_t();
    ~ocl_args_d_t();

    // Regular OpenCL objects:
    cl_context       context;           // hold the context handler
    cl_device_id     device;            // hold the selected device handler
    cl_command_queue commandQueue;      // hold the commands-queue handler
    cl_program       program;           // hold the program handler
    cl_kernel        kernel;            // hold the kernel handler
    float            platformVersion;   // hold the OpenCL platform version (default 2.0)
    float            deviceVersion;     // hold the OpenCL device version (default. 2.0)
    float            compilerVersion;   // hold the device OpenCL C version (default. 2.0)

    // Objects that are specific for algorithm implemented in this sample
    cl_mem          mnemonic_bits;
    cl_mem          mnemonic;
    cl_mem          seed;
    cl_mem          work;
};

/* This function helps to create informative messages in
 * case when OpenCL errors occur. It returns a string
 * representation for an OpenCL error code.
 * (E.g. "CL_DEVICE_NOT_FOUND" instead of just -1.)
 */
const char* TranslateOpenCLError(cl_int errorCode);
/*
 * Check whether an OpenCL platform is the required platform
 * (based on the platform's name)
 */
bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform);
/*
 * Find and return the preferred OpenCL platform
 * In case that preferredPlatform is NULL, the ID of the first discovered platform will be returned
 */
cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType);
/*
 * This function read the OpenCL platdorm and device versions
 * (using clGetxxxInfo API) and stores it in the ocl structure.
 * Later it will enable us to support both OpenCL 1.2 and 2.0 platforms and devices
 * in the same program.
 */
int GetPlatformAndDeviceVersion(cl_platform_id platformId, ocl_args_d_t* ocl);
/*
 * This function picks/creates necessary OpenCL objects which are needed.
 * The objects are:
 * OpenCL platform, device, context, and command queue.
 *
 * All these steps are needed to be performed once in a regular OpenCL application.
 * This happens before actual compute kernels calls are performed.
 *
 * For convenience, in this application you store all those basic OpenCL objects in structure ocl_args_d_t,
 * so this function populates fields of this structure, which is passed as parameter ocl.
 * Please, consider reviewing the fields before going further.
 * The structure definition is right in the beginning of this file.
 */
int SetupOpenCL(ocl_args_d_t* ocl, cl_device_type deviceType);
/*
 * Create and build OpenCL program from its source code
 */
int CreateAndBuildProgram(ocl_args_d_t* ocl, const char* file);
/*
 * Create OpenCL buffers from host memory
 * These buffers will be used later by the OpenCL kernel
 */
int CreateBufferArguments(ocl_args_d_t* ocl, cl_ulong* mnemonic_bits, cl_uchar* mnemonic, cl_uchar* seed, size_t mnemonic_bits_buf_size, size_t mnemonic_buf_size, size_t  seed_buf_size);
/*
 * Set kernel arguments
 */
cl_uint SetKernelArguments(ocl_args_d_t* ocl);
/*
 * Execute the kernel
 */
cl_uint ExecuteKernel(ocl_args_d_t* ocl, cl_uint num);

bool ReadAndVerify(ocl_args_d_t* ocl, cl_uchar* mnemonic, cl_uchar* seed, size_t mnemonic_buf_size, size_t seed_buf_size);
int SetupOpenCLMY(ocl_args_d_t* ocl, cl_uint num_platform, cl_uint num_device);








