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

#include "CL\cl.h"
#include <d3d9.h>
#include <stdint.h>

#pragma once

#define DEBUG_TEST
#ifdef DEBUG_TEST
#define FILE_PATH "Find_Addresses.txt"
#else
#define FILE_PATH "Find_Addresses.csv"
#endif //DEBUG_TEST

#ifdef _DEBUG
#define SOURCE_FILE_CODE_BIN "x64/Debug/Crypto.ir"
#define SOURCE_FILE_CODE "Crypto.cl"
#else
#ifdef DEBUG_TEST
#define SOURCE_FILE_CODE_BIN "x64/Release/Crypto.ir"
#define SOURCE_FILE_CODE "x64/Debug/Crypto.cl"
#else
#define SOURCE_FILE_CODE_BIN "Crypto.ir"
#define SOURCE_FILE_CODE "Crypto.cl"
#endif //DEBUG_TEST
#endif //_DEBUG

// Print useful information to the default output. Same usage as with printf
void LogInfo(const char* str, ...);

// Print error notification to the default output. Same usage as with printf
void LogError(const char* str, ...);

// Read OpenCL source code from fileName and store it in source. The number of read bytes returns in sourceSize
int ReadSourceFromFile(const char* fileName, char** source, size_t* sourceSize);

int LoadProgramSource(const char** files, size_t length, char** buffer, size_t* sizes);

void start_time(void);
void stop_time_and_print(const char* str);
void stop_time_and_print(const char* str, float deliv);

void start_time_forSave(void);
void stop_time_and_print_forSave(const char* str);
void stop_time_and_print_forSave(const char* str, float deliv);

void start_time_forCheck(void);
void stop_time_and_print_forCheck(const char* str);
void stop_time_and_print_forCheck(const char* str, float deliv);

void start_time_forSearch(void);
void stop_time_and_print_forSearch(const char* str);
void stop_time_and_print_forSearch(const char* str, float deliv);


void Clear_File(void);
void Save_In_File(uint8_t* mnemonic_buf, char* bip44_addresses, char* bip84_addresses);
void Add_Line_In_File(char* mnemonic_buf, char* bip44_addresses, char* bip84_addresses, int num);
void Generate_Bytes(uint8_t* buf, size_t len);

