#pragma once
#include <Crypto_Tools.h>
#define WORK_BUFF_SIZE (128 + 128 + 256 + 64)

extern char bip44_addresses[(34 + 1) * NUM_ADDRESSES * CARDS_NUMBER];
extern char bip84_addresses[(42 + 1) * NUM_ADDRESSES * CARDS_NUMBER];



void Get_Derived_BIP44_BIP84_Addressses(uint8_t* seeds, char* bip44_addr, char* bip84_addr);
int Test4(void);