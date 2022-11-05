
#include "CL/cl.h"
#include "sha2.h"
#include "sha512.h"
#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include "trezor-crypto/hmac.h"
//#include <cstdint>
//#include <cstring>
//#include <cctype>




 QWORD sha512_dev_k[80];
 static const QWORD sha512_host_k[80] = {
  0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc, 0x3956c25bf348b538,
  0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118, 0xd807aa98a3030242, 0x12835b0145706fbe,
  0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2, 0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235,
  0xc19bf174cf692694, 0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
  0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5, 0x983e5152ee66dfab,
  0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4, 0xc6e00bf33da88fc2, 0xd5a79147930aa725,
  0x06ca6351e003826f, 0x142929670a0e6e70, 0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed,
  0x53380d139d95b3df, 0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
  0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30, 0xd192e819d6ef5218,
  0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8, 0x19a4c116b8d2d0c8, 0x1e376c085141ab53,
  0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8, 0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373,
  0x682e6ff3d6b2b8a3, 0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
  0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b, 0xca273eceea26619c,
  0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178, 0x06f067aa72176fba, 0x0a637dc5a2c898a6,
  0x113f9804bef90dae, 0x1b710b35131c471b, 0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc,
  0x431d67c49c100d4c, 0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817
};



 void sha512_Init(SHA512_CTX* ctx)
{
  ctx->bitcount[0] = ctx->bitcount[1] = 0;
  ctx->state[0] = 0x6a09e667f3bcc908;
  ctx->state[1] = 0xbb67ae8584caa73b;
  ctx->state[2] = 0x3c6ef372fe94f82b;
  ctx->state[3] = 0xa54ff53a5f1d36f1;
  ctx->state[4] = 0x510e527fade682d1;
  ctx->state[5] = 0x9b05688c2b3e6c1f;
  ctx->state[6] = 0x1f83d9abfb41bd6b;
  ctx->state[7] = 0x5be0cd19137e2179;
}

 /*** ENDIAN REVERSAL MACROS *******************************************/








 void sha512_Transform(const uint64_t* state_in, const uint64_t* data, uint64_t* state_out) {
     uint64_t	a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, h = 0, s0 = 0, s1 = 0;
     uint64_t	T1 = 0, T2 = 0, W512[16] = { 0 };
     int		j = 0;

     /* Initialize registers with the prev. intermediate value */
     a = state_in[0];
     b = state_in[1];
     c = state_in[2];
     d = state_in[3];
     e = state_in[4];
     f = state_in[5];
     g = state_in[6];
     h = state_in[7];

     j = 0;
     do {
         /* Apply the SHA-512 compression function to update a..h with copy */
         T1 = h + Sigma1_512(e) + Ch(e, f, g) + sha512_host_k[j] + (W512[j] = *data++);
         T2 = Sigma0_512(a) + Maj(a, b, c);
         h = g;
         g = f;
         f = e;
         e = d + T1;
         d = c;
         c = b;
         b = a;
         a = T1 + T2;

         j++;
     } while (j < 16);

     do {
         /* Part of the message block expansion: */
         s0 = W512[(j + 1) & 0x0f];
         s0 = sigma0_512(s0);
         s1 = W512[(j + 14) & 0x0f];
         s1 = sigma1_512(s1);

         /* Apply the SHA-512 compression function to update a..h */
         T1 = h + Sigma1_512(e) + Ch(e, f, g) + sha512_host_k[j] +
             (W512[j & 0x0f] += s1 + W512[(j + 9) & 0x0f] + s0);
         T2 = Sigma0_512(a) + Maj(a, b, c);
         h = g;
         g = f;
         f = e;
         e = d + T1;
         d = c;
         c = b;
         b = a;
         a = T1 + T2;

         j++;
     } while (j < 80);
     //for (int i = 0; i < 8; i++) {
     //    printf("state_in[%d] = 0x%08X\n", i, state_in[i]);
     //}
     //printf("*********************************\n");
     /* Compute the current intermediate hash value */
     state_out[0] = state_in[0] + a;
     state_out[1] = state_in[1] + b;
     state_out[2] = state_in[2] + c;
     state_out[3] = state_in[3] + d;
     state_out[4] = state_in[4] + e;
     state_out[5] = state_in[5] + f;
     state_out[6] = state_in[6] + g;
     state_out[7] = state_in[7] + h;
     //for (int i = 0; i < 8; i++) {
     //    printf("state_out[%d] = 0x%08X\n", i, state_out[i]);
     //}
     /* Clean up */
     //a = b = c = d = e = f = g = h = T1 = T2 = 0;
 }

 void sha512_Update(SHA512_CTX* context, const uint8_t* data, size_t len)
{
     unsigned int	freespace = 0, usedspace = 0;
     usedspace = (context->bitcount[0] >> 3) % SHA512_BLOCK_SIZE;
     if (usedspace > 0) {
         /* Calculate how much free space is available in the buffer */
         freespace = SHA512_BLOCK_SIZE - usedspace;

         if (len >= freespace) {
             /* Fill the buffer completely and process it */
             memcpy(((uint8_t*)context->buffer) + usedspace, data, freespace);
             ADDINC128(context->bitcount, freespace << 3);
             len -= freespace;
             data += freespace;
#if BYTE_ORDER == LITTLE_ENDIAN
             /* Convert TO host byte order */
             for (int j = 0; j < 16; j++) {
                 REVERSE64(context->buffer[j], context->buffer[j]);
             }
#endif
             sha512_Transform(context->state, context->buffer, context->state);
         }
         else {
             /* The buffer is not yet full */
             memcpy(((uint8_t*)context->buffer) + usedspace, data, len);
             ADDINC128(context->bitcount, len << 3);
             /* Clean up: */
             usedspace = freespace = 0;
             return;
         }
     }
     while (len >= SHA512_BLOCK_SIZE) {
         /* Process as many complete blocks as we can */
         memcpy(context->buffer, data, SHA512_BLOCK_SIZE);
#if BYTE_ORDER == LITTLE_ENDIAN
         /* Convert TO host byte order */
         for (int j = 0; j < 16; j++) {
             REVERSE64(context->buffer[j], context->buffer[j]);
         }
#endif
         sha512_Transform(context->state, context->buffer, context->state);
         ADDINC128(context->bitcount, SHA512_BLOCK_SIZE << 3);
         len -= SHA512_BLOCK_SIZE;
         data += SHA512_BLOCK_SIZE;
     }
     if (len > 0) {
         /* There's left-overs, so save 'em */
         memcpy(context->buffer, data, len);
         ADDINC128(context->bitcount, len << 3);
     }
}

 static void sha512_Last(SHA512_CTX* context) {
     unsigned int	usedspace = 0;

     usedspace = (context->bitcount[0] >> 3) % SHA512_BLOCK_SIZE;
     /* Begin padding with a 1 bit: */
     ((uint8_t*)context->buffer)[usedspace++] = 0x80;

     if (usedspace > SHA512_SHORT_BLOCK_LENGTH) {
         memset(((uint8_t*)context->buffer) + usedspace, 0, SHA512_BLOCK_SIZE - usedspace);

#if BYTE_ORDER == LITTLE_ENDIAN
         /* Convert TO host byte order */
         for (int j = 0; j < 16; j++) {
             REVERSE64(context->buffer[j], context->buffer[j]);
         }
#endif
         /* Do second-to-last transform: */
         sha512_Transform(context->state, context->buffer, context->state);

         /* And prepare the last transform: */
         usedspace = 0;
     }
     /* Set-up for the last transform: */
     memset(((uint8_t*)context->buffer) + usedspace, 0, SHA512_SHORT_BLOCK_LENGTH - usedspace);

#if BYTE_ORDER == LITTLE_ENDIAN
     /* Convert TO host byte order */
     for (int j = 0; j < 14; j++) {
         REVERSE64(context->buffer[j], context->buffer[j]);
     }
#endif
     /* Store the length of input data (in bits): */
     context->buffer[14] = context->bitcount[1];
     context->buffer[15] = context->bitcount[0];

     /* Final transform: */
     sha512_Transform(context->state, context->buffer, context->state);
 }

 void sha512_Final(SHA512_CTX* context, uint8_t digest[]) {
     /* If no digest buffer is passed, we don't bother doing this: */
     if (digest != (uint8_t*)0) {
         sha512_Last(context);

         /* Save the hash data for output: */
#if BYTE_ORDER == LITTLE_ENDIAN
        /* Convert FROM host byte order */
         for (int j = 0; j < 8; j++) {
             REVERSE64(context->state[j], context->state[j]);
         }
#endif
         memcpy(digest, context->state, SHA512_DIGEST_SIZE);
     }
 }

#define SHA512_BLOCK_LENGTH		128
#define SHA512_DIGEST_LENGTH	64
static uint8_t i_key_pad[SHA512_BLOCK_LENGTH];

 void sha512_Raw(const uint8_t* data, size_t len, uint8_t digest[SHA512_DIGEST_LENGTH]) {
     SHA512_CTX	context = { 0 };
     sha512_Init(&context);
     sha512_Update(&context, data, len);
     sha512_Final(&context, digest);
 }

 //void hmac_sha512_Init(HMAC_SHA512_CTX* hctx, const uint8_t* key,
 //    const uint32_t keylen) {
 //    
 //    memset(i_key_pad, 0, SHA512_BLOCK_LENGTH);
 //    if (keylen > SHA512_BLOCK_LENGTH) {
 //        sha512_Raw(key, keylen, i_key_pad);
 //    }
 //    else {
 //        memcpy(i_key_pad, key, keylen);
 //    }
 //    for (int i = 0; i < SHA512_BLOCK_LENGTH; i++) {
 //        hctx->o_key_pad[i] = i_key_pad[i] ^ 0x5c;
 //        i_key_pad[i] ^= 0x36;
 //    }
 //    sha512_Init(&(hctx->ctx));
 //    sha512_Update(&(hctx->ctx), i_key_pad, SHA512_BLOCK_LENGTH);
 //}

 //void hmac_sha512_Update(HMAC_SHA512_CTX* hctx, const uint8_t* msg,
 //    const uint32_t msglen) {
 //    sha512_Update(&(hctx->ctx), msg, msglen);
 //}

 //void hmac_sha512_Final(HMAC_SHA512_CTX* hctx, uint8_t* hmac) {
 //    sha512_Final(&(hctx->ctx), hmac);
 //    sha512_Init(&(hctx->ctx));
 //    sha512_Update(&(hctx->ctx), hctx->o_key_pad, SHA512_BLOCK_LENGTH);
 //    sha512_Update(&(hctx->ctx), hmac, SHA512_DIGEST_LENGTH);
 //    //memcpy(hctx->ctx.buffer, hmac, SHA512_DIGEST_LENGTH);
 //    sha512_Final(&(hctx->ctx), hmac);
 //}

 //void hmac_sha512(const uint8_t* key, const uint32_t keylen, const uint8_t* msg,
 //    const uint32_t msglen, uint8_t* hmac) {
 //    HMAC_SHA512_CTX hctx = { 0 };
 //    hmac_sha512_Init(&hctx, key, keylen);
 //    hmac_sha512_Update(&hctx, msg, msglen);     
 //    //memcpy(hctx.ctx.buffer, msg, msglen);
 //    hmac_sha512_Final(&hctx, hmac);
 //}
