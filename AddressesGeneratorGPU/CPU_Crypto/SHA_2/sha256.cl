#include "sha2_common.cl"







//char * hash_to_string(uchar * buff) {
//	char * string = (char *)malloc(70);
//	int k, i;
//	for (i = 0, k = 0; i < 32; i++, k+= 2)
//	{
//		sprintf(string + k, "%.2x", buff[i]);
//		//printf("%02x", buff[i]);
//	}
//	string[64] = 0;
//	return string;
//}
//
//void print_job(JOB * j){
//	printf("%s  %s\n", hash_to_string(j->digest), j->fname);
//}
//
//void print_jobs(JOB ** jobs, int n) {
//	for (int i = 0; i < n; i++)
//	{
//        print_job(jobs[i]);
//		// printf("@ %p JOB[%i] \n", jobs[i], i);
//		// printf("\t @ 0x%p data = %x \n", jobs[i]->data, (jobs[i]->data == 0)? 0 : jobs[i]->data[0]);
//		// printf("\t @ 0x%p size = %llu \n", &(jobs[i]->size), jobs[i]->size);
//		// printf("\t @ 0x%p fname = %s \n", &(jobs[i]->fname), jobs[i]->fname);
//		// printf("\t @ 0x%p digest = %s \n------\n", jobs[i]->digest, hash_to_string(jobs[i]->digest));
//	}
//}

void mycpy12( uint *d,  const uint *s) {
#pragma unroll 3
    for (int k=0; k < 3; k++) d[k] = s[k];
}

void mycpy16( uint *d,  const uint *s) {
#pragma unroll 4
    for (int k=0; k < 4; k++) d[k] = s[k];
}

void mycpy32( uint *d,  const uint *s) {
#pragma unroll 8
    for (int k=0; k < 8; k++) d[k] = s[k];
}

void mycpy44( uint *d,  const uint *s) {
#pragma unroll 11
    for (int k=0; k < 11; k++) d[k] = s[k];
}

void mycpy48( uint *d,  const uint *s) {
#pragma unroll 12
    for (int k=0; k < 12; k++) d[k] = s[k];
}

void mycpy64( uint *d,  const uint *s) {
#pragma unroll 16
    for (int k=0; k < 16; k++) d[k] = s[k];
}

void sha256_transform( SHA256_CTX *ctx,  const uchar data[])
{
	uint a, b, c, d, e, f, g, h, i, j, t1, t2, m[64];

    //mycpy32(S, ctx->state);

    #pragma unroll 16
	for (i = 0, j = 0; i < 16; ++i, j += 4)
		m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);

    #pragma unroll 64
	for (; i < 64; ++i)
		m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

	a = ctx->state[0];
	b = ctx->state[1];
	c = ctx->state[2];
	d = ctx->state[3];
	e = ctx->state[4];
	f = ctx->state[5];
	g = ctx->state[6];
	h = ctx->state[7];

    #pragma unroll 64
	for (i = 0; i < 64; ++i) {
		t1 = h + EP1(e) + CH(e, f, g) + sha256_dev_k[i] + m[i];
		t2 = EP0(a) + MAJ(a, b, c);
		h = g;
		g = f;
		f = e;
		e = d + t1;
		d = c;
		c = b;
		b = a;
		a = t1 + t2;
	}

	ctx->state[0] += a;
	ctx->state[1] += b;
	ctx->state[2] += c;
	ctx->state[3] += d;
	ctx->state[4] += e;
	ctx->state[5] += f;
	ctx->state[6] += g;
	ctx->state[7] += h;
}

__kernel void sha256_init(__global SHA256_CTX* ctx)
{
	ctx->datalen = 0;
	ctx->bitlen = 0;
	ctx->state[0] = 0x6a09e667;
	ctx->state[1] = 0xbb67ae85;
	ctx->state[2] = 0x3c6ef372;
	ctx->state[3] = 0xa54ff53a;
	ctx->state[4] = 0x510e527f;
	ctx->state[5] = 0x9b05688c;
	ctx->state[6] = 0x1f83d9ab;
	ctx->state[7] = 0x5be0cd19;
}

 void sha256_update( SHA256_CTX* ctx, __constant uchar data[],  const size_t* len)
{
	uint i;

	// for each byte in message
	for (i = 0; i < *len; ++i) {
		// ctx->data == message 512 bit chunk
		ctx->data[ctx->datalen] = data[i];
		ctx->datalen++;
		if (ctx->datalen == 64) {
			sha256_transform(ctx, ctx->data);
			ctx->bitlen += 512;
			ctx->datalen = 0;
		}
	}
}

 void sha256_final( SHA256_CTX* ctx,  uchar hash[])
{
	uint i;

	i = ctx->datalen;

	// Pad whatever data is left in the buffer.
	if (ctx->datalen < 56) {
		ctx->data[i++] = 0x80;
		while (i < 56)
			ctx->data[i++] = 0x00;
	}
	else {
		ctx->data[i++] = 0x80;
		while (i < 64)
			ctx->data[i++] = 0x00;
		sha256_transform(ctx, ctx->data);
		//memset(ctx->data, 0, 56);
		for (uint x = 0; x < 56; x++)
		{
			ctx->data[x] = 0;
		}

	}

	// Append to the padding the total message's length in bits and transform.
	ctx->bitlen += ctx->datalen * 8;
	ctx->data[63] = ctx->bitlen;
	ctx->data[62] = ctx->bitlen >> 8;
	ctx->data[61] = ctx->bitlen >> 16;
	ctx->data[60] = ctx->bitlen >> 24;
	ctx->data[59] = ctx->bitlen >> 32;
	ctx->data[58] = ctx->bitlen >> 40;
	ctx->data[57] = ctx->bitlen >> 48;
	ctx->data[56] = ctx->bitlen >> 56;
	sha256_transform(ctx, ctx->data);

	// Since this implementation uses little endian byte ordering and SHA uses big endian,
	// reverse all the bytes when copying the final state to the output hash.
	for (i = 0; i < 4; ++i) {
		hash[i] = (ctx->state[0] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 4] = (ctx->state[1] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 8] = (ctx->state[2] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 12] = (ctx->state[3] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 16] = (ctx->state[4] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 20] = (ctx->state[5] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 24] = (ctx->state[6] >> (24 - i * 8)) & 0x000000ff;
		hash[i + 28] = (ctx->state[7] >> (24 - i * 8)) & 0x000000ff;
	}
}







//#endif   // SHA256_H

