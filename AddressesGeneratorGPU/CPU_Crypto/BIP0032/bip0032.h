#pragma once
#include <cstdint>
#ifndef __BIP0032_H
#define __BIP0032_H
#include "secp256k1/include/secp256k1.h"
#include "secp256k1/include/secp256k1_recovery.h"



/****************************** MACROS ******************************/
#define BTC_ECKEY_UNCOMPRESSED_LENGTH 65
#define BTC_ECKEY_COMPRESSED_LENGTH 33
#define BTC_ECKEY_PKEY_LENGTH 32
#define BTC_BIP32_CHAINCODE_SIZE 32

/**************************** DATA TYPES ****************************/
typedef struct
{
    uint32_t depth;
    uint32_t fingerprint;
    uint32_t child_num;
    uint8_t chain_code[BTC_BIP32_CHAINCODE_SIZE];
    uint8_t private_key[BTC_ECKEY_PKEY_LENGTH];
    uint8_t public_key[BTC_ECKEY_COMPRESSED_LENGTH];
} btc_hdnode;

typedef struct {
	uint8_t b58prefix_pubkey_address;
	uint8_t b58prefix_script_address;
	const char bech32_hrp[5];
	uint8_t b58prefix_secret_address; //!private key
	uint32_t b58prefix_bip32_privkey;
	uint32_t b58prefix_bip32_pubkey;
} chain_handle;
/*********************** FUNCTION DECLARATIONS **********************/
int base58_encode_check(const uint8_t* data, int datalen, char* str, int strsize);
bool btc_hdnode_from_seed(const secp256k1_context* secp256k1_ctx, const uint8_t* seed, btc_hdnode* out);
void btc_ecc_get_compressed_pubkey(const secp256k1_context* secp256k1_ctx, const uint8_t* private_key, uint8_t* public_key);
uint8_t btc_hdnode_private_ckd(const secp256k1_context* secp256k1_ctx, btc_hdnode* inout, uint32_t i);
uint8_t btc_hd_generate_key(const secp256k1_context* secp256k1_ctx, btc_hdnode* node, const char* keypath, const uint8_t* keymaster, const uint8_t* chaincode);
#endif	//__BIP0032_H