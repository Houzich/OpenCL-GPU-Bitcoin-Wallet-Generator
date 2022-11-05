#pragma once
#include "CL\cl.h"
#include <OpenCL_Tools.h>

//#define SERVER_FOR_WORK

#ifdef SERVER_FOR_WORK
#define NUM_THREADS_CHECK				4
#define NUM_THREADS_SQLITE				4
#define NUM_PLATFORM					1
#define NUM_ADDRESSES					1024*5
#define CARDS_NUMBER					3
#else
#define NUM_THREADS_CHECK				4
#define NUM_THREADS_SQLITE				4
#define NUM_PLATFORM					0
#define NUM_ADDRESSES					1024*5
#define CARDS_NUMBER					1
#endif

#define CRYPTO_BUFF_MNEMONIC_BITS		1
#define CRYPTO_BUFF_MNEMONIC			2
#define CRYPTO_BUFF_SEED				3
#define CRYPTO_BUFF_WORK				4

#define WORK_BUFF_SIZE (128 + 128 + 256 + 64)
#define MNEMONIC_BITS_BUF_SIZE_TEMP  (sizeof(cl_ulong) * 2 * NUM_ADDRESSES)
#define MNEMONIC_BUF_SIZE_TEMP  (120 * NUM_ADDRESSES)
#define SEED_BUF_SIZE_TEMP  (64 * NUM_ADDRESSES)
#define WORK_BUF_SIZE_TEMP  (WORK_BUFF_SIZE * NUM_ADDRESSES)
#define SIZE_MNEMONIC_BITS_BUF  (((MNEMONIC_BITS_BUF_SIZE_TEMP - 1) / 64 + 1) * 64)
#define SIZE_MNEMONIC_BUF  (((MNEMONIC_BUF_SIZE_TEMP - 1) / 64 + 1) * 64)
#define SIZE_SEED_BUF  (((SEED_BUF_SIZE_TEMP - 1) / 64 + 1) * 64)
#define SIZE_WORK_BUF  (((WORK_BUF_SIZE_TEMP - 1) / 64 + 1) * 64)


struct crypto_args_t
{
	crypto_args_t();
	~crypto_args_t();

	// Regular OpenCL objects:
	cl_context       context[CARDS_NUMBER];           // hold the context handler
	cl_device_id     device[CARDS_NUMBER];            // hold the selected device handler
	cl_command_queue commandQueue[CARDS_NUMBER];      // hold the commands-queue handler
	cl_program       program[CARDS_NUMBER];           // hold the program handler
	cl_kernel        kernel[CARDS_NUMBER];            // hold the kernel handler
	float            platformVersion[CARDS_NUMBER];   // hold the OpenCL platform version (default 2.0)
	float            deviceVersion[CARDS_NUMBER];     // hold the OpenCL device version (default. 2.0)
	float            compilerVersion[CARDS_NUMBER];   // hold the device OpenCL C version (default. 2.0)

	// Objects that are specific for algorithm implemented in this sample
	cl_mem          mnemonic_bits[CARDS_NUMBER];
	cl_mem          mnemonic[CARDS_NUMBER];
	cl_mem          seed[CARDS_NUMBER];
	cl_mem          work[CARDS_NUMBER];
};





cl_int CreateBuffer(crypto_args_t* ocl, cl_int buff_name, cl_int buff_num, cl_mem_flags flags, void* buf);
cl_int SetKernelArg(crypto_args_t* ocl, cl_int buff_name, cl_int buff_num, cl_int arg_num);
int SetupOpenCLCrypto(crypto_args_t* ocl, cl_uint num, cl_uint num_platform, cl_uint num_device);
int CreateAndBuildProgramCrypto(crypto_args_t* ocl, cl_uint num, const char* file);
cl_int ReleaseMemObject(crypto_args_t* ocl, cl_int buff_name, cl_int buff_num);