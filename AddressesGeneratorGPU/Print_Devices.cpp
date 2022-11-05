/* Print some OpenCL parameters of all available devices.
 *
 * Compile with
 *   cc -o queryocl queryocl.c -lOpenCL
 * eventually you need to pass -I and -L options.
 *
 * Then call with
 *   ./queryocl
 */

 /* This is free and unencumbered software released into the public domain.
  *
  * Anyone is free to copy, modify, publish, use, compile, sell, or
  * distribute this software, either in source code form or as a compiled
  * binary, for any purpose, commercial or non-commercial, and by any
  * means.
  *
  * In jurisdictions that recognize copyright laws, the author or authors
  * of this software dedicate any and all copyright interest in the
  * software to the public domain. We make this dedication for the benefit
  * of the public at large and to the detriment of our heirs and
  * successors. We intend this dedication to be an overt act of
  * relinquishment in perpetuity of all present and future rights to this
  * software under copyright law.
  *
  * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
  * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
  * MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.
  * IN NO EVENT SHALL THE AUTHORS BE LIABLE FOR ANY CLAIM, DAMAGES OR
  * OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE,
  * ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
  * OTHER DEALINGS IN THE SOFTWARE.
  *
  * For more information, please refer to <http://unlicense.org> */

#include <CL/cl.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
using namespace std;

#define PF_STRING(name)	\
	clGetPlatformInfo(platforms[p], name, 0, NULL, &size);	\
	str = (char*)malloc(size);					\
	clGetPlatformInfo(platforms[p], name, size, str, NULL);	\
	printf("%22s: %s\n", #name, str);			\
	free(str)

#define DEV_STRING(name)	\
	clGetDeviceInfo(devices[d], name, 0, NULL, &size);	\
	str = (char*)malloc(size);					\
	clGetDeviceInfo(devices[d], name, size, str, NULL);	\
	printf("%39s: %s\n", #name, str);			\
	free(str)

#define DEV_UINT(name)	\
	clGetDeviceInfo(devices[d], name, sizeof(cl_uint), &ui, NULL);	\
	printf("%39s: %i\n", #name, ui)

#define DEV_ULONG(name)	\
	clGetDeviceInfo(devices[d], name, sizeof(cl_ulong), &ul, NULL);	\
	printf("%39s: %I64d\n", #name, ul)

#define DEV_BOOL(name)	\
	clGetDeviceInfo(devices[d], name, sizeof(cl_bool), &bl, NULL);	\
	printf("%39s: %s\n", #name, bl ? "yes" : "no")

int Print_Parameters_of_All_Available_Devices(void)
{

	unsigned int p, d;
	cl_uint num_platforms, num_devices;
	cl_platform_id* platforms;
	cl_device_id* devices;
	size_t size;
	char* str;
	cl_uint ui;
	cl_ulong ul;
	cl_bool bl;
	cl_device_type dt;
	cl_device_fp_config fc;
	cl_device_mem_cache_type mct;
	cl_device_local_mem_type lmt;
	cl_device_exec_capabilities ec;
	cl_command_queue_properties cqp;
	size_t* sa;
	unsigned int i;

	clGetPlatformIDs(0, NULL, &num_platforms);
	platforms = (cl_platform_id*)malloc(num_platforms * sizeof(cl_platform_id));
	clGetPlatformIDs(num_platforms, platforms, NULL);
	printf("\n");
	printf("%i Platforms:\n", num_platforms);



	for (p = 0; p < num_platforms; p++) {
		printf("******************************************************************\n");
		printf("******************************************************************\n");
		printf("******************************************************************\n");
		printf("Platform \"%i\":\n", p);
		PF_STRING(CL_PLATFORM_PROFILE);
		PF_STRING(CL_PLATFORM_VERSION);
		PF_STRING(CL_PLATFORM_NAME);
		PF_STRING(CL_PLATFORM_VENDOR);
		//PF_STRING(CL_PLATFORM_EXTENSIONS);
		clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, 0, NULL, &num_devices);
		devices = (cl_device_id*)malloc(num_devices * sizeof(cl_device_id));
		clGetDeviceIDs(platforms[p], CL_DEVICE_TYPE_ALL, num_devices, devices, NULL);
		printf("%i Devices:\n", num_devices);

		for (d = 0; d < num_devices; d++) {
			printf("Device \"%i.%i\":\n", p, d);
			clGetDeviceInfo(devices[d], CL_DEVICE_TYPE,
				sizeof(cl_device_type), &dt, NULL);
			printf("%39s:%s%s%s%s\n", "CL_DEVICE_TYPE",
				dt & CL_DEVICE_TYPE_CPU ? " CPU" : "",
				dt & CL_DEVICE_TYPE_GPU ? " GPU" : "",
				dt & CL_DEVICE_TYPE_ACCELERATOR ? " ACCELERATOR" : "",
				dt & CL_DEVICE_TYPE_DEFAULT ? " DEFAULT" : "");
			DEV_UINT(CL_DEVICE_VENDOR_ID);
			DEV_UINT(CL_DEVICE_MAX_COMPUTE_UNITS);
			DEV_UINT(CL_DEVICE_MAX_WORK_ITEM_DIMENSIONS);
			clGetDeviceInfo(devices[d], CL_DEVICE_MAX_WORK_ITEM_SIZES, 0, NULL, &size);
			sa = (size_t*)malloc(size);
			clGetDeviceInfo(devices[d], CL_DEVICE_MAX_WORK_ITEM_SIZES, size, sa, NULL);
			printf("%39s:", "CL_DEVICE_MAX_WORK_ITEM_SIZES");
			for (i = 0; i < size / sizeof(size_t); i++)
				printf(" %i", (int)sa[i]);
			printf("\n");
			DEV_UINT(CL_DEVICE_MAX_WORK_GROUP_SIZE);
			DEV_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_CHAR);
			DEV_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_SHORT);
			DEV_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_INT);
			DEV_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_LONG);
			DEV_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_HALF);
			DEV_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_FLOAT);
			DEV_UINT(CL_DEVICE_PREFERRED_VECTOR_WIDTH_DOUBLE);
			DEV_UINT(CL_DEVICE_NATIVE_VECTOR_WIDTH_CHAR);
			DEV_UINT(CL_DEVICE_NATIVE_VECTOR_WIDTH_SHORT);
			DEV_UINT(CL_DEVICE_NATIVE_VECTOR_WIDTH_INT);
			DEV_UINT(CL_DEVICE_NATIVE_VECTOR_WIDTH_LONG);
			DEV_UINT(CL_DEVICE_NATIVE_VECTOR_WIDTH_HALF);
			DEV_UINT(CL_DEVICE_NATIVE_VECTOR_WIDTH_FLOAT);
			DEV_UINT(CL_DEVICE_NATIVE_VECTOR_WIDTH_DOUBLE);
			DEV_UINT(CL_DEVICE_MAX_CLOCK_FREQUENCY);
			DEV_UINT(CL_DEVICE_ADDRESS_BITS);
			DEV_ULONG(CL_DEVICE_MAX_MEM_ALLOC_SIZE);
			DEV_BOOL(CL_DEVICE_IMAGE_SUPPORT);
			DEV_UINT(CL_DEVICE_MAX_READ_IMAGE_ARGS);
			DEV_UINT(CL_DEVICE_MAX_WRITE_IMAGE_ARGS);
			DEV_UINT(CL_DEVICE_IMAGE2D_MAX_WIDTH);
			DEV_UINT(CL_DEVICE_IMAGE2D_MAX_HEIGHT);
			DEV_UINT(CL_DEVICE_IMAGE3D_MAX_WIDTH);
			DEV_UINT(CL_DEVICE_IMAGE3D_MAX_HEIGHT);
			DEV_UINT(CL_DEVICE_IMAGE3D_MAX_DEPTH);
			DEV_UINT(CL_DEVICE_MAX_SAMPLERS);
			DEV_UINT(CL_DEVICE_MAX_PARAMETER_SIZE);
			DEV_UINT(CL_DEVICE_MEM_BASE_ADDR_ALIGN);
			DEV_UINT(CL_DEVICE_MIN_DATA_TYPE_ALIGN_SIZE);
			clGetDeviceInfo(devices[d], CL_DEVICE_SINGLE_FP_CONFIG,
				sizeof(cl_device_fp_config), &fc, NULL);
			printf("%39s:%s%s%s%s%s%s\n", "CL_DEVICE_SINGLE_FP_CONFIG",
				fc & CL_FP_DENORM ? " DENORM" : "",
				fc & CL_FP_INF_NAN ? " INF_NAN" : "",
				fc & CL_FP_ROUND_TO_NEAREST ? " ROUND_TO_NEAREST" : "",
				fc & CL_FP_ROUND_TO_ZERO ? " ROUND_TO_ZERO" : "",
				fc & CL_FP_ROUND_TO_INF ? " ROUND_TO_INF" : "",
				fc & CL_FP_FMA ? " FMA" : "");
#ifdef CL_DEVICE_DOUBLE_FP_CONFIG
			clGetDeviceInfo(devices[d], CL_DEVICE_DOUBLE_FP_CONFIG,
				sizeof(cl_device_fp_config), &fc, NULL);
			printf("%39s:%s%s%s%s%s%s\n", "CL_DEVICE_DOUBLE_FP_CONFIG",
				fc & CL_FP_DENORM ? " DENORM" : "",
				fc & CL_FP_INF_NAN ? " INF_NAN" : "",
				fc & CL_FP_ROUND_TO_NEAREST ? " ROUND_TO_NEAREST" : "",
				fc & CL_FP_ROUND_TO_ZERO ? " ROUND_TO_ZERO" : "",
				fc & CL_FP_ROUND_TO_INF ? " ROUND_TO_INF" : "",
				fc & CL_FP_FMA ? " FMA" : "");
#endif
			clGetDeviceInfo(devices[d], CL_DEVICE_GLOBAL_MEM_CACHE_TYPE,
				sizeof(cl_device_mem_cache_type), &mct, NULL);
			printf("%39s: %s\n", "CL_DEVICE_GLOBAL_MEM_CACHE_TYPE",
				mct == CL_NONE ? "NONE" :
				mct == CL_READ_ONLY_CACHE ? "READ_ONLY_CACHE" :
				mct == CL_READ_WRITE_CACHE ? "READ_WRITE_CACHE" : "?");
			DEV_UINT(CL_DEVICE_GLOBAL_MEM_CACHELINE_SIZE);
			DEV_ULONG(CL_DEVICE_GLOBAL_MEM_CACHE_SIZE);
			DEV_ULONG(CL_DEVICE_GLOBAL_MEM_SIZE);
			DEV_ULONG(CL_DEVICE_MAX_CONSTANT_BUFFER_SIZE);
			DEV_UINT(CL_DEVICE_MAX_CONSTANT_ARGS);
			clGetDeviceInfo(devices[d], CL_DEVICE_LOCAL_MEM_TYPE,
				sizeof(cl_device_local_mem_type), &lmt, NULL);
			printf("%39s: %s\n", "CL_DEVICE_LOCAL_MEM_TYPE",
				lmt == CL_LOCAL ? "LOCAL" :
				lmt == CL_GLOBAL ? "GLOBAL" : "?");
			DEV_ULONG(CL_DEVICE_LOCAL_MEM_SIZE);
			DEV_BOOL(CL_DEVICE_ERROR_CORRECTION_SUPPORT);
			DEV_BOOL(CL_DEVICE_HOST_UNIFIED_MEMORY);
			DEV_UINT(CL_DEVICE_PROFILING_TIMER_RESOLUTION);
			DEV_BOOL(CL_DEVICE_ENDIAN_LITTLE);
			DEV_BOOL(CL_DEVICE_AVAILABLE);
			DEV_BOOL(CL_DEVICE_COMPILER_AVAILABLE);
			clGetDeviceInfo(devices[d], CL_DEVICE_EXECUTION_CAPABILITIES,
				sizeof(cl_device_exec_capabilities), &ec, NULL);
			printf("%39s:%s%s\n", "CL_DEVICE_EXECUTION_CAPABILITIES",
				ec & CL_EXEC_KERNEL ? " KERNEL" : "",
				ec & CL_EXEC_NATIVE_KERNEL ? " NATIVE_KERNEL" : "");
			clGetDeviceInfo(devices[d], CL_DEVICE_QUEUE_PROPERTIES,
				sizeof(cl_command_queue_properties), &cqp, NULL);
			printf("%39s:%s%s\n", "CL_DEVICE_QUEUE_PROPERTIES",
				cqp & CL_QUEUE_OUT_OF_ORDER_EXEC_MODE_ENABLE ? " OUT_OF_ORDER_EXEC_MODE_ENABLE" : "",
				cqp & CL_QUEUE_PROFILING_ENABLE ? " PROFILING_ENABLE" : "");
			DEV_STRING(CL_DEVICE_PROFILE);
			DEV_STRING(CL_DEVICE_NAME);
			DEV_STRING(CL_DEVICE_VENDOR);
			DEV_STRING(CL_DEVICE_VERSION);
			//DEV_STRING(CL_DEVICE_EXTENSIONS);
		}

		printf("\n");
		free(devices);
	}

	printf("******************************************************************\n");
	printf("******************************************************************\n");
	printf("******************************************************************\n");
	printf("******************************************************************\n");
	printf("******************************************************************\n");
	printf("******************************************************************\n");
	free(platforms);

	return 0;
}


int Print_Name_Device(cl_device_id* device)
{

	unsigned int d = 0;
	size_t size;
	char* str;
	cl_device_id* devices = device;
	DEV_STRING(CL_DEVICE_NAME);
	DEV_STRING(CL_DEVICE_VENDOR);
	DEV_STRING(CL_DEVICE_VERSION);

	return 0;
}