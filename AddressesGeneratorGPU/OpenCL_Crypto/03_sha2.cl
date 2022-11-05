#ifndef _SHA2_CL
#define _SHA2_CL
#include "01_common.cl"

//#define F1(x,y,z) (bitselect(z,y,x))
//#define F0(x,y,z) (bitselect (x, y, ((x) ^ (z))))

/* Two of six logical functions used in SHA-1, SHA-256, SHA-384, and SHA-512: */
#define F1(x,y,z)	(((x) & (y)) ^ ((~(x)) & (z)))
#define F0(x,y,z)	(((x) & (y)) ^ ((x) & (z)) ^ ((y) & (z)))


#define mod(x,y) ((x)-((x)/(y)*(y)))
#define shr32(x,n) ((x) >> (n))
#define rotl32(a,n) rotate ((a), (n))
#define rotl64(a,n) (rotate ((a), (n)))
#define rotr64(a,n) (rotate ((a), (64ul-n)))
#define S0(x) (rotl32 ((x), 25u) ^ rotl32 ((x), 14u) ^ shr32 ((x),  3u))
#define S1(x) (rotl32 ((x), 15u) ^ rotl32 ((x), 13u) ^ shr32 ((x), 10u))
#define S2(x) (rotl32 ((x), 30u) ^ rotl32 ((x), 19u) ^ rotl32 ((x), 10u))
#define S3(x) (rotl32 ((x), 26u) ^ rotl32 ((x), 21u) ^ rotl32 ((x),  7u))
//#define SHA512_S0(x) (rotr64(x,28ul) ^ rotr64(x,34ul) ^ rotr64(x,39ul))
//#define SHA512_S1(x) (rotr64(x,14ul) ^ rotr64(x,18ul) ^ rotr64(x,41ul))
//#define little_s0(x) (rotr64(x,1ul) ^ rotr64(x,8ul) ^ ((x) >> 7ul))
//#define little_s1(x) (rotr64(x,19ul) ^ rotr64(x,61ul) ^ ((x) >> 6ul))


 /* Shift-right (used in SHA-256, SHA-384, and SHA-512): */
#define SHR(b,x) 		((x) >> (b))
/* 32-bit Rotate-right (used in SHA-256): */
#define ROTR32(b,x)	(((x) >> (b)) | ((x) << (32 - (b))))
 /* 64-bit Rotate-right (used in SHA-384 and SHA-512): */
#define ROTR64(b,x)	(((x) >> (b)) | ((x) << (64 - (b))))
 /* Four of six logical functions used in SHA-384 and SHA-512: */
#define REVERSE32(w,x)	{ \
	uint tmp = (w); \
	tmp = (tmp >> 16) | (tmp << 16); \
	(x) = ((tmp & 0xff00ff00UL) >> 8) | ((tmp & 0x00ff00ffUL) << 8); \
}
#define REVERSE64(w,x)	{ \
	ulong tmp = (w); \
	tmp = (tmp >> 32) | (tmp << 32); \
	tmp = ((tmp & 0xff00ff00ff00ff00ULL) >> 8) | \
	      ((tmp & 0x00ff00ff00ff00ffULL) << 8); \
	(x) = ((tmp & 0xffff0000ffff0000ULL) >> 16) | \
	      ((tmp & 0x0000ffff0000ffffULL) << 16); \
}
/* Four of six logical functions used in SHA-384 and SHA-512: */
#define SHA512_S0(x)	(ROTR64(28, (x)) ^ ROTR64(34, (x)) ^ ROTR64(39, (x)))
#define SHA512_S1(x)	(ROTR64(14, (x)) ^ ROTR64(18, (x)) ^ ROTR64(41, (x)))
#define little_s0(x)	(ROTR64( 1, (x)) ^ ROTR64( 8, (x)) ^ SHR( 7,   (x)))
#define little_s1(x)	(ROTR64(19, (x)) ^ ROTR64(61, (x)) ^ SHR( 6,   (x)))


#define highBit(i) (0x1UL << (8*i + 7))
#define fBytes(i) (0xFFFFFFFFFFFFFFFFUL >> (8 * (8-i)))
#define SHA256C00 0x428a2f98u
#define SHA256C01 0x71374491u
#define SHA256C02 0xb5c0fbcfu
#define SHA256C03 0xe9b5dba5u
#define SHA256C04 0x3956c25bu
#define SHA256C05 0x59f111f1u
#define SHA256C06 0x923f82a4u
#define SHA256C07 0xab1c5ed5u
#define SHA256C08 0xd807aa98u
#define SHA256C09 0x12835b01u
#define SHA256C0a 0x243185beu
#define SHA256C0b 0x550c7dc3u
#define SHA256C0c 0x72be5d74u
#define SHA256C0d 0x80deb1feu
#define SHA256C0e 0x9bdc06a7u
#define SHA256C0f 0xc19bf174u
#define SHA256C10 0xe49b69c1u
#define SHA256C11 0xefbe4786u
#define SHA256C12 0x0fc19dc6u
#define SHA256C13 0x240ca1ccu
#define SHA256C14 0x2de92c6fu
#define SHA256C15 0x4a7484aau
#define SHA256C16 0x5cb0a9dcu
#define SHA256C17 0x76f988dau
#define SHA256C18 0x983e5152u
#define SHA256C19 0xa831c66du
#define SHA256C1a 0xb00327c8u
#define SHA256C1b 0xbf597fc7u
#define SHA256C1c 0xc6e00bf3u
#define SHA256C1d 0xd5a79147u
#define SHA256C1e 0x06ca6351u
#define SHA256C1f 0x14292967u
#define SHA256C20 0x27b70a85u
#define SHA256C21 0x2e1b2138u
#define SHA256C22 0x4d2c6dfcu
#define SHA256C23 0x53380d13u
#define SHA256C24 0x650a7354u
#define SHA256C25 0x766a0abbu
#define SHA256C26 0x81c2c92eu
#define SHA256C27 0x92722c85u
#define SHA256C28 0xa2bfe8a1u
#define SHA256C29 0xa81a664bu
#define SHA256C2a 0xc24b8b70u
#define SHA256C2b 0xc76c51a3u
#define SHA256C2c 0xd192e819u
#define SHA256C2d 0xd6990624u
#define SHA256C2e 0xf40e3585u
#define SHA256C2f 0x106aa070u
#define SHA256C30 0x19a4c116u
#define SHA256C31 0x1e376c08u
#define SHA256C32 0x2748774cu
#define SHA256C33 0x34b0bcb5u
#define SHA256C34 0x391c0cb3u
#define SHA256C35 0x4ed8aa4au
#define SHA256C36 0x5b9cca4fu
#define SHA256C37 0x682e6ff3u
#define SHA256C38 0x748f82eeu
#define SHA256C39 0x78a5636fu
#define SHA256C3a 0x84c87814u
#define SHA256C3b 0x8cc70208u
#define SHA256C3c 0x90befffau
#define SHA256C3d 0xa4506cebu
#define SHA256C3e 0xbef9a3f7u
#define SHA256C3f 0xc67178f2u 

// 512 bytes
__constant ulong padLong[8] = { highBit(0), highBit(1), highBit(2), highBit(3), highBit(4), highBit(5), highBit(6), highBit(7) };

// 512 bytes
__constant ulong maskLong[8] = { 0, fBytes(1), fBytes(2), fBytes(3), fBytes(4), fBytes(5), fBytes(6), fBytes(7) };

uint SWAP256(uint val) {
  return (rotate(((val) & 0x00FF00FF), 24U) | rotate(((val) & 0xFF00FF00), 8U));
}

ulong SWAP512(const ulong val) {
  //printf("val 0x%llX\n", val);
  ulong tmp = (rotr64(val & 0x0000FFFF0000FFFFUL, 16UL) | rotl64(val & 0xFFFF0000FFFF0000UL, 16UL));
  //printf("tmp 0x%llX\n", tmp);
  ulong ret = (rotr64(tmp & 0xFF00FF00FF00FF00UL, 8UL) | rotl64(tmp & 0x00FF00FF00FF00FFUL, 8UL));
  //printf("ret 0x%llX\n", ret);
  return ret;
}

__kernel void SWAP512_GPU(__global const ulong *value, __global ulong *ret) {
    ulong val = *value;
    //printf("val gpu 0x%llX\n", val);
    REVERSE64(val, *ret);
    //printf("ret gpu 0x%llX\n", *ret);
}

__kernel void SWAP512_ToGlobal_GPU(const ulong val, __global ulong* ret) {
    //printf("val gpu 0x%llX\n", val);
    ulong tmp = (rotr64(val & 0x0000FFFF0000FFFFUL, 16UL) | rotl64(val & 0xFFFF0000FFFF0000UL, 16UL));
    //printf("tmp gpu 0x%llX\n", tmp);
    *ret = (rotr64(tmp & 0xFF00FF00FF00FF00UL, 8UL) | rotl64(tmp & 0x00FF00FF00FF00FFUL, 8UL));
    //printf("ret gpu 0x%llX\n", *ret);
}

  // 1, 383 0's, 128 bit length BE
// ulong is 64 bits => 8 bytes so msg[0] is bytes 1->8  msg[1] is bytes 9->16
// msg[24] is bytes 193->200 but our message is only 192 bytes
static int md_pad_128(ulong *msg, const long msgLen_bytes) {
  const uint padLongIndex = ((uint)msgLen_bytes) / 8; // 24
  const uint overhang = (((uint)msgLen_bytes) - padLongIndex*8); // 0
  msg[padLongIndex] &= maskLong[overhang]; // msg[24] = msg[24] & 0 -> 0's out this byte
  msg[padLongIndex] |= padLong[overhang]; // msg[24] = msg[24] | 0x1UL << 7 -> sets it to 0x1UL << 7
  msg[padLongIndex + 1] = 0; // msg[25] = 0
  msg[padLongIndex + 2] = 0; // msg[26] = 0
  uint i = 0;

  // 27, 28, 29, 30, 31 = 0
  for (i = padLongIndex + 3; i % 16 != 0; i++) {
    msg[i] = 0;
  }
  // i = 32
  int nBlocks = i / 16; // nBlocks = 2
  msg[i-2] = 0; // msg[30] = 0; already did this in loop..
  msg[i-1] = SWAP512(msgLen_bytes*8); // msg[31] = SWAP512(1536)
  return nBlocks; // 2
};

// 1, 383 0's, 128 bit length BE
// ulong is 64 bits => 8 bytes so msg[0] is bytes 1->8  msg[1] is bytes 9->16
// msg[24] is bytes 193->200 but our message is only 192 bytes
static int md_pad_128_global(__global ulong* msg, const long msgLen_bytes) {
    const uint padLongIndex = ((uint)msgLen_bytes) / 8; // 24
    const uint overhang = (((uint)msgLen_bytes) - padLongIndex * 8); // 0
    msg[padLongIndex] &= maskLong[overhang]; // msg[24] = msg[24] & 0 -> 0's out this byte
    msg[padLongIndex] |= padLong[overhang]; // msg[24] = msg[24] | 0x1UL << 7 -> sets it to 0x1UL << 7
    msg[padLongIndex + 1] = 0; // msg[25] = 0
    msg[padLongIndex + 2] = 0; // msg[26] = 0
    uint i = 0;

    // 27, 28, 29, 30, 31 = 0
    for (i = padLongIndex + 3; i % 16 != 0; i++) {
        msg[i] = 0;
    }
    // i = 32
    int nBlocks = i / 16; // nBlocks = 2
    msg[i - 2] = 0; // msg[30] = 0; already did this in loop..
    msg[i - 1] = SWAP512(msgLen_bytes * 8); // msg[31] = SWAP512(1536)
    return nBlocks; // 2
};

// 256 bytes
__constant uint k_sha256[64] =
{
  SHA256C00, SHA256C01, SHA256C02, SHA256C03,
  SHA256C04, SHA256C05, SHA256C06, SHA256C07,
  SHA256C08, SHA256C09, SHA256C0a, SHA256C0b,
  SHA256C0c, SHA256C0d, SHA256C0e, SHA256C0f,
  SHA256C10, SHA256C11, SHA256C12, SHA256C13,
  SHA256C14, SHA256C15, SHA256C16, SHA256C17,
  SHA256C18, SHA256C19, SHA256C1a, SHA256C1b,
  SHA256C1c, SHA256C1d, SHA256C1e, SHA256C1f,
  SHA256C20, SHA256C21, SHA256C22, SHA256C23,
  SHA256C24, SHA256C25, SHA256C26, SHA256C27,
  SHA256C28, SHA256C29, SHA256C2a, SHA256C2b,
  SHA256C2c, SHA256C2d, SHA256C2e, SHA256C2f,
  SHA256C30, SHA256C31, SHA256C32, SHA256C33,
  SHA256C34, SHA256C35, SHA256C36, SHA256C37,
  SHA256C38, SHA256C39, SHA256C3a, SHA256C3b,
  SHA256C3c, SHA256C3d, SHA256C3e, SHA256C3f,
};

// 5kB
__constant ulong k_sha512[80] =
{
    0x428a2f98d728ae22UL, 0x7137449123ef65cdUL, 0xb5c0fbcfec4d3b2fUL, 0xe9b5dba58189dbbcUL, 0x3956c25bf348b538UL, 
    0x59f111f1b605d019UL, 0x923f82a4af194f9bUL, 0xab1c5ed5da6d8118UL, 0xd807aa98a3030242UL, 0x12835b0145706fbeUL, 
    0x243185be4ee4b28cUL, 0x550c7dc3d5ffb4e2UL, 0x72be5d74f27b896fUL, 0x80deb1fe3b1696b1UL, 0x9bdc06a725c71235UL, 
    0xc19bf174cf692694UL, 0xe49b69c19ef14ad2UL, 0xefbe4786384f25e3UL, 0x0fc19dc68b8cd5b5UL, 0x240ca1cc77ac9c65UL, 
    0x2de92c6f592b0275UL, 0x4a7484aa6ea6e483UL, 0x5cb0a9dcbd41fbd4UL, 0x76f988da831153b5UL, 0x983e5152ee66dfabUL, 
    0xa831c66d2db43210UL, 0xb00327c898fb213fUL, 0xbf597fc7beef0ee4UL, 0xc6e00bf33da88fc2UL, 0xd5a79147930aa725UL, 
    0x06ca6351e003826fUL, 0x142929670a0e6e70UL, 0x27b70a8546d22ffcUL, 0x2e1b21385c26c926UL, 0x4d2c6dfc5ac42aedUL, 
    0x53380d139d95b3dfUL, 0x650a73548baf63deUL, 0x766a0abb3c77b2a8UL, 0x81c2c92e47edaee6UL, 0x92722c851482353bUL, 
    0xa2bfe8a14cf10364UL, 0xa81a664bbc423001UL, 0xc24b8b70d0f89791UL, 0xc76c51a30654be30UL, 0xd192e819d6ef5218UL, 
    0xd69906245565a910UL, 0xf40e35855771202aUL, 0x106aa07032bbd1b8UL, 0x19a4c116b8d2d0c8UL, 0x1e376c085141ab53UL, 
    0x2748774cdf8eeb99UL, 0x34b0bcb5e19b48a8UL, 0x391c0cb3c5c95a63UL, 0x4ed8aa4ae3418acbUL, 0x5b9cca4f7763e373UL, 
    0x682e6ff3d6b2b8a3UL, 0x748f82ee5defb2fcUL, 0x78a5636f43172f60UL, 0x84c87814a1f0ab72UL, 0x8cc702081a6439ecUL, 
    0x90befffa23631e28UL, 0xa4506cebde82bde9UL, 0xbef9a3f7b2c67915UL, 0xc67178f2e372532bUL, 0xca273eceea26619cUL, 
    0xd186b8c721c0c207UL, 0xeada7dd6cde0eb1eUL, 0xf57d4f7fee6ed178UL, 0x06f067aa72176fbaUL, 0x0a637dc5a2c898a6UL, 
    0x113f9804bef90daeUL, 0x1b710b35131c471bUL, 0x28db77f523047d84UL, 0x32caab7b40c72493UL, 0x3c9ebe0a15c9bebcUL, 
    0x431d67c49c100d4cUL, 0x4cc5d4becb3e42b6UL, 0x597f299cfc657e2aUL, 0x5fcb6fab3ad6faecUL, 0x6c44198c4a475817UL   
};

#define SHA256_STEP(F0a,F1a,a,b,c,d,e,f,g,h,x,K) { h += K; h += x; h += S3 (e); h += F1a (e,f,g); d += h; h += S2 (a); h += F0a (a,b,c); }
#define SHA512_STEP(a,b,c,d,e,f,g,h,x,K) { h += K + SHA512_S1(e) + F1(e, f, g) + x; d += h; h += SHA512_S0(a) + F0(a, b, c);}
#define ROUND_STEP_SHA512(i) { SHA512_STEP(a, b, c, d, e, f, g, h, W[i + 0], k_sha512[i +  0]); SHA512_STEP(h, a, b, c, d, e, f, g, W[i + 1], k_sha512[i +  1]); SHA512_STEP(g, h, a, b, c, d, e, f, W[i + 2], k_sha512[i +  2]); SHA512_STEP(f, g, h, a, b, c, d, e, W[i + 3], k_sha512[i +  3]); SHA512_STEP(e, f, g, h, a, b, c, d, W[i + 4], k_sha512[i +  4]); SHA512_STEP(d, e, f, g, h, a, b, c, W[i + 5], k_sha512[i +  5]); SHA512_STEP(c, d, e, f, g, h, a, b, W[i + 6], k_sha512[i +  6]); SHA512_STEP(b, c, d, e, f, g, h, a, W[i + 7], k_sha512[i +  7]); SHA512_STEP(a, b, c, d, e, f, g, h, W[i + 8], k_sha512[i +  8]); SHA512_STEP(h, a, b, c, d, e, f, g, W[i + 9], k_sha512[i +  9]); SHA512_STEP(g, h, a, b, c, d, e, f, W[i + 10], k_sha512[i + 10]); SHA512_STEP(f, g, h, a, b, c, d, e, W[i + 11], k_sha512[i + 11]); SHA512_STEP(e, f, g, h, a, b, c, d, W[i + 12], k_sha512[i + 12]); SHA512_STEP(d, e, f, g, h, a, b, c, W[i + 13], k_sha512[i + 13]); SHA512_STEP(c, d, e, f, g, h, a, b, W[i + 14], k_sha512[i + 14]); SHA512_STEP(b, c, d, e, f, g, h, a, W[i + 15], k_sha512[i + 15]); }
#define SHA256_EXPAND(x,y,z,w) (S1 (x) + y + S0 (z) + w) 

static void sha256_process2 (const uint *W, uint *digest) {
  uint a = digest[0];
  uint b = digest[1];
  uint c = digest[2];
  uint d = digest[3];
  uint e = digest[4];
  uint f = digest[5];
  uint g = digest[6];
  uint h = digest[7];

  uint w0_t = W[0];
  uint w1_t = W[1];
  uint w2_t = W[2];
  uint w3_t = W[3];
  uint w4_t = W[4];
  uint w5_t = W[5];
  uint w6_t = W[6];
  uint w7_t = W[7];
  uint w8_t = W[8];
  uint w9_t = W[9];
  uint wa_t = W[10];
  uint wb_t = W[11];
  uint wc_t = W[12];
  uint wd_t = W[13];
  uint we_t = W[14];
  uint wf_t = W[15];

  #define ROUND_EXPAND(i) { w0_t = SHA256_EXPAND (we_t, w9_t, w1_t, w0_t); w1_t = SHA256_EXPAND (wf_t, wa_t, w2_t, w1_t); w2_t = SHA256_EXPAND (w0_t, wb_t, w3_t, w2_t); w3_t = SHA256_EXPAND (w1_t, wc_t, w4_t, w3_t); w4_t = SHA256_EXPAND (w2_t, wd_t, w5_t, w4_t); w5_t = SHA256_EXPAND (w3_t, we_t, w6_t, w5_t); w6_t = SHA256_EXPAND (w4_t, wf_t, w7_t, w6_t); w7_t = SHA256_EXPAND (w5_t, w0_t, w8_t, w7_t); w8_t = SHA256_EXPAND (w6_t, w1_t, w9_t, w8_t); w9_t = SHA256_EXPAND (w7_t, w2_t, wa_t, w9_t); wa_t = SHA256_EXPAND (w8_t, w3_t, wb_t, wa_t); wb_t = SHA256_EXPAND (w9_t, w4_t, wc_t, wb_t); wc_t = SHA256_EXPAND (wa_t, w5_t, wd_t, wc_t); wd_t = SHA256_EXPAND (wb_t, w6_t, we_t, wd_t); we_t = SHA256_EXPAND (wc_t, w7_t, wf_t, we_t); wf_t = SHA256_EXPAND (wd_t, w8_t, w0_t, wf_t); }
  #define ROUND_STEP(i) { SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w0_t, k_sha256[i +  0]); SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w1_t, k_sha256[i +  1]); SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, w2_t, k_sha256[i +  2]); SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, w3_t, k_sha256[i +  3]); SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, w4_t, k_sha256[i +  4]); SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, w5_t, k_sha256[i +  5]); SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, w6_t, k_sha256[i +  6]); SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, w7_t, k_sha256[i +  7]); SHA256_STEP (F0, F1, a, b, c, d, e, f, g, h, w8_t, k_sha256[i +  8]); SHA256_STEP (F0, F1, h, a, b, c, d, e, f, g, w9_t, k_sha256[i +  9]); SHA256_STEP (F0, F1, g, h, a, b, c, d, e, f, wa_t, k_sha256[i + 10]); SHA256_STEP (F0, F1, f, g, h, a, b, c, d, e, wb_t, k_sha256[i + 11]); SHA256_STEP (F0, F1, e, f, g, h, a, b, c, d, wc_t, k_sha256[i + 12]); SHA256_STEP (F0, F1, d, e, f, g, h, a, b, c, wd_t, k_sha256[i + 13]); SHA256_STEP (F0, F1, c, d, e, f, g, h, a, b, we_t, k_sha256[i + 14]); SHA256_STEP (F0, F1, b, c, d, e, f, g, h, a, wf_t, k_sha256[i + 15]); }

  ROUND_STEP (0);
  ROUND_EXPAND();
  ROUND_STEP(16);
  ROUND_EXPAND();
  ROUND_STEP(32);
  ROUND_EXPAND();
  ROUND_STEP(48);

  digest[0] += a;
  digest[1] += b;
  digest[2] += c;
  digest[3] += d;
  digest[4] += e;
  digest[5] += f;
  digest[6] += g;
  digest[7] += h;
}
void sha512( ulong* input, const uint length,  ulong* hash) {
    //printf("---------------------------\n");
    const uint nBlocks = md_pad_128(input, (const ulong)length);    
    //printf("nBlocks %d\n", nBlocks);
    //printf("---------------------------\n");
    //for (int i = 0; i < 32; i++) 
    //    printf("input[%d] 0x%llX\n", i, input[i]);
    ulong W[0x50] = { 0 };
    ulong State[8] = { 0 };
    State[0] = 0x6a09e667f3bcc908UL;
    State[1] = 0xbb67ae8584caa73bUL;
    State[2] = 0x3c6ef372fe94f82bUL;
    State[3] = 0xa54ff53a5f1d36f1UL;
    State[4] = 0x510e527fade682d1UL;
    State[5] = 0x9b05688c2b3e6c1fUL;
    State[6] = 0x1f83d9abfb41bd6bUL;
    State[7] = 0x5be0cd19137e2179UL;
    ulong a, b, c, d, e, f, g, h;
    for (int block_i = 0; block_i < nBlocks; block_i++) {
        W[0] = SWAP512(input[0]);
        W[1] = SWAP512(input[1]);
        W[2] = SWAP512(input[2]);
        W[3] = SWAP512(input[3]);
        W[4] = SWAP512(input[4]);
        W[5] = SWAP512(input[5]);
        W[6] = SWAP512(input[6]);
        W[7] = SWAP512(input[7]);
        W[8] = SWAP512(input[8]);
        W[9] = SWAP512(input[9]);
        W[10] = SWAP512(input[10]);
        W[11] = SWAP512(input[11]);
        W[12] = SWAP512(input[12]);
        W[13] = SWAP512(input[13]);
        W[14] = SWAP512(input[14]);
        W[15] = SWAP512(input[15]);
        for (int i = 16; i < 80; i++) {
            W[i] = W[i - 16] + little_s0(W[i - 15]) + W[i - 7] + little_s1(W[i - 2]);
        }
        a = State[0];
        b = State[1];
        c = State[2];
        d = State[3];
        e = State[4];
        f = State[5];
        g = State[6];
        h = State[7];
        for (int i = 0; i < 80; i += 16) {
            ROUND_STEP_SHA512(i)
        }
        State[0] += a;
        State[1] += b;
        State[2] += c;
        State[3] += d;
        State[4] += e;
        State[5] += f;
        State[6] += g;
        State[7] += h;
        input += 16;
    }
    hash[0] = SWAP512(State[0]);
    hash[1] = SWAP512(State[1]);
    hash[2] = SWAP512(State[2]);
    hash[3] = SWAP512(State[3]);
    hash[4] = SWAP512(State[4]);
    hash[5] = SWAP512(State[5]);
    hash[6] = SWAP512(State[6]);
    hash[7] = SWAP512(State[7]);
    return;
}

__kernel void sha512_GPU(__global ulong *input, const uint length, __global ulong *hash) {
   ulong W[0x50];
   ulong State[8];

   //for (int i = 0; i < 0x50; i++) W[i] = 0;
   const uint nBlocks = md_pad_128_global(input, (const ulong)length);

   __global ulong *inpt = input;
      //for (int i = 0; i < 32; i++) 
      //  printf("input[%d] 0x%llX\n", i, input[i]);
  State[0] = 0x6a09e667f3bcc908UL;
  State[1] = 0xbb67ae8584caa73bUL;
  State[2] = 0x3c6ef372fe94f82bUL;
  State[3] = 0xa54ff53a5f1d36f1UL;
  State[4] = 0x510e527fade682d1UL;
  State[5] = 0x9b05688c2b3e6c1fUL;
  State[6] = 0x1f83d9abfb41bd6bUL;
  State[7] = 0x5be0cd19137e2179UL;
  ulong a,b,c,d,e,f,g,h;

  for (int block_i = 0; block_i < nBlocks; block_i++) {
    REVERSE64(inpt[0], W[0]);
    REVERSE64(inpt[1], W[1]);
    REVERSE64(inpt[2], W[2]);
    REVERSE64(inpt[3], W[3]);
    REVERSE64(inpt[4], W[4]);
    REVERSE64(inpt[5], W[5]);
    REVERSE64(inpt[6], W[6]);
    REVERSE64(inpt[7], W[7]);
    REVERSE64(inpt[8], W[8]);
    REVERSE64(inpt[9], W[9]);
    REVERSE64(inpt[10], W[10]);
    REVERSE64(inpt[11], W[11]);
    REVERSE64(inpt[12], W[12]);
    REVERSE64(inpt[13], W[13]);
    REVERSE64(inpt[14], W[14]);
    REVERSE64(inpt[15], W[15]);
    for (int i = 16; i < 80; i++) {
      W[i] = W[i-16] + little_s0(W[i-15]) + W[i-7] + little_s1(W[i-2]);
    }
    a = State[0];
    b = State[1];
    c = State[2];
    d = State[3];
    e = State[4];
    f = State[5];
    g = State[6];
    h = State[7];
    for (int i = 0; i < 80; i += 16) {
      ROUND_STEP_SHA512(i)
    }
    State[0] += a;
    State[1] += b;
    State[2] += c;
    State[3] += d;
    State[4] += e;
    State[5] += f;
    State[6] += g;
    State[7] += h;
    inpt += 16;
  }

  REVERSE64(State[0], hash[0]);
  REVERSE64(State[1], hash[1]);
  REVERSE64(State[2], hash[2]);
  REVERSE64(State[3], hash[3]);
  REVERSE64(State[4], hash[4]);
  REVERSE64(State[5], hash[5]);
  REVERSE64(State[6], hash[6]);
  REVERSE64(State[7], hash[7]);
  return;
}

static void sha256(__private const uint *pass, int pass_len, __private uint* hash) {
  int plen=pass_len/4;
  if (mod(pass_len,4)) plen++; 
  __private uint* p = hash;
  uint W[0x10]={0};
  int loops=plen;
  int curloop=0;
  uint State[8]={0};
  State[0] = 0x6a09e667;
  State[1] = 0xbb67ae85;
  State[2] = 0x3c6ef372;
  State[3] = 0xa54ff53a;
  State[4] = 0x510e527f;
  State[5] = 0x9b05688c;
  State[6] = 0x1f83d9ab;
  State[7] = 0x5be0cd19;
  while (loops>0) {
    W[0x0]=0x0;
    W[0x1]=0x0;
    W[0x2]=0x0;
    W[0x3]=0x0;
    W[0x4]=0x0;
    W[0x5]=0x0;
    W[0x6]=0x0;
    W[0x7]=0x0;
    W[0x8]=0x0;
    W[0x9]=0x0;
    W[0xA]=0x0;
    W[0xB]=0x0;
    W[0xC]=0x0;
    W[0xD]=0x0;
    W[0xE]=0x0;
    W[0xF]=0x0;
    for (int m=0;loops!=0 && m<16;m++) {
      W[m]^=SWAP256(pass[m+(curloop*16)]);
      loops--;
    }
    if (loops==0 && mod(pass_len,64)!=0) {
      uint padding=0x80<<(((pass_len+4)-((pass_len+4)/4*4))*8);
      int v=mod(pass_len,64);
      W[v/4]|=SWAP256(padding);
      if ((pass_len&0x3B)!=0x3B) {
        W[0x0F]=pass_len*8;
      }
    }
    sha256_process2(W,State);
    curloop++;
  }
  if (mod(plen,16)==0) {
    W[0x0]=0x0;
    W[0x1]=0x0;
    W[0x2]=0x0;
    W[0x3]=0x0;
    W[0x4]=0x0;
    W[0x5]=0x0;
    W[0x6]=0x0;
    W[0x7]=0x0;
    W[0x8]=0x0;
    W[0x9]=0x0;
    W[0xA]=0x0;
    W[0xB]=0x0;
    W[0xC]=0x0;
    W[0xD]=0x0;
    W[0xE]=0x0;
    W[0xF]=0x0;
    if ((pass_len&0x3B)!=0x3B) {
      uint padding=0x80<<(((pass_len+4)-((pass_len+4)/4*4))*8);
      W[0]|=SWAP256(padding);
    }
    W[0x0F]=pass_len*8;
    sha256_process2(W,State);
  }
  p[0]=SWAP256(State[0]);
  p[1]=SWAP256(State[1]);
  p[2]=SWAP256(State[2]);
  p[3]=SWAP256(State[3]);
  p[4]=SWAP256(State[4]);
  p[5]=SWAP256(State[5]);
  p[6]=SWAP256(State[6]);
  p[7]=SWAP256(State[7]);
  return;
}

#undef F0
#undef F1
#undef S0
#undef S1
#undef S2
#undef S3

#undef mod
#undef shr32
#undef rotl32

#endif //_SHA2_CL