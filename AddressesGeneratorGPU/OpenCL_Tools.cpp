
#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>
#include <iostream>
#include <string>
#include <filesystem>


#include "OpenCL_Tools.h"
#include <AOCLUtils/opencl.h>


using namespace aocl_utils;

#define NUMBER_OF_FILES 1

const char* sourceFilesPath[NUMBER_OF_FILES] = {
	//"OpenCL_Crypto/common.cl", 
	//"OpenCL_Crypto/ripemd.cl", 
	//"OpenCL_Crypto/sha2.cl", 
	//"OpenCL_Crypto/secp256k1_common.cl", 
	//"OpenCL_Crypto/secp256k1_scalar.cl", 
	//"OpenCL_Crypto/secp256k1_field.cl", 
	//"OpenCL_Crypto/secp256k1_group.cl", 
	//"OpenCL_Crypto/secp256k1_prec.cl", 
	//"OpenCL_Crypto/secp256k1.cl", 
	//"OpenCL_Crypto/address.cl", 
	//"OpenCL_Crypto/mnemonic_constants.cl", 
	//"OpenCL_Crypto/int_to_address.cl",
	"Template.cl"
};



const char* TranslateOpenCLError(cl_int errorCode)
{
	switch (errorCode)
	{
	case CL_SUCCESS:                            return "CL_SUCCESS";
	case CL_DEVICE_NOT_FOUND:                   return "CL_DEVICE_NOT_FOUND";
	case CL_DEVICE_NOT_AVAILABLE:               return "CL_DEVICE_NOT_AVAILABLE";
	case CL_COMPILER_NOT_AVAILABLE:             return "CL_COMPILER_NOT_AVAILABLE";
	case CL_MEM_OBJECT_ALLOCATION_FAILURE:      return "CL_MEM_OBJECT_ALLOCATION_FAILURE";
	case CL_OUT_OF_RESOURCES:                   return "CL_OUT_OF_RESOURCES";
	case CL_OUT_OF_HOST_MEMORY:                 return "CL_OUT_OF_HOST_MEMORY";
	case CL_PROFILING_INFO_NOT_AVAILABLE:       return "CL_PROFILING_INFO_NOT_AVAILABLE";
	case CL_MEM_COPY_OVERLAP:                   return "CL_MEM_COPY_OVERLAP";
	case CL_IMAGE_FORMAT_MISMATCH:              return "CL_IMAGE_FORMAT_MISMATCH";
	case CL_IMAGE_FORMAT_NOT_SUPPORTED:         return "CL_IMAGE_FORMAT_NOT_SUPPORTED";
	case CL_BUILD_PROGRAM_FAILURE:              return "CL_BUILD_PROGRAM_FAILURE";
	case CL_MAP_FAILURE:                        return "CL_MAP_FAILURE";
	case CL_MISALIGNED_SUB_BUFFER_OFFSET:       return "CL_MISALIGNED_SUB_BUFFER_OFFSET";                          //-13
	case CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST:    return "CL_EXEC_STATUS_ERROR_FOR_EVENTS_IN_WAIT_LIST";   //-14
	case CL_COMPILE_PROGRAM_FAILURE:            return "CL_COMPILE_PROGRAM_FAILURE";                               //-15
	case CL_LINKER_NOT_AVAILABLE:               return "CL_LINKER_NOT_AVAILABLE";                                  //-16
	case CL_LINK_PROGRAM_FAILURE:               return "CL_LINK_PROGRAM_FAILURE";                                  //-17
	case CL_DEVICE_PARTITION_FAILED:            return "CL_DEVICE_PARTITION_FAILED";                               //-18
	case CL_KERNEL_ARG_INFO_NOT_AVAILABLE:      return "CL_KERNEL_ARG_INFO_NOT_AVAILABLE";                         //-19
	case CL_INVALID_VALUE:                      return "CL_INVALID_VALUE";
	case CL_INVALID_DEVICE_TYPE:                return "CL_INVALID_DEVICE_TYPE";
	case CL_INVALID_PLATFORM:                   return "CL_INVALID_PLATFORM";
	case CL_INVALID_DEVICE:                     return "CL_INVALID_DEVICE";
	case CL_INVALID_CONTEXT:                    return "CL_INVALID_CONTEXT";
	case CL_INVALID_QUEUE_PROPERTIES:           return "CL_INVALID_QUEUE_PROPERTIES";
	case CL_INVALID_COMMAND_QUEUE:              return "CL_INVALID_COMMAND_QUEUE";
	case CL_INVALID_HOST_PTR:                   return "CL_INVALID_HOST_PTR";
	case CL_INVALID_MEM_OBJECT:                 return "CL_INVALID_MEM_OBJECT";
	case CL_INVALID_IMAGE_FORMAT_DESCRIPTOR:    return "CL_INVALID_IMAGE_FORMAT_DESCRIPTOR";
	case CL_INVALID_IMAGE_SIZE:                 return "CL_INVALID_IMAGE_SIZE";
	case CL_INVALID_SAMPLER:                    return "CL_INVALID_SAMPLER";
	case CL_INVALID_BINARY:                     return "CL_INVALID_BINARY";
	case CL_INVALID_BUILD_OPTIONS:              return "CL_INVALID_BUILD_OPTIONS";
	case CL_INVALID_PROGRAM:                    return "CL_INVALID_PROGRAM";
	case CL_INVALID_PROGRAM_EXECUTABLE:         return "CL_INVALID_PROGRAM_EXECUTABLE";
	case CL_INVALID_KERNEL_NAME:                return "CL_INVALID_KERNEL_NAME";
	case CL_INVALID_KERNEL_DEFINITION:          return "CL_INVALID_KERNEL_DEFINITION";
	case CL_INVALID_KERNEL:                     return "CL_INVALID_KERNEL";
	case CL_INVALID_ARG_INDEX:                  return "CL_INVALID_ARG_INDEX";
	case CL_INVALID_ARG_VALUE:                  return "CL_INVALID_ARG_VALUE";
	case CL_INVALID_ARG_SIZE:                   return "CL_INVALID_ARG_SIZE";
	case CL_INVALID_KERNEL_ARGS:                return "CL_INVALID_KERNEL_ARGS";
	case CL_INVALID_WORK_DIMENSION:             return "CL_INVALID_WORK_DIMENSION";
	case CL_INVALID_WORK_GROUP_SIZE:            return "CL_INVALID_WORK_GROUP_SIZE";
	case CL_INVALID_WORK_ITEM_SIZE:             return "CL_INVALID_WORK_ITEM_SIZE";
	case CL_INVALID_GLOBAL_OFFSET:              return "CL_INVALID_GLOBAL_OFFSET";
	case CL_INVALID_EVENT_WAIT_LIST:            return "CL_INVALID_EVENT_WAIT_LIST";
	case CL_INVALID_EVENT:                      return "CL_INVALID_EVENT";
	case CL_INVALID_OPERATION:                  return "CL_INVALID_OPERATION";
	case CL_INVALID_GL_OBJECT:                  return "CL_INVALID_GL_OBJECT";
	case CL_INVALID_BUFFER_SIZE:                return "CL_INVALID_BUFFER_SIZE";
	case CL_INVALID_MIP_LEVEL:                  return "CL_INVALID_MIP_LEVEL";
	case CL_INVALID_GLOBAL_WORK_SIZE:           return "CL_INVALID_GLOBAL_WORK_SIZE";                           //-63
	case CL_INVALID_PROPERTY:                   return "CL_INVALID_PROPERTY";                                   //-64
	case CL_INVALID_IMAGE_DESCRIPTOR:           return "CL_INVALID_IMAGE_DESCRIPTOR";                           //-65
	case CL_INVALID_COMPILER_OPTIONS:           return "CL_INVALID_COMPILER_OPTIONS";                           //-66
	case CL_INVALID_LINKER_OPTIONS:             return "CL_INVALID_LINKER_OPTIONS";                             //-67
	case CL_INVALID_DEVICE_PARTITION_COUNT:     return "CL_INVALID_DEVICE_PARTITION_COUNT";                     //-68
	//case CL_INVALID_PIPE_SIZE:                  return "CL_INVALID_PIPE_SIZE";                                  //-69
	//case CL_INVALID_DEVICE_QUEUE:               return "CL_INVALID_DEVICE_QUEUE";                               //-70    

	default:
		return "UNKNOWN ERROR CODE";
	}
}


/* Convenient container for all OpenCL specific objects used in the sample
 *
 * It consists of two parts:
 *   - regular OpenCL objects which are used in almost each normal OpenCL applications
 *   - several OpenCL objects that are specific for this particular sample
 *
 * You collect all these objects in one structure for utility purposes
 * only, there is no OpenCL specific here: just to avoid global variables
 * and make passing all these arguments in functions easier.
 */


ocl_args_d_t::ocl_args_d_t() :
	context(NULL),
	device(NULL),
	commandQueue(NULL),
	program(NULL),
	kernel(NULL),
	platformVersion(OPENCL_VERSION_2_2),
	deviceVersion(OPENCL_VERSION_2_2),
	compilerVersion(OPENCL_VERSION_2_2),
	mnemonic_bits(NULL),
	mnemonic(NULL),
	seed(NULL),
	work(NULL)
{
}

/*
 * destructor - called only once
 * Release all OpenCL objects
 * This is a regular sequence of calls to deallocate all created OpenCL resources in bootstrapOpenCL.
 *
 * You may want to call these deallocation procedures in the middle of your application execution
 * (not at the end) if you don't further need OpenCL runtime.
 * You may want to do that in order to free some memory, for example,
 * or recreate OpenCL objects with different parameters.
 *
 */
ocl_args_d_t::~ocl_args_d_t()
{
	cl_int err = CL_SUCCESS;

	if (kernel)
	{
		err = clReleaseKernel(kernel);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseKernel returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (program)
	{
		err = clReleaseProgram(program);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseProgram returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (mnemonic_bits)
	{
		err = clReleaseMemObject(mnemonic_bits);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (mnemonic)
	{
		err = clReleaseMemObject(mnemonic);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (seed)
	{
		err = clReleaseMemObject(seed);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (work)
	{
		err = clReleaseMemObject(work);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseMemObject returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (commandQueue)
	{
		err = clReleaseCommandQueue(commandQueue);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseCommandQueue returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (device)
	{
		err = clReleaseDevice(device);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseDevice returned '%s'.\n", TranslateOpenCLError(err));
		}
	}
	if (context)
	{
		err = clReleaseContext(context);
		if (err != CL_SUCCESS)
		{
			LogError("Error: clReleaseContext returned '%s'.\n", TranslateOpenCLError(err));
		}
	}

	/*
	 * Note there is no procedure to deallocate platform
	 * because it was not created at the startup,
	 * but just queried from OpenCL runtime.
	 */
}


/*
 * Check whether an OpenCL platform is the required platform
 * (based on the platform's name)
 */
bool CheckPreferredPlatformMatch(cl_platform_id platform, const char* preferredPlatform)
{
	size_t stringLength = 0;
	cl_int err = CL_SUCCESS;
	bool match = false;

	// In order to read the platform's name, we first read the platform's name string length (param_value is NULL).
	// The value returned in stringLength
	err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, 0, NULL, &stringLength);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetPlatformInfo() to get CL_PLATFORM_NAME length returned '%s'.\n", TranslateOpenCLError(err));
		return false;
	}

	// Now, that we know the platform's name string length, we can allocate enough space before read it
	std::vector<char> platformName(stringLength);

	// Read the platform's name string
	// The read value returned in platformName
	err = clGetPlatformInfo(platform, CL_PLATFORM_NAME, stringLength, &platformName[0], NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetplatform_ids() to get CL_PLATFORM_NAME returned %s.\n", TranslateOpenCLError(err));
		return false;
	}

	// Now check if the platform's name is the required one
	if (strstr(&platformName[0], preferredPlatform) != 0)
	{
		// The checked platform is the one we're looking for
		LogInfo("Platform: %s\n", &platformName[0]);
		match = true;
	}

	return match;
}

/*
 * Find and return the preferred OpenCL platform
 * In case that preferredPlatform is NULL, the ID of the first discovered platform will be returned
 */
cl_platform_id FindOpenCLPlatform(const char* preferredPlatform, cl_device_type deviceType)
{
	cl_uint numPlatforms = 0;
	cl_int err = CL_SUCCESS;

	// Get (in numPlatforms) the number of OpenCL platforms available
	// No platform ID will be return, since platforms is NULL
	err = clGetPlatformIDs(0, NULL, &numPlatforms);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetplatform_ids() to get num platforms returned %s.\n", TranslateOpenCLError(err));
		return NULL;
	}
	LogInfo("Number of available platforms: %u\n", numPlatforms);

	if (0 == numPlatforms)
	{
		LogError("Error: No platforms found!\n");
		return NULL;
	}

	std::vector<cl_platform_id> platforms(numPlatforms);

	// Now, obtains a list of numPlatforms OpenCL platforms available
	// The list of platforms available will be returned in platforms
	err = clGetPlatformIDs(numPlatforms, &platforms[0], NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetplatform_ids() to get platforms returned %s.\n", TranslateOpenCLError(err));
		return NULL;
	}

	// Check if one of the available platform matches the preferred requirements
	for (cl_uint i = 0; i < numPlatforms; i++)
	{
		bool match = true;
		cl_uint numDevices = 0;

		// If the preferredPlatform is not NULL then check if platforms[i] is the required one
		// Otherwise, continue the check with platforms[i]
		if ((NULL != preferredPlatform) && (strlen(preferredPlatform) > 0))
		{
			// In case we're looking for a specific platform
			match = CheckPreferredPlatformMatch(platforms[i], preferredPlatform);
		}

		// match is true if the platform's name is the required one or don't care (NULL)
		if (match)
		{
			// Obtains the number of deviceType devices available on platform
			// When the function failed we expect numDevices to be zero.
			// We ignore the function return value since a non-zero error code
			// could happen if this platform doesn't support the specified device type.
			err = clGetDeviceIDs(platforms[i], deviceType, 0, NULL, &numDevices);
			if (err != CL_SUCCESS)
			{
				LogInfo("   Required device was not found on this platform.\n");
			}

			if (0 != numDevices)
			{
				// There is at list one device that answer the requirements
				LogInfo("   Required device was found.\n");
				cl_device_id* devices = (cl_device_id*)malloc(numDevices * sizeof(cl_device_id));
				clGetDeviceIDs(platforms[i], CL_DEVICE_TYPE_ALL, numDevices, devices, NULL);
				printf("%i Devices:\n", numDevices);
				size_t size;
				for (int d = 0; d < numDevices; d++) {
					clGetDeviceInfo(devices[d], CL_DEVICE_NAME, 0, NULL, &size);
					char* str = (char*)malloc(size);
					clGetDeviceInfo(devices[d], CL_DEVICE_NAME, size, str, NULL);
					printf(" DEVICE NAME: %s\n", str);
					free(str);
				}
				return platforms[i];
			}
		}
	}

	LogError("Error: Required device was not found on any platform.\n");
	return NULL;
}


/*
 * This function read the OpenCL platdorm and device versions
 * (using clGetxxxInfo API) and stores it in the ocl structure.
 * Later it will enable us to support both OpenCL 1.2 and 2.0 platforms and devices
 * in the same program.
 */
int GetPlatformAndDeviceVersion(cl_platform_id platformId, ocl_args_d_t* ocl)
{
	cl_int err = CL_SUCCESS;

	// Read the platform's version string length (param_value is NULL).
	// The value returned in stringLength
	size_t stringLength = 0;
	err = clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, 0, NULL, &stringLength);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetPlatformInfo() to get CL_PLATFORM_VERSION length returned '%s'.\n", TranslateOpenCLError(err));
		return err;
	}

	// Now, that we know the platform's version string length, we can allocate enough space before read it
	std::vector<char> platformVersion(stringLength);

	// Read the platform's version string
	// The read value returned in platformVersion
	err = clGetPlatformInfo(platformId, CL_PLATFORM_VERSION, stringLength, &platformVersion[0], NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetplatform_ids() to get CL_PLATFORM_VERSION returned %s.\n", TranslateOpenCLError(err));
		return err;
	}

	if (strstr(&platformVersion[0], "OpenCL 2.0") != NULL)
	{
		ocl->platformVersion = OPENCL_VERSION_2_0;
	}

	// Read the device's version string length (param_value is NULL).
	err = clGetDeviceInfo(ocl->device, CL_DEVICE_VERSION, 0, NULL, &stringLength);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetDeviceInfo() to get CL_DEVICE_VERSION length returned '%s'.\n", TranslateOpenCLError(err));
		return err;
	}

	// Now, that we know the device's version string length, we can allocate enough space before read it
	std::vector<char> deviceVersion(stringLength);

	// Read the device's version string
	// The read value returned in deviceVersion
	err = clGetDeviceInfo(ocl->device, CL_DEVICE_VERSION, stringLength, &deviceVersion[0], NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetDeviceInfo() to get CL_DEVICE_VERSION returned %s.\n", TranslateOpenCLError(err));
		return err;
	}

	if (strstr(&deviceVersion[0], "OpenCL 2.0") != NULL)
	{
		ocl->deviceVersion = OPENCL_VERSION_2_0;
	}

	// Read the device's OpenCL C version string length (param_value is NULL).
	err = clGetDeviceInfo(ocl->device, CL_DEVICE_OPENCL_C_VERSION, 0, NULL, &stringLength);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetDeviceInfo() to get CL_DEVICE_OPENCL_C_VERSION length returned '%s'.\n", TranslateOpenCLError(err));
		return err;
	}

	// Now, that we know the device's OpenCL C version string length, we can allocate enough space before read it
	std::vector<char> compilerVersion(stringLength);

	// Read the device's OpenCL C version string
	// The read value returned in compilerVersion
	err = clGetDeviceInfo(ocl->device, CL_DEVICE_OPENCL_C_VERSION, stringLength, &compilerVersion[0], NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetDeviceInfo() to get CL_DEVICE_OPENCL_C_VERSION returned %s.\n", TranslateOpenCLError(err));
		return err;
	}
	else if (strstr(&compilerVersion[0], "OpenCL C 2.0") != NULL)
	{
		ocl->compilerVersion = OPENCL_VERSION_2_0;
	}

	return err;
}


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
int SetupOpenCL(ocl_args_d_t* ocl, cl_device_type deviceType)
{
	// The following variable stores return codes for all OpenCL calls.
	cl_int err = CL_SUCCESS;

	// Query for all available OpenCL platforms on the system
	// Here you enumerate all platforms and pick one which name has preferredPlatform as a sub-string
	cl_platform_id platformId = FindOpenCLPlatform("", deviceType);
	if (NULL == platformId)
	{
		LogError("Error: Failed to find OpenCL platform.\n");
		return CL_INVALID_VALUE;
	}

	// Create context with device of specified type.
	// Required device type is passed as function argument deviceType.
	// So you may use this function to create context for any CPU or GPU OpenCL device.
	// The creation is synchronized (pfn_notify is NULL) and NULL user_data
	cl_context_properties contextProperties[] = { CL_CONTEXT_PLATFORM, (cl_context_properties)platformId, 0 };
	ocl->context = clCreateContextFromType(contextProperties, deviceType, NULL, NULL, &err);
	if ((CL_SUCCESS != err) || (NULL == ocl->context))
	{
		LogError("Couldn't create a context, clCreateContextFromType() returned '%s'.\n", TranslateOpenCLError(err));
		return err;
	}

	// Query for OpenCL device which was used for context creation
	err = clGetContextInfo(ocl->context, CL_CONTEXT_DEVICES, sizeof(cl_device_id), &ocl->device, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clGetContextInfo() to get list of devices returned %s.\n", TranslateOpenCLError(err));
		return err;
	}

	// Read the OpenCL platform's version and the device OpenCL and OpenCL C versions
	GetPlatformAndDeviceVersion(platformId, ocl);

	// Create command queue.
	// OpenCL kernels are enqueued for execution to a particular device through special objects called command queues.
	// Command queue guarantees some ordering between calls and other OpenCL commands.
	// Here you create a simple in-order OpenCL command queue that doesn't allow execution of two kernels in parallel on a target device.
#ifdef CL_VERSION_2_0
	if (OPENCL_VERSION_2_0 == ocl->deviceVersion)
	{
		const cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
		ocl->commandQueue = clCreateCommandQueueWithProperties(ocl->context, ocl->device, properties, &err);
	}
	else {
		// default behavior: OpenCL 1.2
		cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
		ocl->commandQueue = clCreateCommandQueue(ocl->context, ocl->device, properties, &err);
}
#else
	// default behavior: OpenCL 1.2
	cl_command_queue_properties properties = CL_QUEUE_PROFILING_ENABLE;
	ocl->commandQueue = clCreateCommandQueue(ocl->context, ocl->device, properties, &err);
#endif
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateCommandQueue() returned %s.\n", TranslateOpenCLError(err));
		return err;
	}

	return CL_SUCCESS;
}



extern const char* OpenCLSource[4];
/*
 * Create and build OpenCL program from its source code
 */
int CreateAndBuildProgram(ocl_args_d_t* ocl, const char* file)
{
	cl_int err = CL_SUCCESS;

	// Upload the OpenCL C source code from the input file to source
	// The size of the C program is returned in sourceSize
	//char* source[NUMBER_OF_FILES];
	//size_t src_sizes[NUMBER_OF_FILES];
	//err = LoadProgramSource(sourceFilesPath, NUMBER_OF_FILES, source, src_sizes);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: ReadSourceFromFile returned %s.\n", TranslateOpenCLError(err));
	//    goto Finish;
	//}
	//    // And now after you obtained a regular C string call clCreateProgramWithSource to create OpenCL program object.
	//ocl->program = clCreateProgramWithSource(ocl->context, NUMBER_OF_FILES, (const char**)source, src_sizes, &err);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: clCreateProgramWithSource returned %s.\n", TranslateOpenCLError(err));
	//    goto Finish;
	//}

	//char* source = NULL;
	//size_t src_size = 0;
	//err = ReadSourceFromFile("Template.cl", &source, &src_size);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: ReadSourceFromFile returned %s.\n", TranslateOpenCLError(err));
	//    goto Finish;
	//}
	//// And now after you obtained a regular C string call clCreateProgramWithSource to create OpenCL program object.
	//ocl->program = clCreateProgramWithSource(ocl->context, 1, (const char**)&source, &src_size, &err);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: clCreateProgramWithSource returned %s.\n", TranslateOpenCLError(err));
	//    goto Finish;
	//}

	// Create the program for all device. Use the first device as the
	// representative device (assuming all device are of the same type).
	//printf("Using IR: %s\n", binary_file.c_str());
	ocl->program = createProgramFromBinary(ocl->context, file, &ocl->device, 1);

	//ocl->program = clCreateProgramWithSource(ocl->context, 4, OpenCLSource, NULL, &err);
	//if (err != CL_SUCCESS)
	//{
	//	LogError("Error: clCreateProgramWithSource returned %s.\n", TranslateOpenCLError(err));
	//	goto Finish;
	//}


	// Build the program
	// During creation a program is not built. You need to explicitly call build function.
	// Here you just use create-build sequence,
	// but there are also other possibilities when program consist of several parts,
	// some of which are libraries, and you may want to consider using clCompileProgram and clLinkProgram as
	// alternatives.
	err = clBuildProgram(ocl->program, 1, &ocl->device, "", NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clBuildProgram() for source program returned %s.\n", TranslateOpenCLError(err));

		// In case of error print the build log to the standard output
		// First check the size of the log
		// Then allocate the memory and obtain the log from the program
		if (err == CL_BUILD_PROGRAM_FAILURE)
		{
			size_t log_size = 0;
			clGetProgramBuildInfo(ocl->program, ocl->device, CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

			std::vector<char> build_log(log_size);
			clGetProgramBuildInfo(ocl->program, ocl->device, CL_PROGRAM_BUILD_LOG, log_size, &build_log[0], NULL);

			LogError("Error happened during the build of OpenCL program.\nBuild log:%s", &build_log[0]);
		}
	}

Finish:

	//if (source)
	//{
	//    delete[] source;
	//    source = NULL;
	//}

	return err;
}


/*
 * Create OpenCL buffers from host memory
 * These buffers will be used later by the OpenCL kernel
 */
int CreateBufferArguments(ocl_args_d_t* ocl, cl_ulong* mnemonic_bits, cl_uchar* mnemonic, cl_uchar* seed, size_t mnemonic_bits_buf_size, size_t mnemonic_buf_size, size_t  seed_buf_size)
{
	cl_int err = CL_SUCCESS;

	// Create third (output) image based on host memory
	ocl->mnemonic_bits = clCreateBuffer(ocl->context, CL_MEM_READ_ONLY | CL_MEM_USE_HOST_PTR, mnemonic_bits_buf_size, mnemonic_bits, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateImage for mnemonic_bits returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	ocl->mnemonic = clCreateBuffer(ocl->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, mnemonic_buf_size, mnemonic, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateImage for mnemonic returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	ocl->seed = clCreateBuffer(ocl->context, CL_MEM_WRITE_ONLY | CL_MEM_USE_HOST_PTR, seed_buf_size, seed, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateImage for seed returned %s\n", TranslateOpenCLError(err));
		return err;
	}
	return CL_SUCCESS;
}


/*
 * Set kernel arguments
 */
cl_uint SetKernelArguments(ocl_args_d_t* ocl)
{
	cl_int err = CL_SUCCESS;

	err = clSetKernelArg(ocl->kernel, 0, sizeof(cl_mem), (void*)&ocl->mnemonic_bits);
	if (err != CL_SUCCESS)
	{
		LogError("error: Failed to set argument mnemonic_bits, returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	err = clSetKernelArg(ocl->kernel, 1, sizeof(cl_mem), (void*)&ocl->mnemonic);
	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to set argument mnemonic, returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	err = clSetKernelArg(ocl->kernel, 2, sizeof(cl_mem), (void*)&ocl->seed);
	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to set argument seed, returned %s\n", TranslateOpenCLError(err));
		return err;
	}

	//err = clSetKernelArg(ocl->kernel, 3, 256, NULL);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: Failed to set argument local, returned %s\n", TranslateOpenCLError(err));
	//    return err;
	//}
	return err;
}


/*
 * Execute the kernel
 */
cl_uint ExecuteKernel(ocl_args_d_t* ocl, cl_uint num)
{
	cl_int err = CL_SUCCESS;

	//size_t global, local;
	////   Get the maximum work group size for executing the kernel on the device
	//err = clGetKernelWorkGroupInfo(kernel, devices[currDevice], CL_KERNEL_WORK_GROUP_SIZE, sizeof(local), &local, NULL);
	//CheckError(err, "Failed to retrieve work group info!");

	////   Execute the kernel over the entire range of our 1d input data set
	////   using the maximum number of work group items for this device
	//global = DATA_SIZE;
	//err = clEnqueueNDRangeKernel(commandQueues[currDevice], kernel, 1, NULL, &global, &local, 0, NULL, NULL);
	//CheckError(err, "Failed to execute kernel!");


	size_t global_item_size = num;
	size_t local_item_size = 1;		//Trabalhar em grupos de 64 elementos
	// execute kernel
	err = clEnqueueNDRangeKernel(ocl->commandQueue, ocl->kernel, 1, NULL, &global_item_size, &local_item_size, 0, NULL, NULL);

	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to run kernel, return %s\n", TranslateOpenCLError(err));
		return err;
	}

	// Wait until the queued kernel is completed by the device
	err = clFinish(ocl->commandQueue);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clFinish return %s\n", TranslateOpenCLError(err));
		return err;
	}

	return CL_SUCCESS;
}


/*
 * "Read" the result buffer (mapping the buffer to the host memory address)
 */
bool ReadAndVerify(ocl_args_d_t* ocl, cl_uchar* mnemonic, cl_uchar* seed, size_t mnemonic_buf_size, size_t seed_buf_size)
{
	cl_int err = CL_SUCCESS;
	bool result = true;

	mnemonic = (cl_uchar*)clEnqueueMapBuffer(ocl->commandQueue, ocl->mnemonic, CL_TRUE, CL_MAP_READ, 0, mnemonic_buf_size, 0, NULL, NULL, &err);

	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueMapBuffer returned %s\n", TranslateOpenCLError(err));
		return false;
	}

	seed = (cl_uchar*)clEnqueueMapBuffer(ocl->commandQueue, ocl->seed, CL_TRUE, CL_MAP_READ, 0, seed_buf_size, 0, NULL, NULL, &err);

	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueMapBuffer returned %s\n", TranslateOpenCLError(err));
		return false;
	}

	// Call clFinish to guarantee that output region is updated
	err = clFinish(ocl->commandQueue);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clFinish returned %s\n", TranslateOpenCLError(err));
	}

	//printf("target_mnemonic_buf: %s\n", target_mnemonic_buf);
	//printf("mnemonic_found_buf: %d\n", *mnemonic_found_buf);

		// Unmapped the output buffer before releasing it
	//err = clEnqueueUnmapMemObject(ocl->commandQueue, ocl->mnemonic_bits, mnemonic_bits, 0, NULL, NULL);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: clEnqueueUnmapMemObject mnemonic_bits returned %s\n", TranslateOpenCLError(err));
	//}

	// Unmapped the output buffer before releasing it
	err = clEnqueueUnmapMemObject(ocl->commandQueue, ocl->mnemonic, mnemonic, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueUnmapMemObject mnemonic returned %s\n", TranslateOpenCLError(err));
	}

	// Unmapped the output buffer before releasing it
	err = clEnqueueUnmapMemObject(ocl->commandQueue, ocl->seed, seed, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueUnmapMemObject mnemonic_found_buf returned %s\n", TranslateOpenCLError(err));
	}

	return result;
}


int SetupOpenCLMY(ocl_args_d_t* ocl, cl_uint num_platform, cl_uint num_device)
{
	cl_int err = CL_SUCCESS;
	
	//platform
	cl_uint num_platforms = num_platform + 1;
	cl_platform_id* platforms = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id));
	clGetPlatformIDs(num_platforms, platforms, NULL);
	cl_platform_id platform = platforms[num_platform];
	//device
	cl_uint num_devices = num_device + 1;
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
	cl_device_id* devices = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));
	clGetDeviceIDs(platform, CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
	cl_device_id device = devices[num_device];

	ocl->device = device;
	ocl->context = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	if ((CL_SUCCESS != err) || (NULL == ocl->context))
	{
		LogError("Couldn't create a context, clCreateContextFromType() returned '%s'.\n", TranslateOpenCLError(err));
		return err;
	}
	// Create command queue.
	const cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
	ocl->commandQueue = clCreateCommandQueueWithProperties(ocl->context, ocl->device, properties, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateCommandQueueWithProperties() returned %s.\n", TranslateOpenCLError(err));
		return err;
	}
	printf("Platform \"%i\", Device \"%i\"\n", num_platform, num_device);
	return CL_SUCCESS;
}