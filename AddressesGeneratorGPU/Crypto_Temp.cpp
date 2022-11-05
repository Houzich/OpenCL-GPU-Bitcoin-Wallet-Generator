//#include <stdio.h>
//#include <stdlib.h>
#include <iostream>
#include <chrono>
#include <thread>
#include <fstream>
#include <string>
#include <memory>
#include <sstream>
#include <iomanip>
#include <vector>
#include <map>

#include <OpenCL_Tools.h>
#include <Crypto_Tools.h>
#include <Crypto.h>
#include <BIP0032/bip0032.h>
#include <Tools/Addresses_Generate_CPU.h>
#include <condition_variable>
#include <sqlite_tools.h>

using namespace sqlite_db;
using namespace std::chrono_literals;

int Print_Parameters_of_All_Available_Devices(void);


char bip44_addresses[(34 + 1) * NUM_ADDRESSES * CARDS_NUMBER] = { 0 };
char bip84_addresses[(42 + 1) * NUM_ADDRESSES * CARDS_NUMBER] = { 0 };
uint8_t seed_buffer[SIZE_SEED_BUF * CARDS_NUMBER] = { 0 };
uint8_t mnemonic_buffer[SIZE_MNEMONIC_BUF * CARDS_NUMBER] = { 0 };

char bip44_addresses_for_find[(34 + 1) * NUM_ADDRESSES * CARDS_NUMBER] = { 0 };
char bip84_addresses_for_find[(42 + 1) * NUM_ADDRESSES * CARDS_NUMBER] = { 0 };
uint8_t mnemonic_buffer_for_find[SIZE_MNEMONIC_BUF * CARDS_NUMBER] = { 0 };

int Test3(void)
{
	cl_int err;
	ocl_args_d_t ocl;
	cl_uint num_platform;
	cl_uint num_device;
	Print_Parameters_of_All_Available_Devices();
	std::cout << "";
	std::cout << "";
	std::cout << "Enter platform number: ";
	std::cin >> num_platform;
	std::cout << "Enter device number: ";
	std::cin >> num_device;
	err = SetupOpenCLMY(&ocl, num_platform, num_device);
	if (err != CL_SUCCESS)
	{
		return -1;
	}
	cl_ulong* mnemonic_bits_buf = (cl_ulong*)_aligned_malloc(SIZE_MNEMONIC_BITS_BUF, 4096);
	cl_uchar* mnemonic_buf = (cl_uchar*)_aligned_malloc(SIZE_MNEMONIC_BUF, 4096);
	cl_uchar* seed_buf = (cl_uchar*)_aligned_malloc(SIZE_SEED_BUF, 4096);
	//cl_uchar* work_buf = (cl_uchar*)_aligned_malloc(SIZE_WORK_BUF, 4096);
	if (NULL == mnemonic_bits_buf || NULL == mnemonic_buf || NULL == seed_buf /*|| NULL == work_buf*/)
	{
		LogError("Error: _aligned_malloc failed to allocate buffers.\n");
		goto Error;
	}

	Generate_Bytes((uint8_t*)mnemonic_bits_buf, SIZE_MNEMONIC_BITS_BUF);
	// Create third (output) image based on host memory
	ocl.mnemonic_bits = clCreateBuffer(ocl.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, SIZE_MNEMONIC_BITS_BUF, mnemonic_bits_buf, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for mnemonic_bits returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	ocl.mnemonic = clCreateBuffer(ocl.context, CL_MEM_WRITE_ONLY, SIZE_MNEMONIC_BUF, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for mnemonic returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	ocl.seed = clCreateBuffer(ocl.context, CL_MEM_WRITE_ONLY, SIZE_SEED_BUF, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for seed returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	printf("CREATE AND BUILD PROGRAM. WAIT...\n");
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
	err = clEnqueueReadBuffer(ocl.commandQueue, ocl.mnemonic, CL_TRUE, 0, SIZE_MNEMONIC_BUF, mnemonic_buf,
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
	for (int i = 0; i < NUM_ADDRESSES; i++)
	{
		LogError("mnemonic: %s\n", &mnemonic_buf[i * 120]);
	}
	//***************************************************************************************************************************
	//***************************************************************************************************************************
	//***************************************************************************************************************************
	//***************************************************************************************************************************
	//***************************************************************************************************************************
	ocl.mnemonic = clCreateBuffer(ocl.context, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, SIZE_MNEMONIC_BUF, mnemonic_buf, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for mnemonic returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	ocl.seed = clCreateBuffer(ocl.context, CL_MEM_WRITE_ONLY, SIZE_SEED_BUF, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for seed returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	ocl.work = clCreateBuffer(ocl.context, CL_MEM_WRITE_ONLY, SIZE_WORK_BUF, NULL, &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateBuffer for work returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	ocl.kernel = clCreateKernel(ocl.program, "mnemonic_to_seed", &err);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	err = clSetKernelArg(ocl.kernel, 0, sizeof(cl_mem), (void*)&ocl.mnemonic);
	if (err != CL_SUCCESS)
	{
		LogError("error: Failed to set argument mnemonic, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	err = clSetKernelArg(ocl.kernel, 1, sizeof(cl_mem), (void*)&ocl.seed);
	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to set argument seed, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	err = clSetKernelArg(ocl.kernel, 2, sizeof(cl_mem), (void*)&ocl.work);
	if (err != CL_SUCCESS)
	{
		LogError("Error: Failed to set argument work, returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}

	start_time();

	// Execute (enqueue) the kernel
	size_t range2[3] = { NUM_ADDRESSES, 1, 1 }; // Define global size of execution
	err = clEnqueueNDRangeKernel(ocl.commandQueue, ocl.kernel, 1, NULL, range2, NULL, 0, NULL, NULL);
	if (err != CL_SUCCESS)
	{
		LogError("Error: clEnqueueNDRangeKernel returned %s\n", TranslateOpenCLError(err));
		goto Error;
	}
	err = clEnqueueReadBuffer(ocl.commandQueue, ocl.seed, CL_TRUE, 0, SIZE_SEED_BUF, seed_buf,
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
	stop_time_and_print("GENERATE SEEDS AND ADDRESSES", 1);

	printf("\nWAIT...\n");
	start_time();
	Get_Derived_BIP44_BIP84_Addressses(seed_buf, bip44_addresses, bip84_addresses);
	stop_time_and_print("CHECK");
	start_time();
	Save_In_File(mnemonic_buf, bip44_addresses, bip84_addresses);
	stop_time_and_print("SAVE IN FILE");


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





// Getting the mutex
std::mutex  save_mutex;
std::mutex  search_mutex;
std::condition_variable startSave;
std::condition_variable startSearch;
//std::condition_variable stopSearch;
bool start_save = false;
int start_search = 0;
int stop_search = 1;
std::unique_lock<std::mutex> search_lock(search_mutex);

void Check_Addressses(void) {
	// Getting the lock on queue using mutex
	//std::unique_lock<std::mutex> lock(check_mutex);
	//while (1) {	

		//startCheck.wait(lock, [&] {return start_check; });
		//start_check = false;
		//printf("Check_Addressses!\n");


	//start_time_forCheck();
	for (int i = 0; i < CARDS_NUMBER; i++)
	{
		Get_Derived_BIP44_BIP84_Addressses(&seed_buffer[SIZE_SEED_BUF * i], &bip44_addresses[(34 + 1) * NUM_ADDRESSES * i], &bip84_addresses[(42 + 1) * NUM_ADDRESSES * i]);
	}
	//strcpy(&bip44_addresses[(34 + 1) * 60], "1JpBNhCVW5iq9CBrmBh9VDcQFoBu5ffLLb");
	//strcpy(&bip84_addresses[(42 + 1) * 30], "bc1q9xws9g8z5mgh6uwfscy8qn7l7twf0xvux7xdd7");

	//stop_time_and_print_forCheck("CHECK");

	//start_save = true;
	//startSave.notify_one();

	//stopSearch.wait(search_lock, [&] {return stop_search == 1; });
	while(stop_search == 0)
		std::this_thread::sleep_for(1ms);
	stop_search = 0;

	memcpy(bip44_addresses_for_find, bip44_addresses, sizeof(bip44_addresses));
	memcpy(bip84_addresses_for_find, bip84_addresses, sizeof(bip84_addresses));
	memcpy(mnemonic_buffer_for_find, mnemonic_buffer, sizeof(mnemonic_buffer));


	start_search = 1;
	startSearch.notify_one();

	//}
}

void Save_Addressses(void) {
	// Getting the lock on queue using mutex
	std::unique_lock<std::mutex> lock(save_mutex);
	//while (1) {
	startSave.wait(lock, [&] {return start_save; });
	start_save = false;
	//printf("Save_Addressses!\n");

	//start_time_forSave();
	for (int i = 0; i < CARDS_NUMBER; i++)
	{
		Save_In_File(&mnemonic_buffer[SIZE_MNEMONIC_BUF * i], &bip44_addresses[(34 + 1) * NUM_ADDRESSES * i], &bip84_addresses[(42 + 1) * NUM_ADDRESSES * i]);
	}
	//stop_time_and_print_forSave("SAVE IN FILE");


	start_search = 1;
	startSearch.notify_one();
//}
}

static bool start_string = false;
void Search_Addressses_In_DB(Sqlite3& db_bip44, Sqlite3& db_bip84, char* bip44_addresses, char* bip84_addresses, std::vector<int>& out_idx_bip44, std::vector<int>& out_idx_bip84) {
	const char* address = NULL;
	std::string table_name;
	int find;
	for (int i = 0; i < NUM_ADDRESSES; i++) {
		address = (const char*)&bip44_addresses[35 * i];
		table_name = get_table_name_from_char(address[1]);
		find = db_bip44.search_address(table_name, address);
		if (find == 1) {
			out_idx_bip44.push_back(i);
		}
			
	}
	for (int i = 0; i < NUM_ADDRESSES; i++) {
		address = (const char*)&bip84_addresses[43 * i];
		table_name = get_table_name_from_char(address[4]);
		find = db_bip84.search_address(table_name, address);
		if (find == 1) {
			out_idx_bip84.push_back(i);
		}
			
	}
}


void Search_Addressses(void) {
	Sqlite3 db_bip44;
	Sqlite3 db_bip84;
#if defined(_WIN32)
	/* List of possible priority classes:
	https://docs.microsoft.com/en-us/windows/win32/api/processthreadsapi/nf-processthreadsapi-setpriorityclass
	And respective thread priority numbers:
	https://docs.microsoft.com/en-us/windows/win32/procthread/scheduling-priorities
	*/
	int result = SetPriorityClass(GetCurrentProcess(), THREAD_PRIORITY_HIGHEST);
	if (result != 0) {
		std::cerr << "Setting priority class failed with " << GetLastError() << std::endl;
	}
	result = SetThreadPriority(GetCurrentProcess(), THREAD_PRIORITY_TIME_CRITICAL);
	if (result != 0) {
		std::cerr << "Setting priority number failed with " << GetLastError() << std::endl;
	}
#endif
	try
	{
		db_bip44.open("BIP44_Addresses.db");
		db_bip84.open("BIP84_Addresses.db");
	}
	catch (const Sqlite3Exception& err)
	{
		std::cerr << err.what() << std::endl;
		printf("ERROR Exit!\n");
		//std::exit(1);
		return;
	}
	while (1) {
		startSearch.wait(search_lock, [&] {return start_search != 0; });
		if (start_search == -1) break;
		start_search = 0;
		//printf("Find_Addressses!\n");
		start_time_forSearch();
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			//Save_In_File(&mnemonic_buffer[SIZE_MNEMONIC_BUF * i], &bip44_addresses[(34 + 1) * NUM_ADDRESSES * i], &bip84_addresses[(42 + 1) * NUM_ADDRESSES * i]);
			char* mnemonic = (char*)&mnemonic_buffer_for_find[SIZE_MNEMONIC_BUF * i];
			char* bip44_addr = &bip44_addresses_for_find[(34 + 1) * NUM_ADDRESSES * i];
			char* bip84_addr = &bip84_addresses_for_find[(42 + 1) * NUM_ADDRESSES * i];
			std::vector<int> out_idx_bip44;
			std::vector<int> out_idx_bip84;
			Search_Addressses_In_DB(db_bip44, db_bip84, bip44_addr, bip84_addr, out_idx_bip44, out_idx_bip84);
		
			if ((out_idx_bip44.size() != 0) || (out_idx_bip84.size() != 0))
			{
				if (out_idx_bip44.size() != 0)
				{
					for (int i = 0; i < out_idx_bip44.size(); i++)
					{
						int num = out_idx_bip44[i];
						printf("!!!out_idx_bip44 idx = %d\n", num);
						printf("%s, %s, %s\n", (const char*)&mnemonic[120 * num], (const char*)&bip44_addr[35 * num], (const char*)&bip84_addr[43 * num]);
						Add_Line_In_File(mnemonic, bip44_addr, bip84_addr, num);
					}
				}
				if (out_idx_bip84.size() != 0)
				{
					for (int i = 0; i < out_idx_bip84.size(); i++)
					{
						int num = out_idx_bip84[i];
						printf("!!!out_idx_bip84 idx = %d\n", num);
						printf("%s, %s, %s\n", (const char*)&mnemonic[120 * num], (const char*)&bip44_addr[35 * num], (const char*)&bip84_addr[43 * num]);
						Add_Line_In_File(mnemonic, bip44_addr, bip84_addr, num);
					}
				}

			}
		
		}
		printf("\r");
		stop_time_and_print_forSearch("SEARCH IN FILE");
		stop_search = 1;
		//stopSearch.notify_one();

		//for (int i = 0; i < 20; i++) {
		//	Sleep(1000);
		//	printf("Find_Addressses!\n");
		//}

	}
	printf("Exit!\n");
}



int Test4(void)
{
	cl_int err = CL_SUCCESS;
	crypto_args_t ocl;
	cl_uint num_platform = NUM_PLATFORM;
	cl_uint num_device = 0;

	std::thread check;
	std::thread save;
	std::thread search;
	search = std::thread(&Search_Addressses);
	//start_find = 1;
	//startFind.notify_one();
	//Sleep(6000);
	//start_find = -1;
	//startFind.notify_one();
	//while (1) {
	//	Sleep(60000 * 5);
	//}

	Print_Parameters_of_All_Available_Devices();
	//Clear_File();
	cl_ulong number_of_addresses = 15000;
	std::cout << "Enter number of addresses: ";
	std::cin >> number_of_addresses;
	number_of_addresses = (((number_of_addresses - 1) / (NUM_ADDRESSES * CARDS_NUMBER) + 1) * (NUM_ADDRESSES * CARDS_NUMBER));

	for (int i = 0; i < CARDS_NUMBER; i++)
	{
		num_device = i;
		err = SetupOpenCLCrypto(&ocl, i, num_platform, num_device);
		if (err != CL_SUCCESS)
		{
			return -1;
		}
	}
	cl_ulong* mnemonic_bits_buf[CARDS_NUMBER];
	cl_uchar* mnemonic_buf[CARDS_NUMBER];
	cl_uchar* seed_buf[CARDS_NUMBER];
	for (int i = 0; i < CARDS_NUMBER; i++)
	{
		mnemonic_bits_buf[i] = (cl_ulong*)_aligned_malloc(SIZE_MNEMONIC_BITS_BUF, 4096);
		mnemonic_buf[i] = (cl_uchar*)_aligned_malloc(SIZE_MNEMONIC_BUF, 4096);
		seed_buf[i] = (cl_uchar*)_aligned_malloc(SIZE_SEED_BUF, 4096);
		//cl_uchar* work_buf = (cl_uchar*)_aligned_malloc(SIZE_WORK_BUF * CRYPTO_BUFF_NUMBERS, 4096);
		if (NULL == mnemonic_bits_buf[i] || NULL == mnemonic_buf[i] || NULL == seed_buf[i] /*|| NULL == work_buf[i]*/)
		{
			LogError("Error: _aligned_malloc failed to allocate buffers.\n");
			goto Error;
		}
	}

	printf("CREATE AND BUILD PROGRAM. WAIT...\n");
	for (int i = 0; i < CARDS_NUMBER; i++)
	{
		if (CL_SUCCESS != CreateAndBuildProgramCrypto(&ocl, i, SOURCE_FILE_CODE))
		{
			goto Error;
		}
	}

	printf("GENERATE %lu ADDRESSES. PACKET %u. WAIT...\n\n", number_of_addresses, NUM_ADDRESSES * CARDS_NUMBER);
	start_time();
	for (cl_ulong step = 0; step < number_of_addresses / (NUM_ADDRESSES * CARDS_NUMBER); step++)
	{
		//start_time();

		// Create third (output) image based on host memory
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			Generate_Bytes((uint8_t*)mnemonic_bits_buf[i], SIZE_MNEMONIC_BITS_BUF);


			err |= CreateBuffer(&ocl, CRYPTO_BUFF_MNEMONIC_BITS, i, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mnemonic_bits_buf[i]);
			err |= CreateBuffer(&ocl, CRYPTO_BUFF_MNEMONIC, i, CL_MEM_WRITE_ONLY, NULL);
			err |= CreateBuffer(&ocl, CRYPTO_BUFF_SEED, i, CL_MEM_WRITE_ONLY, NULL);
			if (err != CL_SUCCESS)
			{
				goto Error;
			}
		}

		//printf("GENERATE %lu ADDRESSES. PACKET %u. WAIT...\n", number_of_addresses, NUM_ADDRESSES * CARDS_NUMBER);
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			ocl.kernel[i] = clCreateKernel(ocl.program[i], "int_to_mnemonic", &err);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
				goto Error;
			}
		}


		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			err |= SetKernelArg(&ocl, CRYPTO_BUFF_MNEMONIC_BITS, i, 0);
			err |= SetKernelArg(&ocl, CRYPTO_BUFF_MNEMONIC, i, 1);
			if (err != CL_SUCCESS)
			{
				goto Error;
			}
		}
#define START_CARD 0
		size_t range[CARDS_NUMBER][3];
		for (int i = START_CARD; i < CARDS_NUMBER; i++)
		{
			// Execute (enqueue) the kernel
			range[i][0] = NUM_ADDRESSES; // Define global size of execution
			range[i][1] = 1; // Define global size of execution
			range[i][2] = 1; // Define global size of execution
			err = clEnqueueNDRangeKernel(ocl.commandQueue[i], ocl.kernel[i], 1, NULL, range[i], NULL, 0, NULL, NULL);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clEnqueueNDRangeKernel returned %s\n", TranslateOpenCLError(err));
				goto Error;
			}
		}
		//start_time();
#pragma omp parallel for
		for (int i = START_CARD; i < CARDS_NUMBER; i++)
		{
			err |= clEnqueueReadBuffer(ocl.commandQueue[i], ocl.mnemonic[i], CL_FALSE, 0, SIZE_MNEMONIC_BUF, mnemonic_buf[i],
				0, NULL, NULL);
		}
		if (err != CL_SUCCESS)
		{
			LogError("Error Generate Mnemonic: clEnqueueReadBuffer returned %s\n", TranslateOpenCLError(err));
			goto Error;
		}
		//stop_time_and_print("GENERATE SEEDS clEnqueueReadBuffer", 1);
		//stop_time_and_print("GENERATE MNEMONIC");

		//err = clReleaseKernel(ocl.kernel);
		//if (err != CL_SUCCESS)
		//{
		//	LogError("Error: clReleaseKernel returned %s\n", TranslateOpenCLError(err));
		//	return -1;
		//}

		//start_time();
		for (int i = START_CARD; i < CARDS_NUMBER; i++)
		{
			err = clFinish(ocl.commandQueue[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clFinish returned %s\n", TranslateOpenCLError(err));
				goto Error;
			}
		}


		//for (int i = 0; i < NUM_ADDRESSES; i++)
		//{
		//	LogError("mnemonic[0]: %s\n", (void*)(mnemonic_buf[0] + (i * 120)));
		//}
		//for (int i = 0; i < NUM_ADDRESSES; i++)
		//{
		//	LogError("mnemonic[1]: %s\n", (void*)(mnemonic_buf[1] + (i * 120)));
		//}
		//for (int i = 0; i < NUM_ADDRESSES; i++)
		//{
		//	LogError("mnemonic[2]: %s\n", (void *)(mnemonic_buf[2] + (i * 120)));
		//}



		//stop_time_and_print("GENERATE SEEDS clFinish", 1);
		//***************************************************************************************************************************
		//***************************************************************************************************************************
		//***************************************************************************************************************************
		//***************************************************************************************************************************
		//***************************************************************************************************************************
		//start_time();
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			err |= CreateBuffer(&ocl, CRYPTO_BUFF_MNEMONIC, i, CL_MEM_READ_ONLY | CL_MEM_COPY_HOST_PTR, mnemonic_buf[i]);
			err |= CreateBuffer(&ocl, CRYPTO_BUFF_SEED, i, CL_MEM_WRITE_ONLY, NULL);
			err |= CreateBuffer(&ocl, CRYPTO_BUFF_WORK, i, CL_MEM_WRITE_ONLY, NULL);
			if (err != CL_SUCCESS)
			{
				goto Error;
			}
		}
		//stop_time_and_print("CreateBuffer", 1);
		//start_time();
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			ocl.kernel[i] = clCreateKernel(ocl.program[i], "mnemonic_to_seed", &err);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clCreateKernel returned %s\n", TranslateOpenCLError(err));
				goto Error;
			}
		}
		//stop_time_and_print("clCreateKernel", 1);
		//start_time();
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			err |= SetKernelArg(&ocl, CRYPTO_BUFF_MNEMONIC, i, 0);
			err |= SetKernelArg(&ocl, CRYPTO_BUFF_SEED, i, 1);
			err |= SetKernelArg(&ocl, CRYPTO_BUFF_WORK, i, 2);
			if (err != CL_SUCCESS)
			{
				goto Error;
			}
		}

		//stop_time_and_print("SetKernelArg", 1);


		//check(clEnqueueNDRangeKernel(commandQueue[platform][dev], multiplicationKernel[platform], 2, NULL, globalWorkSize, localWorkSize, 0, NULL, &GPUExecution[i]));

		//// Non-blocking copy of result from device to host
		//cqs[i] = commandQueue[platform][dev];
		//check2(ptrs[i] = clEnqueueMapBuffer(cqs[i], d_C[i], CL_FALSE, CL_MAP_READ, 0, WC * sizeof(TYPE) * workSize[i], 1, &GPUExecution[i], &GPUDone[i], &err));


		//start_time();
		size_t range2[CARDS_NUMBER][3];
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			// Execute (enqueue) the kernel
			range2[i][0] = NUM_ADDRESSES; // Define global size of execution
			range2[i][1] = 1; // Define global size of execution
			range2[i][2] = 1; // Define global size of execution
			err = clEnqueueNDRangeKernel(ocl.commandQueue[i], ocl.kernel[i], 1, NULL, range2[i], NULL, 0, NULL, NULL);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clEnqueueNDRangeKernel returned %s\n", TranslateOpenCLError(err));
				goto Error;
			}
		}
		//stop_time_and_print("clEnqueueNDRangeKernel", 1);
		//start_time();
#pragma omp parallel for
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			err |= clEnqueueReadBuffer(ocl.commandQueue[i], ocl.seed[i], CL_FALSE, 0, SIZE_SEED_BUF, seed_buf[i],
				0, NULL, NULL);
		}
		if (err != CL_SUCCESS)
		{
			LogError("Error Generate SEEDs: clEnqueueReadBuffer returned %s\n", TranslateOpenCLError(err));
			goto Error;
		}
		//err = clWaitForEvents(1, &GPUDone[1]);

		//stop_time_and_print("clEnqueueReadBuffer", 1);
		//start_time();
		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			err = clFinish(ocl.commandQueue[i]);
			if (err != CL_SUCCESS)
			{
				LogError("Error: clFinish returned %s\n", TranslateOpenCLError(err));
			}
		}
		//stop_time_and_print("clFinish", 1);



		////printf("\nWAIT...\n");

		//	//printf("Generate!\n");
		if (step != 0) {
			check.join();
			//save.join();
		}


		for (int i = 0; i < CARDS_NUMBER; i++)
		{
			memcpy(&seed_buffer[SIZE_SEED_BUF * i], seed_buf[i], SIZE_SEED_BUF);
			memcpy(&mnemonic_buffer[SIZE_MNEMONIC_BUF * i], mnemonic_buf[i], SIZE_MNEMONIC_BUF);
		}
		check = std::thread(&Check_Addressses);
		//save = std::thread(&Save_Addressses);

	}

	check.join();
	//save.join();


	while(!search_lock.owns_lock())
		std::this_thread::sleep_for(10ms);
	start_search = -1;
	startSearch.notify_one();
	//search_lock.lock();
	search.join();

	stop_time_and_print("GENERATE SEEDS AND ADDRESSES AND SEARCH", 1);
	printf("\n\nEND!\n");

	for (int i = 0; i < CARDS_NUMBER; i++)
	{
	_aligned_free(mnemonic_bits_buf[i]);
	_aligned_free(mnemonic_buf[i]);
	_aligned_free(seed_buf[i]);
	}

	return 0;
Error:
	//закрываем find
	while (!search_lock.owns_lock())
		std::this_thread::sleep_for(10ms);
	start_search = -1;
	startSearch.notify_one();
	search.join();

	printf("\n\nERROR!\n");
	for (int i = 0; i < CARDS_NUMBER; i++)
	{
		_aligned_free(mnemonic_bits_buf[i]);
		_aligned_free(mnemonic_buf[i]);
		_aligned_free(seed_buf[i]);
	}
	return -1;
}




extern secp256k1_context* secp256k1_ctx_XXX;
void Get_Derived_BIP44_BIP84_Addressses(uint8_t* seeds, char* bip44_addr, char* bip84_addr) {
	static bool start = false;
	if (!start) {
		secp256k1_ctx_XXX = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
		start = true;
	}
	//secp256k1_ctx_XXX = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
#pragma omp parallel for
	for (int i = 0; i < NUM_ADDRESSES; i++) {
		Get_Derived_Address_BIP44_BIP84_From_Seed(&seeds[i * 64], &bip44_addr[i * 35], &bip84_addr[i * 43]);
	}
}












