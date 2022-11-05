/*****************************************************************************
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <vector>
#include <OpenCL_Tools.h>
#include <Crypto.h>
#include <BIP0032/bip0032.h>
#include <Tools/Addresses_Generate_CPU.h>
#include <fstream>
#include <Crypto_Tools.h>
#include "sqlite_tools.h"
#include <iostream>

void Get_Derived_Addressses(uint8_t* hash160_bip44_buf, uint8_t* seeds);
void Generate_Bytes(uint8_t* buf, size_t len);


void Clear_File(void);


int Print_Parameters_of_All_Available_Devices(void);





int Test2(void)
{
	cl_int err;
	ocl_args_d_t ocl;
	cl_device_type deviceType = CL_DEVICE_TYPE_GPU;
	Print_Parameters_of_All_Available_Devices();
	Clear_File();
	//Print_Parameters_of_All_Available_Devices();
	size_t mnemonic_bits_buf_size = sizeof(cl_ulong) * 2 * NUM_ADDRESSES;
	size_t mnemonic_buf_size = 120 * NUM_ADDRESSES;
	size_t seed_buf_size = 64 * NUM_ADDRESSES;
	size_t work_buf_size = WORK_BUFF_SIZE * NUM_ADDRESSES;
	if (CL_SUCCESS != SetupOpenCL(&ocl, deviceType))	//initialize Open CL objects (context, queue, etc.)
	{
		return -1;
	}

	/*
	cl_ulong mnemonic_hi = 0b0000110000011110001001001110010110010001011101110111100111010010;
	cl_ulong mnemonic_lo = 0b1001011111100001010011010100010111110001010011100001101000011010;
	cl_ulong mnemonic_hi = 0xC1E24E5917779D2;
	cl_ulong mnemonic_lo = 0x97E14D45F14E1A1A;
	Entropy:"00001100000111100010010011100101100100010111011101111001110100101001011111100001010011010100010111110001010011100001101000011010"
	Checksum : "0111"
	Seed: "5b56c417303faa3fcba7e57400e120a0ca83ec5a4fc9ffba757fbe63fbd77a89a1a3be4c67196f57c39a88b76373733891bfaba16ed27a813ceed498804c0570"
	Mnemonic: army van defense carry jealous true garbage claim echo media make crunch
	BIP44 m/44'/0'/0'/0/0 address: 1HQ3rb7nyLPrjnuW85MUknPekwkn7poAUm
	*/
	size_t optimizedSizeMnemonicBitsBuf = ((sizeof(cl_uchar) * mnemonic_bits_buf_size - 1) / 64 + 1) * 64;
	size_t optimizedSizeMnemonicBuf = ((sizeof(cl_uchar) * mnemonic_buf_size - 1) / 64 + 1) * 64;
	size_t optimizedSizeSeedBuf = ((sizeof(cl_uchar) * seed_buf_size - 1) / 64 + 1) * 64;
	size_t optimizedSizeWorkBuf = ((sizeof(cl_uchar) * work_buf_size - 1) / 64 + 1) * 64;
	cl_ulong* mnemonic_bits_buf = (cl_ulong*)_aligned_malloc(optimizedSizeMnemonicBitsBuf, 4096);
	cl_uchar* mnemonic_buf = (cl_uchar*)_aligned_malloc(optimizedSizeMnemonicBuf, 4096);
	cl_uchar* seed_buf = (cl_uchar*)_aligned_malloc(optimizedSizeSeedBuf, 4096);
	//cl_uchar* work_buf = (cl_uchar*)_aligned_malloc(optimizedSizeWorkBuf, 4096);
	if (NULL == mnemonic_bits_buf || NULL == mnemonic_buf || NULL == seed_buf /*|| NULL == work_buf*/)
	{
		LogError("Error: _aligned_malloc failed to allocate buffers.\n");
		goto Error;
	}

#ifdef DEBUG_TEST
	for (int i = 0; i < NUM_ADDRESSES; i++) {
		mnemonic_bits_buf[(i * 2) + 1] = 0b1001011111100001010011010100010111110001010011100001101000011010;
		mnemonic_bits_buf[i * 2] = 0b0000110000011110001001001110010110010001011101110111100111010010;
	}
#else
	Generate_Bytes((uint8_t*)mnemonic_bits_buf, optimizedSizeMnemonicBitsBuf);
#endif //DEBUG_TEST


	// Create third (output) image based on host memory
	ocl.mnemonic_bits = clCreateBuffer(ocl.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, optimizedSizeMnemonicBitsBuf, mnemonic_bits_buf, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for mnemonic_bits returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	ocl.mnemonic = clCreateBuffer(ocl.context, CL_MEM_WRITE_ONLY, optimizedSizeMnemonicBuf, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for mnemonic returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	ocl.seed = clCreateBuffer(ocl.context, CL_MEM_WRITE_ONLY, optimizedSizeSeedBuf, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for seed returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	if (CL_SUCCESS != CreateAndBuildProgram(&ocl, SOURCE_FILE_CODE))
	{
		goto Error;
	}

	start_time();
	printf("\nGENERATE %d ADDRESSES. WAIT...\n", NUM_ADDRESSES);
	ocl.kernel = clCreateKernel(ocl.program, "int_to_mnemonic", &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	err = clSetKernelArg(ocl.kernel, 0, sizeof(cl_mem), (void*)&ocl.mnemonic_bits);
	if (err != CL_SUCCESS)
	{
		LogError("error: Failed to set argument mnemonic_bits, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	err = clSetKernelArg(ocl.kernel, 1, sizeof(cl_mem), (void*)&ocl.mnemonic);
	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to set argument mnemonic, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}


	// Execute (enqueue) the kernel
	size_t range[3] = { NUM_ADDRESSES, 1, 1 }; // Define global size of execution
	err = clEnqueueNDRangeKernel(ocl.commandQueue, ocl.kernel, 1, NULL, range, NULL, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueNDRangeKernel returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	err = clEnqueueReadBuffer(ocl.commandQueue, ocl.mnemonic, CL_TRUE, 0, optimizedSizeMnemonicBuf, mnemonic_buf,
		0, NULL, NULL);
	stop_time_and_print("GENERATE MNEMONIC");

	//err = clReleaseKernel(ocl.kernel);
	//if (err != CL_SUCCESS)
	//{
	//	LogError("Error: clReleaseKernel returned %s\n", TranslateOpenCLError(err));
	//	return -1;
	//}
	err = clFinish(ocl.commandQueue);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clFinish returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}


#define ocl2 ocl

	//ocl_args_d_t ocl2;
	//if (CL_SUCCESS != SetupOpenCL(&ocl2, deviceType))	//initialize Open CL objects (context, queue, etc.)
	//{
	//	return -1;
	//}
	ocl2.mnemonic = clCreateBuffer(ocl2.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, optimizedSizeMnemonicBuf, mnemonic_buf, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for mnemonic returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	ocl2.seed = clCreateBuffer(ocl2.context, CL_MEM_WRITE_ONLY, optimizedSizeSeedBuf, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for seed returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	ocl2.work = clCreateBuffer(ocl2.context, CL_MEM_WRITE_ONLY, optimizedSizeWorkBuf, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for work returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	//if (CL_SUCCESS != CreateAndBuildProgram(&ocl2))
	//{
	//	return -1;
	//}

	//start_time();
	ocl2.kernel = clCreateKernel(ocl2.program, "mnemonic_to_seed", &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	//stop_time_and_print("Create Kernel");
	//start_time();
	err = clSetKernelArg(ocl2.kernel, 0, sizeof(cl_mem), (void*)&ocl2.mnemonic);
	if (err != CL_SUCCESS)
	{
		LogError("error: Failed to set argument mnemonic_bits, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	err = clSetKernelArg(ocl2.kernel, 1, sizeof(cl_mem), (void*)&ocl2.seed);
	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to set argument mnemonic, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	err = clSetKernelArg(ocl2.kernel, 2, sizeof(cl_mem), (void*)&ocl2.work);
	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to set argument work, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	//stop_time_and_print("Set Kernel Arguments");
		// Call clFinish to guarantee that output region is updated

	start_time();

	// Execute (enqueue) the kernel
	size_t range2[3] = { NUM_ADDRESSES, 1, 1 }; // Define global size of execution
	err = clEnqueueNDRangeKernel(ocl2.commandQueue, ocl2.kernel, 1, NULL, range2, NULL, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueNDRangeKernel returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	err = clEnqueueReadBuffer(ocl2.commandQueue, ocl2.seed, CL_TRUE, 0, optimizedSizeSeedBuf, seed_buf,
		0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueReadBuffer returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	err = clFinish(ocl.commandQueue);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clFinish returned %s\n", TranslateOpenCLError(err));
	}
	stop_time_and_print("GENERATE SEEDS AND ADDRESSES", 1.6);

	printf("\nWAIT...\n");
	start_time();
	Get_Derived_BIP44_BIP84_Addressses(seed_buf, bip44_addresses, bip84_addresses);
	stop_time_and_print("CHECK");
	start_time();
	Save_In_File(mnemonic_buf, bip44_addresses, bip84_addresses);
	stop_time_and_print("SAVE IN FILE");
	//}


#ifdef DEBUG_TEST
	for (int i = 0; i < NUM_ADDRESSES; i++) {
		if (memcmp(&seed_buf[0], &seed_buf[i * 64], 64) != 0) {
			printf("\nERROR SEED %d!\n", i);
		}
	}

	for (int i = 0; i < NUM_ADDRESSES; i++) {
		if (memcmp(&bip44_addresses[0], &bip44_addresses[i * 35], 35) != 0) {
			printf("\nERROR BIP44 ADDRESS %d!\n", i);
		}
	}

	for (int i = 0; i < NUM_ADDRESSES; i++) {
		if (memcmp(&bip84_addresses[0], &bip84_addresses[i * 43], 43) != 0) {
			printf("\nERROR BIP84 ADDRESS %d!\n", i);
		}
	}
#endif //DEBUG_TEST


	//char addr[34 + 1];
	//base58_encode_check(hash160_bip44_buf, 21, addr, sizeof(addr));
	//printf("******************************************************************************************\n");
	//printf("Address: %s\n", addr);
	//printf("******************************************************************************************\n");
	//if (memcmp(addr, "1HQ3rb7nyLPrjnuW85MUknPekwkn7poAUm", 34) != 0) {
	//	printf("\nERROR!!!! Address\n");
	//}



	printf("\n\nEND!\n");
	_aligned_free(mnemonic_bits_buf);
	_aligned_free(mnemonic_buf);
	_aligned_free(seed_buf);
	return 0;
Error:
	printf("\n\nERROR!\n");
	_aligned_free(mnemonic_bits_buf);
	_aligned_free(mnemonic_buf);
	_aligned_free(seed_buf);
	return -1;

}


void Check_Test_Seeds(uint8_t* seeds) {
	for (int i = 0; i < NUM_ADDRESSES; i++) {
		if (memcmp(&seeds[0], &seeds[i * 64], 64) != 0) {
			printf("\nERROR SEED %d!\n", i);
		}
	}
}


int _tmain(int argc, TCHAR* argv[])
{
	(void)argc;
	(void)argv;


	//int err;
	//while (1) {
	//	std::string filename = "111.txt";
	//	std::cout << "Please, enter file name: ";
	//	std::getline(std::cin, filename);
	//	std::cout << "Try open " << filename << "\n";

	//	err = create_db("BIP44_Addresses.db");
	//	if (err == -1) break;
	//	err = create_db("BIP84_Addresses.db");
	//	if (err == -1) break;
	//	//err = get_lines_from_txt();
	//	err = get_all_lines_from_txt(filename, "BIP44_Addresses.db", "BIP84_Addresses.db", 10000);
	//	if (err == -1) break;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "BIP44_Addresses Database:" << std::endl;
	//	err = print_info("BIP44_Addresses.db");
	//	if (err == -1) break;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "******************************************************************************************************" << std::endl;
	//	std::cout << "BIP84_Addresses Database:" << std::endl;
	//	err = print_info("BIP84_Addresses.db");
	//	if (err == -1) break;
	//	std::cout << "" << std::endl;
	//	std::cout << "" << std::endl;
	//	std::cout << "END" << std::endl;
	//	break;
	//}

	Test4();


	while (1) {
		Sleep(60000 * 5);
	}

	return 0;
}


void Get_Derived_Addressses(uint8_t* hash160_bip44_buf, uint8_t* seeds) {
	//#pragma omp parallel for
	//	for (int i = 0; i < NUM_ADDRESSES; i++) {
	//		Get_Derived_Address_BIP84_From_Seed(&seeds[i * 64], &bip84_addresses[i * 43]);
	//		base58_encode_check(&hash160_bip44_buf[i * 21], 21, &bip44_addresses[i * 35], 35);
	//	}
}















