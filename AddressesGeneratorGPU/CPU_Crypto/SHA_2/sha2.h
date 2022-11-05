#pragma once


#ifndef SHA2_H
#define SHA2_H
#include <stdint.h>
/****************************** MACROS ******************************/
#ifndef LITTLE_ENDIAN
#define LITTLE_ENDIAN 1234
#define BIG_ENDIAN    4321
#endif

#ifndef BYTE_ORDER
#define BYTE_ORDER LITTLE_ENDIAN
#endif

#if BYTE_ORDER == LITTLE_ENDIAN
#define REVERSE32(w,x)	{ \
	uint32_t tmp = (w); \
	tmp = (tmp >> 16) | (tmp << 16); \
	(x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
}
#define REVERSE64(w,x)	{ \
	uint64_t tmp = (w); \
	tmp = (tmp >> 32) | (tmp << 32); \
	tmp = ((tmp & 0xff00ff00ff00ff00ULL) >> 8) | \
	      ((tmp & 0x00ff00ff00ff00ffULL) << 8); \
	(x) = ((tmp & 0xffff0000ffff0000ULL) >> 16) | \
	      ((tmp & 0x0000ffff0000ffffULL) << 16); \
}
#endif /* BYTE_ORDER == LITTLE_ENDIAN */

#define SHA256_DIGEST_LENGTH		32
#define SHA256_BLOCK_LENGTH			64
#define SHA256_SHORT_BLOCK_LENGTH	(SHA256_BLOCK_LENGTH - 8)

#define SHA512_DIGEST_LENGTH		64
#define SHA512_BLOCK_LENGTH			128
#define SHA512_SHORT_BLOCK_LENGTH	(SHA512_BLOCK_SIZE - 16)

//#define ROTLEFT(a,b) (((a) << (b)) | ((a) >> (32-(b))))
//#define ROTRIGHT(a,b) (((a) >> (b)) | ((a) << (32-(b))))
//
//#define CH(x,y,z) (((x) & (y)) ^ (~(x) & (z)))
//#define MAJ(x,y,z) (((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))
//#define EP0(x) (ROTRIGHT(x,2) ^ ROTRIGHT(x,13) ^ ROTRIGHT(x,22))
//#define EP1(x) (ROTRIGHT(x,6) ^ ROTRIGHT(x,11) ^ ROTRIGHT(x,25))
//#define SIG0(x) (ROTRIGHT(x,7) ^ ROTRIGHT(x,18) ^ ((x) >> 3))
//#define SIG1(x) (ROTRIGHT(x,17) ^ ROTRIGHT(x,19) ^ ((x) >> 10))


 /* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define SHR(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define ROTR32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
 /* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define ROTR64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))
 /* Four of six logical functions used in SHA-384 and SHA-512: */

 /* Two of six logical functions used in SHA-1, SHA-256, SHA-384, and SHA-512: */
#define Ch(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define Maj(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))

#define Sigma0_512(x)	(ROTR64(28, (x)) ^ ROTR64(34, (x)) ^ ROTR64(39, (x)))
#define Sigma1_512(x)	(ROTR64(14, (x)) ^ ROTR64(18, (x)) ^ ROTR64(41, (x)))
#define sigma0_512(x)	(ROTR64( 1, (x)) ^ ROTR64( 8, (x)) ^ SHR( 7,   (x)))
#define sigma1_512(x)	(ROTR64(19, (x)) ^ ROTR64(61, (x)) ^ SHR( 6,   (x)))

/* Four of six logical functions used in SHA-256: */
#define Sigma0_256(x)	(ROTR32(2,  (x)) ^ ROTR32(13, (x)) ^ ROTR32(22, (x)))
#define Sigma1_256(x)	(ROTR32(6,  (x)) ^ ROTR32(11, (x)) ^ ROTR32(25, (x)))
#define sigma0_256(x)	(ROTR32(7,  (x)) ^ ROTR32(18, (x)) ^ SHR(3 ,   (x)))
#define sigma1_256(x)	(ROTR32(17, (x)) ^ ROTR32(19, (x)) ^ SHR(10,   (x)))

/* Four of six logical functions used in SHA-384 and SHA-512: */
#define Sigma0_512(x)	(ROTR64(28, (x)) ^ ROTR64(34, (x)) ^ ROTR64(39, (x)))
#define Sigma1_512(x)	(ROTR64(14, (x)) ^ ROTR64(18, (x)) ^ ROTR64(41, (x)))
#define sigma0_512(x)	(ROTR64( 1, (x)) ^ ROTR64( 8, (x)) ^ SHR( 7,   (x)))
#define sigma1_512(x)	(ROTR64(19, (x)) ^ ROTR64(61, (x)) ^ SHR( 6,   (x)))


#define SHA512_DIGEST_SIZE 64
#define SHA512_BLOCK_SIZE 128

/*
 * Macro for incrementally adding the unsigned 64-bit integer n to the
 * unsigned 128-bit integer (represented using a two-element array of
 * 64-bit words):
 */
#define ADDINC128(w,n)	{ \
	(w)[0] += (uint64_t)(n); \
	if ((w)[0] < (n)) { \
		(w)[1]++; \
	} \
}
 /**************************** DATA TYPES ****************************/
 //typedef unsigned char BYTE;
 //typedef uint32_t  WORD;
typedef uint64_t   QWORD;

typedef struct _SHA256_CTX {
	uint32_t	state[8];
	uint64_t	bitcount;
	uint32_t	buffer[SHA256_BLOCK_LENGTH / sizeof(uint32_t)];
} SHA256_CTX;

typedef struct _SHA512_CTX {
	uint64_t	state[8];
	uint64_t	bitcount[2];
	uint64_t	buffer[SHA512_BLOCK_SIZE / sizeof(uint64_t)];
} SHA512_CTX;

typedef struct JOB {
	uint8_t* data;
	unsigned long long size;
	uint8_t digest[64];
	char fname[128];
}JOB;


/* Initial hash value H for SHA-256: */
const uint32_t sha256_initial_hash_value[8] = {
	0x6a09e667UL,
	0xbb67ae85UL,
	0x3c6ef372UL,
	0xa54ff53aUL,
	0x510e527fUL,
	0x9b05688cUL,
	0x1f83d9abUL,
	0x5be0cd19UL
};

/* Initial hash value H for SHA-512 */
const uint64_t sha512_initial_hash_value[8] = {
	0x6a09e667f3bcc908ULL,
	0xbb67ae8584caa73bULL,
	0x3c6ef372fe94f82bULL,
	0xa54ff53a5f1d36f1ULL,
	0x510e527fade682d1ULL,
	0x9b05688c2b3e6c1fULL,
	0x1f83d9abfb41bd6bULL,
	0x5be0cd19137e2179ULL
};



/*********************** FUNCTION DECLARATIONS **********************/


#endif   // SHA2_H