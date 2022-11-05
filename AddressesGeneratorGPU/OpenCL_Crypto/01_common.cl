
#ifndef _COMMON_CL
#define _COMMON_CL
//#define uint32_t uint
//#define uint64_t ulong
#define uchar unsigned char  
//#define NULL 0

static void memset(uchar* str, int c, size_t n) {
	for (int i = 0; i < n; i++) {
		str[i] = c;
	}
}

static void memcpy(uchar* dest, const uchar* src, size_t n) {
	for (int i = 0; i < n; i++) {
		dest[i] = src[i];
	}
}

static void memcpy_offset(uchar* dest, const uchar* src, int offset, uchar bytes_to_copy) {
	for (int i = 0; i < bytes_to_copy; i++) {
		dest[i] = src[offset + i];
	}
}

static void memzero(void* const pnt, const size_t len) {
	volatile uchar* volatile pnt_ = (volatile uchar* volatile)pnt;
	size_t i = (size_t)0U;

	while (i < len) {
		pnt_[i++] = 0U;
	}
}

static void memczero(void* s, size_t len, int flag) {
	uchar* p = (uchar*)s;
	volatile int vflag = flag;
	unsigned char mask = -(unsigned char)vflag;
	while (len) {
		*p &= ~mask;
		p++;
		len--;
	}
}

void copy_pad_previous(const uchar* pad, const uchar* previous, uchar* joined) {
	for (int x = 0; x < 128; x++) {
		joined[x] = pad[x];
	}
	for (int x = 0; x < 64; x++) {
		joined[x + 128] = previous[x];
	}
}

inline void copy_pad_previous_GPU(__global uchar* pad, __global uchar* previous, __global uchar* joined) {
	for (int x = 0; x < 128; x++) {
		joined[x] = pad[x];
	}
	for (int x = 0; x < 64; x++) {
		joined[x + 128] = previous[x];
	}
}

void print_byte_array_hex(uchar* arr, int len) {
	for (int i = 0; i < len; i++) {
		printf("%02x", arr[i]);
	}
	printf("\n\n");
}

void xor_seed_with_round(char* seed, char* round) {
	for (int x = 0; x < 64; x++) {
		seed[x] = seed[x] ^ round[x];
	}
}
__kernel void xor_seed_with_round_GPU(__global char* seed, __global char* round) {
	for (int x = 0; x < 64; x++) {
		seed[x] = seed[x] ^ round[x];
	}
}
void xor_seed_with_round_GPUTest(__global uchar* seed, uchar* round) {
	for (int x = 0; x < 64; x++) {
		seed[x] = seed[x] ^ round[x];
	}
}

void print_seed(uchar* seed) {
	printf("seed: ");
	print_byte_array_hex(seed, 64);
}

void print_raw_address(uchar* address) {
	printf("address: ");
	print_byte_array_hex(address, 25);
}

void print_mnemonic(uchar* mnemonic) {
	printf("mnemonic: ");
	for (int i = 0; i < 120; i++) {
		printf("%c", mnemonic[i]);
	}
	printf("\n");
}

void print_byte_array(uchar* arr, int len) {
	printf("[");
	for (int x = 0; x < len; x++) {
		printf("%u", arr[x]);
		if (x < len - 1) {
			printf(", ");
		}
	}
	printf("]\n");
}

#endif //_COMMON_CL
