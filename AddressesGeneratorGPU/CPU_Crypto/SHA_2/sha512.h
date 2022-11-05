#pragma once


#ifndef SHA512_H
#define SHA512_H
#include <stdint.h>
#include <SHA_2/sha2.h>
/**************************** DATA TYPES ****************************/

/****************************** MACROS ******************************/


/*********************** FUNCTION DECLARATIONS **********************/
void hmac_sha512(const uint8_t* key, const uint32_t keylen, const uint8_t* msg, const uint32_t msglen, uint8_t* hmac);
void sha512_Init(SHA512_CTX* ctx);
void sha512_Transform(const uint64_t* state_in, const uint64_t* data, uint64_t* state_out);
void sha512_Update(SHA512_CTX* context, const uint8_t* data, size_t len);
void sha512_Final(SHA512_CTX* context, uint8_t digest[]);
void sha512_Raw(const uint8_t* data, size_t len, uint8_t digest[SHA512_DIGEST_LENGTH]);

#endif   // SHA512_H