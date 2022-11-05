/*****************************************************************************
 * Copyright (c) 2013-2016 Intel Corporation
 * All rights reserved.
 *
 * WARRANTY DISCLAIMER
 *
 * THESE MATERIALS ARE PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 * "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 * LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 * A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR ITS
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 * PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY
 * OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY OR TORT (INCLUDING
 * NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THESE
 * MATERIALS, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * Intel Corporation is the author of the Materials, and requests that all
 * problem reports or change requests be submitted to it directly
 *****************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <tchar.h>
#include <memory.h>
#include <windows.h>

 // Suppress a compiler warning about undefined CL_TARGET_OPENCL_VERSION
 // Khronos ICD supports only latest OpenCL version
//#define CL_TARGET_OPENCL_VERSION 220

#include "CL\cl.h"
#include "CL\cl_ext.h"
#include "utils.h"
#include <assert.h>
#include <fstream>
#include <Crypto_Tools.h>
std::ofstream out;

//we want to use POSIX functions
#pragma warning( push )
#pragma warning( disable : 4996 )


void LogInfo(const char* str, ...)
{
	if (str)
	{
		va_list args;
		va_start(args, str);

		vfprintf(stdout, str, args);

		va_end(args);
	}
}

void LogError(const char* str, ...)
{
	if (str)
	{
		va_list args;
		va_start(args, str);

		vfprintf(stderr, str, args);

		va_end(args);
	}
}

// Upload the OpenCL C source code to output argument source
// The memory resource is implicitly allocated in the function
// and should be deallocated by the caller
int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize)
{
	int errorCode = CL_SUCCESS;

	FILE* fp = NULL;
	fopen_s(&fp, fileName, "rb");
	if (fp == NULL)
	{
		LogError("Error: Couldn't find program source file '%s'.\n", fileName);
		errorCode = CL_INVALID_VALUE;
	}
	else {
		fseek(fp, 0, SEEK_END);
		*sourceSize = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		*source = new char[*sourceSize];
		if (*source == NULL)
		{
			LogError("Error: Couldn't allocate %d bytes for program source from file '%s'.\n", *sourceSize, fileName);
			errorCode = CL_OUT_OF_HOST_MEMORY;
		}
		else {
			fread(*source, 1, *sourceSize, fp);
		}
	}
	return errorCode;
}


int LoadProgramSource(const char** files, size_t length, char** buffer, size_t* sizes)
{
	int errorCode = CL_SUCCESS;
	/* Read each source file (*.cl) and store the contents into a temporary datastore */
	for (size_t i = 0; i < length; i++) {
		errorCode = ReadSourceFromFile(files[i], &buffer[i], &sizes[i]);
		if (CL_SUCCESS != errorCode)
		{
			return errorCode;
		}
	}
	return errorCode;
}


static LARGE_INTEGER performanceCountNDRangeStart;
static LARGE_INTEGER performanceCountNDRangeStop;

void start_time(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStart);
}

void stop_time(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStop);
}

void print_time(const char* str, float deliv) {
	LARGE_INTEGER perfFrequency;
	QueryPerformanceFrequency(&perfFrequency);
	printf("PERFORMANCE: %s time %f ms.\n", str,
		(1000.0f * (float)(performanceCountNDRangeStop.QuadPart - performanceCountNDRangeStart.QuadPart) / (float)perfFrequency.QuadPart) / deliv);
}

void stop_time_and_print(const char* str) {
	stop_time();
	print_time(str, 1);
}

void stop_time_and_print(const char* str, float deliv) {
	stop_time();
	print_time(str, deliv);
}


void Clear_File(void) {
	out.open(FILE_PATH);
	out.close();
}

static bool start_string = false;
void Save_In_File(uint8_t* mnemonic_buf, char *bip44_addresses, char *bip84_addresses) {
	out.open(FILE_PATH, std::ios::app);
	if (out.is_open())
	{
		if (!start_string) {
			out << "Mnemonic" << "," << "m/44'/0'/0'/0/0" << "," << "m/84'/0'/0'/0/0" << std::endl;
			start_string = true;
		}	
		for (int i = 0; i < NUM_ADDRESSES; i++) {
			out << (const char*)&mnemonic_buf[120 * i] << "," << (const char*)&bip44_addresses[35 * i] << "," << (const char*)&bip84_addresses[43 * i] << std::endl;
		}
	}
	else
	{
		printf("\n!!!ERROR create file Derived_Addresses.csv!!!\n");
	}
	out.close();
}

void Add_Line_In_File(char* mnemonic_buf, char* bip44_addresses, char* bip84_addresses, int num) {
	out.open(FILE_PATH, std::ios::app);
	if (out.is_open())
	{
		out << (const char*)&mnemonic_buf[120 * num] << "," << (const char*)&bip44_addresses[35 * num] << "," << (const char*)&bip84_addresses[43 * num] << std::endl;
	}
	else
	{
		printf("\n!!!ERROR create file Derived_Addresses.csv!!!\n");
	}
	out.close();
}


void Generate_Bytes(uint8_t* buf, size_t len) {
	static bool start = false;
	if (!start) {
		srand(time(NULL));
		start = true;
	}
	
	for (int i = 0; i < len; i++) {
		buf[i] = rand() % 255;
	}
}

static LARGE_INTEGER performanceCountNDRangeStart_forSave;
static LARGE_INTEGER performanceCountNDRangeStop_forSave;

void start_time_forSave(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStart_forSave);
}

void stop_time_forSave(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStop_forSave);
}

void print_time_forSave(const char* str, float deliv) {
	LARGE_INTEGER perfFrequency;
	QueryPerformanceFrequency(&perfFrequency);
	printf("PERFORMANCE: %s time %f ms.\n", str,
		(1000.0f * (float)(performanceCountNDRangeStop_forSave.QuadPart - performanceCountNDRangeStart_forSave.QuadPart) / (float)perfFrequency.QuadPart) / deliv);
}

void stop_time_and_print_forSave(const char* str) {
	stop_time_forSave();
	print_time_forSave(str, 1);
}

void stop_time_and_print_forSave(const char* str, float deliv) {
	stop_time_forSave();
	print_time_forSave(str, deliv);
}

static LARGE_INTEGER performanceCountNDRangeStart_forCheck;
static LARGE_INTEGER performanceCountNDRangeStop_forCheck;

void start_time_forCheck(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStart_forCheck);
}

void stop_time_forCheck(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStop_forCheck);
}

void print_time_forCheck(const char* str, float deliv) {
	LARGE_INTEGER perfFrequency;
	QueryPerformanceFrequency(&perfFrequency);
	printf("PERFORMANCE: %s time %f ms.\n", str,
		(1000.0f * (float)(performanceCountNDRangeStop_forCheck.QuadPart - performanceCountNDRangeStart_forCheck.QuadPart) / (float)perfFrequency.QuadPart) / deliv);
}

void stop_time_and_print_forCheck(const char* str) {
	stop_time_forCheck();
	print_time_forCheck(str, 1);
}

void stop_time_and_print_forCheck(const char* str, float deliv) {
	stop_time_forCheck();
	print_time_forCheck(str, deliv);
}


static LARGE_INTEGER performanceCountNDRangeStart_forSearch;
static LARGE_INTEGER performanceCountNDRangeStop_forSearch;

void start_time_forSearch(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStart_forSearch);
}

void stop_time_forSearch(void) {
	QueryPerformanceCounter(&performanceCountNDRangeStop_forSearch);
}

void print_time_forSearch(const char* str, float deliv) {
	LARGE_INTEGER perfFrequency;
	QueryPerformanceFrequency(&perfFrequency);
	printf("PERFORMANCE: %s time %f ms.", str,
		(1000.0f * (float)(performanceCountNDRangeStop_forSearch.QuadPart - performanceCountNDRangeStart_forSearch.QuadPart) / (float)perfFrequency.QuadPart) / deliv);
}

void stop_time_and_print_forSearch(const char* str) {
	stop_time_forSearch();
	print_time_forSearch(str, 1);
}

void stop_time_and_print_forSearch(const char* str, float deliv) {
	stop_time_forSearch();
	print_time_forSearch(str, deliv);
}
#pragma warning( pop )