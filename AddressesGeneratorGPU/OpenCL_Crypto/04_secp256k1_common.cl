
#ifndef _SECP256K1_COMMON_CL
#define _SECP256K1_COMMON_CL

#define ECMULT_GEN_PREC_BITS 2
#define ECMULT_GEN_PREC_B ECMULT_GEN_PREC_BITS
#define ECMULT_GEN_PREC_G (1 << ECMULT_GEN_PREC_B)
#define ECMULT_GEN_PREC_N (256 / ECMULT_GEN_PREC_B)
#define ECMULT_WINDOW_SIZE 15
#define WINDOW_A 5
#define WINDOW_G ECMULT_WINDOW_SIZE
#define ECMULT_TABLE_SIZE(w) (1 << ((w)-2))
#define RIPEMD160_BLOCK_LENGTH 64
#define RIPEMD160_DIGEST_LENGTH 20
#define SECP256K1_FLAGS_TYPE_MASK ((1 << 8) - 1)
#define SECP256K1_FLAGS_TYPE_CONTEXT (1 << 0)
#define SECP256K1_FLAGS_TYPE_COMPRESSION (1 << 1)
#define SECP256K1_FLAGS_BIT_CONTEXT_VERIFY (1 << 8)
#define SECP256K1_FLAGS_BIT_CONTEXT_SIGN (1 << 9)
#define SECP256K1_FLAGS_BIT_CONTEXT_DECLASSIFY (1 << 10)
#define SECP256K1_FLAGS_BIT_COMPRESSION (1 << 8)
#define SECP256K1_EC_COMPRESSED (SECP256K1_FLAGS_TYPE_COMPRESSION | SECP256K1_FLAGS_BIT_COMPRESSION)
#define SECP256K1_EC_UNCOMPRESSED (SECP256K1_FLAGS_TYPE_COMPRESSION)
#define SECP256K1_TAG_PUBKEY_EVEN 0x02
#define SECP256K1_TAG_PUBKEY_ODD 0x03
#define SECP256K1_TAG_PUBKEY_UNCOMPRESSED 0x04
#define SECP256K1_TAG_PUBKEY_HYBRID_EVEN 0x06
#define SECP256K1_TAG_PUBKEY_HYBRID_ODD 0x07
#define SECP256K1_FE_CONST_INNER(d7, d6, d5, d4, d3, d2, d1, d0) { (d0) & 0x3FFFFFFUL, (((uint)d0) >> 26) | (((uint)(d1) & 0xFFFFFUL) << 6), (((uint)d1) >> 20) | (((uint)(d2) & 0x3FFFUL) << 12), (((uint)d2) >> 14) | (((uint)(d3) & 0xFFUL) << 18), (((uint)d3) >> 8) | (((uint)(d4) & 0x3UL) << 24), (((uint)d4) >> 2) & 0x3FFFFFFUL,(((uint)d4) >> 28) | (((uint)(d5) & 0x3FFFFFUL) << 4), (((uint)d5) >> 22) | (((uint)(d6) & 0xFFFFUL) << 10), (((uint)d6) >> 16) | (((uint)(d7) & 0x3FFUL) << 16), (((uint)d7) >> 10) }
#define SECP256K1_SCALAR_CONST(d7, d6, d5, d4, d3, d2, d1, d0) {{(d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7)}}
#define SECP256K1_FE_CONST(d7, d6, d5, d4, d3, d2, d1, d0) {SECP256K1_FE_CONST_INNER((d7), (d6), (d5), (d4), (d3), (d2), (d1), (d0))}
#define SECP256K1_GE_CONST(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) {SECP256K1_FE_CONST((a),(b),(c),(d),(e),(f),(g),(h)), SECP256K1_FE_CONST((i),(j),(k),(l),(m),(n),(o),(p)), 0}
#define SECP256K1_GE_CONST_INFINITY {SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), SECP256K1_FE_CONST(0, 0, 0, 0, 0, 0, 0, 0), 1}
#define SECP256K1_N_0 ((uint)0xD0364141UL)
#define SECP256K1_N_1 ((uint)0xBFD25E8CUL)
#define SECP256K1_N_2 ((uint)0xAF48A03BUL)
#define SECP256K1_N_3 ((uint)0xBAAEDCE6UL)
#define SECP256K1_N_4 ((uint)0xFFFFFFFEUL)
#define SECP256K1_N_5 ((uint)0xFFFFFFFFUL)
#define SECP256K1_N_6 ((uint)0xFFFFFFFFUL)
#define SECP256K1_N_7 ((uint)0xFFFFFFFFUL)
#define SECP256K1_N_C_0 (~SECP256K1_N_0 + 1)
#define SECP256K1_N_C_1 (~SECP256K1_N_1)
#define SECP256K1_N_C_2 (~SECP256K1_N_2)
#define SECP256K1_N_C_3 (~SECP256K1_N_3)
#define SECP256K1_N_C_4 (1)

typedef struct {
  uint n[10];
} secp256k1_fe;

typedef struct {
  uint n[8];
} secp256k1_fe_storage;

typedef struct {
  uint d[8];
} secp256k1_scalar;

typedef struct {
  secp256k1_fe x;
  secp256k1_fe y;
  int infinity;
} secp256k1_ge;

typedef struct {
  secp256k1_fe x;
  secp256k1_fe y;
  secp256k1_fe z;
  int infinity;
} secp256k1_gej;

typedef struct {
  secp256k1_fe_storage x;
  secp256k1_fe_storage y;
} secp256k1_ge_storage;

typedef struct {
  unsigned char data[64];
} secp256k1_pubkey;

#define SECP256K1_FE_STORAGE_CONST(d7, d6, d5, d4, d3, d2, d1, d0) {{ (d0), (d1), (d2), (d3), (d4), (d5), (d6), (d7) }}
#define SECP256K1_GE_STORAGE_CONST(a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p) {SECP256K1_FE_STORAGE_CONST((a),(b),(c),(d),(e),(f),(g),(h)), SECP256K1_FE_STORAGE_CONST((i),(j),(k),(l),(m),(n),(o),(p))}
#define SC SECP256K1_GE_STORAGE_CONST

#endif //_SECP256K1_COMMON_CL