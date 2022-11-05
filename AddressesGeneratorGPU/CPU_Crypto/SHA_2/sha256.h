#pragma once

#ifndef SHA256_H
#define SHA256_H
#include <SHA_2/sha2.h>
#include <CL/cl.h>
/**************************** DATA TYPES ****************************/

/****************************** MACROS ******************************/



/*********************** FUNCTION DECLARATIONS **********************/
void sha256_Raw(const uint8_t* data, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH]);

void sha256_Init(SHA256_CTX* ctx);
void sha256_Update( SHA256_CTX* ctx, const uint8_t *data, size_t len);
void sha256_Transform(const uint32_t* state_in, const uint32_t* data, uint32_t* state_out);
void sha256_Final(SHA256_CTX* context, uint8_t digest[]);
#endif   // SHA256_H