#include <Crypto_Tools.h>
#include <OpenCL_Tools.h>
#include <AOCLUtils/opencl.h>


#include <stdio.h>
#include <vector>

using namespace aocl_utils;

crypto_args_t::crypto_args_t()
{
	for (int i = 0; i < CARDS_NUMBER; i++) {
		context[i] = NULL;
		device[i] = NULL;
		commandQueue[i] = NULL;
		program[i] = NULL;
		kernel[i] = NULL;
		platformVersion[i] = OPENCL_VERSION_2_2;
		deviceVersion[i] = OPENCL_VERSION_2_2;
		compilerVersion[i] = OPENCL_VERSION_2_2;
		mnemonic_bits[i] = NULL;
		mnemonic[i] = NULL;
		seed[i] = NULL;
		work[i] = NULL;
	}
}

crypto_args_t::~crypto_args_t()
{
	for (int i = 0; i < CARDS_NUMBER; i++) {
		cl_int err = CL_SUCCESS;

		if (kernel[i])
		{
			err = clReleaseKernel(kernel[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseKernel returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (program[i])
		{
			err = clReleaseProgram(program[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseProgram program returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (mnemonic_bits[i])
		{
			err = clReleaseMemObject(mnemonic_bits[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseMemObject mnemonic_bits returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (mnemonic[i])
		{
			err = clReleaseMemObject(mnemonic[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseMemObject mnemonic returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (seed[i])
		{
			err = clReleaseMemObject(seed[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseMemObject seed returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (work[i])
		{
			err = clReleaseMemObject(work[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseMemObject work returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (commandQueue[i])
		{
			err = clReleaseCommandQueue(commandQueue[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseCommandQueue returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (device[i])
		{
			err = clReleaseDevice(device[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseDevice returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
		if (context[i])
		{
			err = clReleaseContext(context[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clReleaseContext returned '%s'.\n", TranslateOpenCLError(err));
			}
		}
	}

}

cl_int ReleaseMemObject(crypto_args_t* ocl, cl_int buff_name, cl_int buff_num) {
	cl_int err;
	cl_mem* buff;
	char* name_str;
	switch (buff_name)
	{
	case CRYPTO_BUFF_MNEMONIC_BITS:
		buff = &ocl->mnemonic_bits[buff_num];
		name_str = "mnemonic_bits";
		break;

	case CRYPTO_BUFF_MNEMONIC:
		buff = &ocl->mnemonic[buff_num];
		name_str = "mnemonic";
		break;

	case CRYPTO_BUFF_SEED:
		buff = &ocl->seed[buff_num];
		name_str = "seed";
		break;

	case CRYPTO_BUFF_WORK:
		buff = &ocl->work[buff_num];
		name_str = "work";
		break;

	default:
		LogError("Error: CreateBuffer() invalid buff_name\n");
		return -1;
		break;
	}

	err = clReleaseMemObject(*buff);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for %s returned %s\n", name_str, TranslateOpenCLError(err));
		return err;
	}

	return err;
}


cl_int CreateBuffer(crypto_args_t* ocl, cl_int buff_name, cl_int buff_num, cl_mem_flags flags, void* buf) {
	cl_int err;
	size_t size;
	cl_mem* dst;
	char* name_str;
	switch (buff_name)
	{
	case CRYPTO_BUFF_MNEMONIC_BITS:
		size = SIZE_MNEMONIC_BITS_BUF;
		dst = &ocl->mnemonic_bits[buff_num];
		name_str = "mnemonic_bits";
		break;

	case CRYPTO_BUFF_MNEMONIC:
		size = SIZE_MNEMONIC_BUF;
		dst = &ocl->mnemonic[buff_num];
		name_str = "mnemonic";
		break;

	case CRYPTO_BUFF_SEED:
		size = SIZE_SEED_BUF;
		dst = &ocl->seed[buff_num];
		name_str = "seed";
		break;

	case CRYPTO_BUFF_WORK:
		size = SIZE_WORK_BUF;
		dst = &ocl->work[buff_num];
		name_str = "work";
		break;

	default:
		LogError("Error: CreateBuffer() invalid buff_name\n");
		return -1;
		break;
	}

	*dst = clCreateBuffer(ocl->context[buff_num], flags, size, buf, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for %s returned %s\n", name_str, TranslateOpenCLError(err));
		return -1;
	}

	return 0;
}


cl_int SetKernelArg(crypto_args_t* ocl, cl_int buff_name, cl_int buff_num, cl_int arg_num) {
	cl_int err;
	size_t size;
	cl_mem* src;
	char* name_str;
	switch (buff_name)
	{
	case CRYPTO_BUFF_MNEMONIC_BITS:
		size = SIZE_MNEMONIC_BITS_BUF;
		src = &ocl->mnemonic_bits[buff_num];
		name_str = "mnemonic_bits";
		break;

	case CRYPTO_BUFF_MNEMONIC:
		size = SIZE_MNEMONIC_BUF;
		src = &ocl->mnemonic[buff_num];
		name_str = "mnemonic";
		break;

	case CRYPTO_BUFF_SEED:
		size = SIZE_SEED_BUF;
		src = &ocl->seed[buff_num];
		name_str = "seed";
		break;

	case CRYPTO_BUFF_WORK:
		size = SIZE_WORK_BUF;
		src = &ocl->work[buff_num];
		name_str = "work";
		break;

	default:
		LogError("Error: CreateBuffer() invalid buff_name\n");
		return -1;
		break;
	}

	err = clSetKernelArg(ocl->kernel[buff_num], arg_num, sizeof(cl_mem), (void*)src);
	if (err != CL_SUCCESS)
	{
		LogError("error: Failed to set argument %s, returned %s\n", name_str, TranslateOpenCLError(err));
		return -1;
	}

	return 0;
}

int SetupOpenCLCrypto(crypto_args_t* ocl, cl_uint num, cl_uint num_platform, cl_uint num_device)
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

	ocl->device[num] = device;
	ocl->context[num] = clCreateContext(NULL, 1, &device, NULL, NULL, &err);
	if ((CL_SUCCESS != err) || (NULL == ocl->context[num]))
	{
		LogError("Couldn't create a context, clCreateContextFromType() returned '%s'.\n", TranslateOpenCLError(err));
		return err;
	}
	// Create command queue.
	const cl_command_queue_properties properties[] = { CL_QUEUE_PROPERTIES, CL_QUEUE_PROFILING_ENABLE, 0 };
	ocl->commandQueue[num] = clCreateCommandQueueWithProperties(ocl->context[num], ocl->device[num], properties, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateCommandQueueWithProperties() returned %s.\n", TranslateOpenCLError(err));
		return err;
	}
	printf("Platform \"%i\", Device \"%i\"\n", num_platform, num_device);
	return CL_SUCCESS;
}


extern const char* OpenCLSource[4];
#define NUMBER_OF_FILES 1
int CreateAndBuildProgramCrypto(crypto_args_t* ocl, cl_uint num, const char* file)
{
	cl_int err = CL_SUCCESS;

	// Create the program for all device. Use the first device as the
	// representative device (assuming all device are of the same type).
	//printf("Using IR: %s\n", binary_file.c_str());

	// //Upload the OpenCL C source code from the input file to source
	// //The size of the C program is returned in sourceSize
	//char* source[NUMBER_OF_FILES];
	//size_t src_sizes[NUMBER_OF_FILES];
	//err = LoadProgramSource(&file, NUMBER_OF_FILES, source, src_sizes);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: ReadSourceFromFile returned %s.\n", TranslateOpenCLError(err));
	//    goto Finish;
	//}
	//    // And now after you obtained a regular C string call clCreateProgramWithSource to create OpenCL program object.
	//ocl->program[num] = clCreateProgramWithSource(ocl->context[num], NUMBER_OF_FILES, (const char**)source, src_sizes, &err);
	//if (err != CL_SUCCESS)
	//{
	//    LogError("Error: clCreateProgramWithSource returned %s.\n", TranslateOpenCLError(err));
	//    goto Finish;
	//}


	//ocl->program[num] = createProgramFromBinary(ocl->context[num], file, &ocl->device[num], 1);
	//if (ocl->program[num] == NULL)
	//{
	//	LogError("Error: createProgramFromBinary.\n");
	//	goto Finish;
	//}


	ocl->program[num] = clCreateProgramWithSource(ocl->context[num], 4, OpenCLSource, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateProgramWithSource returned %s.\n", TranslateOpenCLError(err));
		goto Finish;
	}


	// Build the program
	// During creation a program is not built. You need to explicitly call build function.
	// Here you just use create-build sequence,
	// but there are also other possibilities when program consist of several parts,
	// some of which are libraries, and you may want to consider using clCompileProgram and clLinkProgram as
	// alternatives.
	err = clBuildProgram(ocl->program[num], 1, &ocl->device[num], "", NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clBuildProgram() for source program returned %s.\n", TranslateOpenCLError(err));

		// In case of error print the build log to the standard output
		// First check the size of the log
		// Then allocate the memory and obtain the log from the program
		if (err == CL_BUILD_PROGRAM_FAILURE)
		{
			size_t log_size = 0;
			clGetProgramBuildInfo(ocl->program[num], ocl->device[num], CL_PROGRAM_BUILD_LOG, 0, NULL, &log_size);

			std::vector<char> build_log(log_size);
			clGetProgramBuildInfo(ocl->program[num], ocl->device[num], CL_PROGRAM_BUILD_LOG, log_size, &build_log[0], NULL);

			LogError("Error happened during the build of OpenCL program.\nBuild log:%s", &build_log[0]);
		}
	}

Finish:
	//for (int i = 0; i < NUMBER_OF_FILES; i++) {
	//	if (source[i])
	//	{
	//		delete[] source[i];
	//		source[i] = NULL;
	//	}
	//}


	return err;
}
