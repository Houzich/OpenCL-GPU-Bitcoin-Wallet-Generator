
#include <iostream>
#include <stdlib.h>
#include <stdio.h>
#include "CL/cl.h"
#include "sha256.h"


cl_uint sha256_dev_k[64] = {
	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
};

//static const uint sha256_host_k[64] = {
//	0x428a2f98,0x71374491,0xb5c0fbcf,0xe9b5dba5,0x3956c25b,0x59f111f1,0x923f82a4,0xab1c5ed5,
//	0xd807aa98,0x12835b01,0x243185be,0x550c7dc3,0x72be5d74,0x80deb1fe,0x9bdc06a7,0xc19bf174,
//	0xe49b69c1,0xefbe4786,0x0fc19dc6,0x240ca1cc,0x2de92c6f,0x4a7484aa,0x5cb0a9dc,0x76f988da,
//	0x983e5152,0xa831c66d,0xb00327c8,0xbf597fc7,0xc6e00bf3,0xd5a79147,0x06ca6351,0x14292967,
//	0x27b70a85,0x2e1b2138,0x4d2c6dfc,0x53380d13,0x650a7354,0x766a0abb,0x81c2c92e,0x92722c85,
//	0xa2bfe8a1,0xa81a664b,0xc24b8b70,0xc76c51a3,0xd192e819,0xd6990624,0xf40e3585,0x106aa070,
//	0x19a4c116,0x1e376c08,0x2748774c,0x34b0bcb5,0x391c0cb3,0x4ed8aa4a,0x5b9cca4f,0x682e6ff3,
//	0x748f82ee,0x78a5636f,0x84c87814,0x8cc70208,0x90befffa,0xa4506ceb,0xbef9a3f7,0xc67178f2
//};




char * hash_to_string(cl_uchar * buff) {
	char * string = (char *)malloc(70); //!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! free(string)
	cl_uint k, i;
	for (i = 0, k = 0; i < 32; i++, k+= 2)
	{
		size_t maxlen = (size_t)(64 - k);
		sprintf_s(&string[k], maxlen, "%02x", buff[i]);
		//printf("%02x", buff[i]);
	}
	string[64] = (char)0;
	return string;
}

void print_job(JOB * j){
	printf("%s  %s\n", hash_to_string(j->digest), j->fname);
}

void print_jobs(JOB ** jobs, int n) {
	for (int i = 0; i < n; i++)
	{
        print_job(jobs[i]);
		// printf("@ %p JOB[%i] \n", jobs[i], i);
		// printf("\t @ 0x%p data = %x \n", jobs[i]->data, (jobs[i]->data == 0)? 0 : jobs[i]->data[0]);
		// printf("\t @ 0x%p size = %llu \n", &(jobs[i]->size), jobs[i]->size);
		// printf("\t @ 0x%p fname = %s \n", &(jobs[i]->fname), jobs[i]->fname);
		// printf("\t @ 0x%p digest = %s \n------\n", jobs[i]->digest, hash_to_string(jobs[i]->digest));
	}
}

void mycpy12( cl_uint *d,  const cl_uint *s) {
//#pragma unroll 3
    for (int k=0; k < 3; k++) d[k] = s[k];
}

void mycpy16( cl_uint *d,  const cl_uint *s) {
//#pragma unroll 4
    for (int k=0; k < 4; k++) d[k] = s[k];
}

void mycpy32( cl_uint *d,  const cl_uint *s) {
//#pragma unroll 8
    for (int k=0; k < 8; k++) d[k] = s[k];
}

void mycpy44( cl_uint *d,  const cl_uint *s) {
//#pragma unroll 11
    for (int k=0; k < 11; k++) d[k] = s[k];
}

void mycpy48( cl_uint *d,  const cl_uint *s) {
//#pragma unroll 12
    for (int k=0; k < 12; k++) d[k] = s[k];
}

void mycpy64( cl_uint *d,  const cl_uint *s) {
//#pragma unroll 16
    for (int k=0; k < 16; k++) d[k] = s[k];
}

void sha256_Transform(const uint32_t* state_in, const uint32_t* data, uint32_t* state_out)
{
	uint32_t	a = 0, b = 0, c = 0, d = 0, e = 0, f = 0, g = 0, h = 0, s0 = 0, s1 = 0;
	uint32_t	T1 = 0, T2 = 0, W256[16] = { 0 };
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
		/* Apply the SHA-256 compression function to update a..h with copy */
		T1 = h + Sigma1_256(e) + Ch(e, f, g) + sha256_dev_k[j] + (W256[j] = *data++);
		T2 = Sigma0_256(a) + Maj(a, b, c);
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
		s0 = W256[(j + 1) & 0x0f];
		s0 = sigma0_256(s0);
		s1 = W256[(j + 14) & 0x0f];
		s1 = sigma1_256(s1);

		/* Apply the SHA-256 compression function to update a..h */
		T1 = h + Sigma1_256(e) + Ch(e, f, g) + sha256_dev_k[j] +
			(W256[j & 0x0f] += s1 + W256[(j + 9) & 0x0f] + s0);
		T2 = Sigma0_256(a) + Maj(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + T1;
		d = c;
		c = b;
		b = a;
		a = T1 + T2;

		j++;
	} while (j < 64);

	/* Compute the current intermediate hash value */
	state_out[0] = state_in[0] + a;
	state_out[1] = state_in[1] + b;
	state_out[2] = state_in[2] + c;
	state_out[3] = state_in[3] + d;
	state_out[4] = state_in[4] + e;
	state_out[5] = state_in[5] + f;
	state_out[6] = state_in[6] + g;
	state_out[7] = state_in[7] + h;
}

void sha256_Init(SHA256_CTX* ctx)
{
	ctx->bitcount = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

void sha256_Update(SHA256_CTX* ctx, const uint8_t *data, size_t len)
{
	while (len >= SHA256_BLOCK_LENGTH) {
		/* Process as many complete blocks as we can */
		memcpy(ctx->buffer, data, SHA256_BLOCK_LENGTH);
#if BYTE_ORDER == LITTLE_ENDIAN
		/* Convert TO host byte order */
		for (int j = 0; j < 16; j++) {
			REVERSE32(ctx->buffer[j], ctx->buffer[j]);
		}
#endif
		sha256_Transform(ctx->state, ctx->buffer, ctx->state);
		ctx->bitcount += SHA256_BLOCK_LENGTH << 3;
		len -= SHA256_BLOCK_LENGTH;
		data += SHA256_BLOCK_LENGTH;
	}
	if (len > 0) {
		/* There's left-overs, so save 'em */
		memcpy(ctx->buffer, data, len);
		ctx->bitcount += len << 3;
	}
}

void sha256_Final(SHA256_CTX* context, uint8_t digest[])
{
	unsigned int	usedspace = 0;

		usedspace = (context->bitcount >> 3) % SHA256_BLOCK_LENGTH;
		/* Begin padding with a 1 bit: */
		((uint8_t*)context->buffer)[usedspace++] = 0x80;

		if (usedspace > SHA256_SHORT_BLOCK_LENGTH) {
			memset(((uint8_t*)context->buffer) + usedspace, 0, SHA256_BLOCK_LENGTH - usedspace);

#if BYTE_ORDER == LITTLE_ENDIAN
			/* Convert TO host byte order */
			for (int j = 0; j < 16; j++) {
				REVERSE32(context->buffer[j], context->buffer[j]);
			}
#endif
			/* Do second-to-last transform: */
			sha256_Transform(context->state, context->buffer, context->state);

			/* And prepare the last transform: */
			usedspace = 0;
		}
		/* Set-up for the last transform: */
		memset(((uint8_t*)context->buffer) + usedspace, 0, SHA256_SHORT_BLOCK_LENGTH - usedspace);

#if BYTE_ORDER == LITTLE_ENDIAN
		/* Convert TO host byte order */
		for (int j = 0; j < 14; j++) {
			REVERSE32(context->buffer[j], context->buffer[j]);
		}
#endif
		/* Set the bit count: */
		context->buffer[14] = context->bitcount >> 32;
		context->buffer[15] = context->bitcount & 0xffffffff;

		/* Final transform: */
		sha256_Transform(context->state, context->buffer, context->state);

#if BYTE_ORDER == LITTLE_ENDIAN
		/* Convert FROM host byte order */
		for (int j = 0; j < 8; j++) {
			REVERSE32(context->state[j], context->state[j]);
		}
#endif
		memcpy(digest, context->state, SHA256_DIGEST_LENGTH);
	
}

void sha256_Raw(const uint8_t* data, size_t len, uint8_t digest[SHA256_DIGEST_LENGTH]) {
	SHA256_CTX	context = { 0 };
	sha256_Init(&context);
	sha256_Update(&context, data, len);
	sha256_Final(&context, digest);
}

//#endif   // SHA256_H

