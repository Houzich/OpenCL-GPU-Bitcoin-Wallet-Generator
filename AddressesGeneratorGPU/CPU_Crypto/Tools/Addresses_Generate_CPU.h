#pragma once
#include <cstdint>
#ifndef __ADDRESSES_GENERATE_CPU_H
#define __ADDRESSES_GENERATE_CPU_H

/**************************** DATA TYPES ****************************/


/****************************** MACROS ******************************/


/*********************** FUNCTION DECLARATIONS **********************/
void Get_Derived_Addressses_BIP84_From_Seed(uint8_t* seeds, char* addr_string, size_t  len);
void Get_Derived_Address_BIP84_From_Seed(uint8_t seed[64], char addr_string[42 + 1]);
void Get_Derived_Address_BIP44_BIP84_From_Seed(uint8_t seed[64], char addr44_string[34 + 1], char addr84_string[42 + 1]);
#endif	//__ADDRESSES_GENERATE_CPU_H