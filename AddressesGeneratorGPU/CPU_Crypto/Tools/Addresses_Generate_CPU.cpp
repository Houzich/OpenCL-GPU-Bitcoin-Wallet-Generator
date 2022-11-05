// TODO: Add OpenCL kernel code here.
#include "mnemonic_constants.h"
#include "Addresses_Generate_CPU.h"

#include "secp256k1/include/secp256k1.h"
#include "secp256k1/include/secp256k1_recovery.h"
#include "trezor-crypto/ripemd160.h"
#include "trezor-crypto/segwit_addr.h"
#include "SHA_2/sha256.h"
#include "SHA_2/sha512.h"
#include "BIP0032/bip0032.h"
#include <cstring>
#include <stdio.h>
#include <omp.h>
#include "utils.h"

static void get_derived_address_bip44(const secp256k1_context* secp256k1_ctx, btc_hdnode* node, const char* keypath, char addr[34 + 1]);
static void get_derived_address_bip84(const secp256k1_context* secp256k1_ctx, btc_hdnode* node, const char* keypath, char addr[42 + 1]);

static const chain_handle chain_bip84 = {
	0x00,
	0x05,
	"bc",
	0x80,
	0x04B2430C, //zprv
	0x04B24746, //zpub

};

const chain_handle chain_bip44 = {
	0x00,
	0x05,
	"bc",
	0x80,
	0x0488ADE4,
	0x0488B21E,
};


const char* mnemonic_generate(char mnemo_out[12 * 10]);

const char* mnemonic_generate(char mnemo_out[12 * 10]) {
	uint8_t data[32] = { 0 };
	//random_buffer(data, 32);

	return mnemo_out;
}



void Get_Derived_Addressses_BIP84_From_Seed(uint8_t *seeds, char *addr_string, size_t  len) {
	for (int i = 0; i < len; i++) {
		Get_Derived_Address_BIP84_From_Seed(&seeds[i*64], &addr_string[i * 43]);
	}
}

void Get_Derived_Addressses_BIP44_BIP84_From_Seed(uint8_t* seeds, char* addr44_string, char* addr84_string, size_t  len) {
	for (int i = 0; i < len; i++) {
		Get_Derived_Address_BIP44_BIP84_From_Seed(&seeds[i * 64], &addr44_string[i * 35], &addr84_string[i * 43]);
	}
}
secp256k1_context* secp256k1_ctx_XXX;
void Get_Derived_Address_BIP44_BIP84_From_Seed(uint8_t seed[64], char addr44_string[34 + 1], char addr84_string[42 + 1]) {
	
	if (secp256k1_ctx_XXX == NULL)
		return;
	btc_hdnode node;
	btc_hdnode_from_seed(secp256k1_ctx_XXX, seed, &node);
	const char* keyPath;
	keyPath = "m/44'/0'/0'/0/0";
	get_derived_address_bip44(secp256k1_ctx_XXX, &node, keyPath, addr44_string);
	//keyPath = "m/84'/0'/0'/0/0";
	//get_derived_address_bip84(secp256k1_ctx_XXX, &node, keyPath, addr84_string);
	addr44_string[34] = 0;
	addr84_string[42] = 0;
}

void Get_Derived_Address_BIP84_From_Seed(uint8_t seed[64], char addr_string[42 + 1]) {
	secp256k1_context* secp256k1_ctx = secp256k1_context_create(SECP256K1_CONTEXT_SIGN | SECP256K1_CONTEXT_VERIFY);
	if (secp256k1_ctx == NULL)
		return;
	btc_hdnode node;
	btc_hdnode_from_seed(secp256k1_ctx, seed, &node);
	const char* keyPath;
	keyPath = "m/84'/0'/0'/0/0";
	get_derived_address_bip84(secp256k1_ctx, &node, keyPath, addr_string);
}


static void get_derived_address_bip44(const secp256k1_context* secp256k1_ctx, btc_hdnode* node, const char* keypath, char addr[34 + 1])
{
	uint8_t hash160[20 + 1];
	hash160[0] = 0;
	uint8_t hashout[32];
	btc_hdnode newnode;
	btc_hd_generate_key(secp256k1_ctx, &newnode, keypath, node->private_key, node->chain_code);
	sha256_Raw(newnode.public_key, BTC_ECKEY_COMPRESSED_LENGTH, hashout);
	ripemd160(hashout, sizeof(hashout), hash160 + 1);
	base58_encode_check(hash160, sizeof(hash160), addr, 34 + 1);
}

static void get_derived_address_bip84(const secp256k1_context* secp256k1_ctx, btc_hdnode* node, const char* keypath, char addr[42 + 1])
{
	uint8_t hash160[20];
	uint8_t hashout[32];
	btc_hdnode newnode;
	btc_hd_generate_key(secp256k1_ctx, &newnode, keypath, node->private_key, node->chain_code);
	sha256_Raw(newnode.public_key, BTC_ECKEY_COMPRESSED_LENGTH, hashout);
	ripemd160(hashout, sizeof(hashout), hash160);
	segwit_addr_encode(addr, "bc", 0, hash160, sizeof(hash160));
}



















