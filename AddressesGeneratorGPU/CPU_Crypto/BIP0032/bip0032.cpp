// TODO: Add OpenCL kernel code here.

#define DEBUG_TEST_GENERATE_BIP32	1
#define SERIALIZE_PRIVATE	0
#define SERIALIZE_PUBLIC	1


#include "bip0032.h"
#include "SHA_2/sha256.h"
#include "SHA_2/sha512.h"
#include "trezor-crypto/ripemd160.h"
#include "trezor-crypto/segwit_addr.h"
#include "CL/cl.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
/**************************** DATA TYPES ****************************/

const char b58digits_ordered[] =
"123456789ABCDEFGHJKLMNPQRSTUVWXYZabcdefghijkmnopqrstuvwxyz";

static void write_be(uint8_t* data, uint32_t x)
{
    data[0] = x >> 24;
    data[1] = x >> 16;
    data[2] = x >> 8;
    data[3] = x;
}


bool b58enc(char* b58, size_t* b58sz, const void* data, size_t binsz) {
    const uint8_t* bin = (const uint8_t*)data;
    int carry = 0;
    size_t i = 0, j = 0, high = 0, zcount = 0;
    size_t size = 0;

    while (zcount < binsz && !bin[zcount]) ++zcount;

    size = (binsz - zcount) * 138 / 100 + 1;
    uint8_t* buf;
    if (!(buf = (uint8_t*)alloca(size))) {
        return 0;
    }
    //uint8_t buf[size];
    memset(buf, 0, size);

    for (i = zcount, high = size - 1; i < binsz; ++i, high = j) {
        for (carry = bin[i], j = size - 1; (j > high) || carry; --j) {
            carry += 256 * buf[j];
            buf[j] = carry % 58;
            carry /= 58;
            if (!j) {
                // Otherwise j wraps to maxint which is > high
                break;
            }
        }
    }

    for (j = 0; j < size && !buf[j]; ++j)
        ;

    if (*b58sz <= zcount + size - j) {
        *b58sz = zcount + size - j + 1;
        return false;
    }

    if (zcount) memset(b58, '1', zcount);
    for (i = zcount; j < size; ++i, ++j) b58[i] = b58digits_ordered[buf[j]];
    b58[i] = '\0';
    *b58sz = i + 1;

    return true;
}

int base58_encode_check(const uint8_t* data, int datalen, char* str, int strsize) {
    uint8_t buf[21 + 32];
    memset(buf, 0, 21 + 32);
    uint8_t* hash = buf + 21;
    memcpy(buf, data, 21);


#define HASHER_DIGEST_LENGTH 32
    sha256_Raw(data, 21, hash);
    sha256_Raw(hash, HASHER_DIGEST_LENGTH, hash);

    size_t res = strsize;
    b58enc(str, &res, buf, 21 + 4);
    return 0;
}

bool btc_hdnode_from_seed(const secp256k1_context* secp256k1_ctx, const uint8_t* seed, btc_hdnode* out)
{
    uint8_t I[BTC_ECKEY_PKEY_LENGTH + BTC_BIP32_CHAINCODE_SIZE];
    memset(out, 0, sizeof(btc_hdnode));
    out->depth = 0;
    out->fingerprint = 0x00000000;
    out->child_num = 0;
    hmac_sha512((const uint8_t*)"Bitcoin seed", 12, seed, 64, I);
    memcpy(out->private_key, I, BTC_ECKEY_PKEY_LENGTH);

    //if (!btc_ecc_verify_privatekey(out->private_key)) {
    //    return false;
    //}
    //printf("privkey: ");
    //for (int i = 0; i < 32; i++) {
    //    printf("%02X ", out->private_key[i]);
    //}
    //printf("\n");

    memcpy(out->chain_code, I + BTC_ECKEY_PKEY_LENGTH, BTC_BIP32_CHAINCODE_SIZE);
    //btc_hdnode_fill_public_key(out);

    btc_ecc_get_compressed_pubkey(secp256k1_ctx, out->private_key, out->public_key);

    return true;
}

void btc_ecc_get_compressed_pubkey(const secp256k1_context* secp256k1_ctx, const uint8_t* private_key, uint8_t* public_key)
{
    //unsigned char pubkey[33];
    //cl_uint in_outlen = 33;
    //uint32_t privkey_scalar[8];

    ////secp256k1_scalar_set_b32
    //const uint8_t* b32 = private_key;
    //privkey_scalar[0] = (uint32_t)b32[31] | (uint32_t)b32[30] << 8 | (uint32_t)b32[29] << 16 | (uint32_t)b32[28] << 24;
    //privkey_scalar[1] = (uint32_t)b32[27] | (uint32_t)b32[26] << 8 | (uint32_t)b32[25] << 16 | (uint32_t)b32[24] << 24;
    //privkey_scalar[2] = (uint32_t)b32[23] | (uint32_t)b32[22] << 8 | (uint32_t)b32[21] << 16 | (uint32_t)b32[20] << 24;
    //privkey_scalar[3] = (uint32_t)b32[19] | (uint32_t)b32[18] << 8 | (uint32_t)b32[17] << 16 | (uint32_t)b32[16] << 24;
    //privkey_scalar[4] = (uint32_t)b32[15] | (uint32_t)b32[14] << 8 | (uint32_t)b32[13] << 16 | (uint32_t)b32[12] << 24;
    //privkey_scalar[5] = (uint32_t)b32[11] | (uint32_t)b32[10] << 8 | (uint32_t)b32[9] << 16 | (uint32_t)b32[8] << 24;
    //privkey_scalar[6] = (uint32_t)b32[7] | (uint32_t)b32[6] << 8 | (uint32_t)b32[5] << 16 | (uint32_t)b32[4] << 24;
    //privkey_scalar[7] = (uint32_t)b32[3] | (uint32_t)b32[2] << 8 | (uint32_t)b32[1] << 16 | (uint32_t)b32[0] << 24;

    //uint64_t t;
    //t = (uint64_t)privkey_scalar[0];
    //privkey_scalar[0] = t & 0xFFFFFFFFUL; t >>= 32;
    //t += (uint64_t)privkey_scalar[1];
    //privkey_scalar[1] = t & 0xFFFFFFFFUL; t >>= 32;
    //t += (uint64_t)privkey_scalar[2];
    //privkey_scalar[2] = t & 0xFFFFFFFFUL; t >>= 32;
    //t += (uint64_t)privkey_scalar[3];
    //privkey_scalar[3] = t & 0xFFFFFFFFUL; t >>= 32;
    //t += (uint64_t)privkey_scalar[4];
    //privkey_scalar[4] = t & 0xFFFFFFFFUL; t >>= 32;
    //t += (uint64_t)privkey_scalar[5];
    //privkey_scalar[5] = t & 0xFFFFFFFFUL; t >>= 32;
    //t += (uint64_t)privkey_scalar[6];
    //privkey_scalar[6] = t & 0xFFFFFFFFUL; t >>= 32;
    //t += (uint64_t)privkey_scalar[7];
    //privkey_scalar[7] = t & 0xFFFFFFFFUL;

    ////secp256k1_scalar_cmov
    //static const uint32_t secp256k1_scalar_one[8] = { 0, 0, 0, 0, 0, 0, 0, 1 };
    //uint32_t mask0 = 0XFFFFFFFF;
    //uint32_t mask1 = 0;
    //privkey_scalar[0] = (privkey_scalar[0] & mask0) | (secp256k1_scalar_one[0] & mask1);
    //privkey_scalar[1] = (privkey_scalar[1] & mask0) | (secp256k1_scalar_one[1] & mask1);
    //privkey_scalar[2] = (privkey_scalar[2] & mask0) | (secp256k1_scalar_one[2] & mask1);
    //privkey_scalar[3] = (privkey_scalar[3] & mask0) | (secp256k1_scalar_one[3] & mask1);
    //privkey_scalar[4] = (privkey_scalar[4] & mask0) | (secp256k1_scalar_one[4] & mask1);
    //privkey_scalar[5] = (privkey_scalar[5] & mask0) | (secp256k1_scalar_one[5] & mask1);
    //privkey_scalar[6] = (privkey_scalar[6] & mask0) | (secp256k1_scalar_one[6] & mask1);
    //privkey_scalar[7] = (privkey_scalar[7] & mask0) | (secp256k1_scalar_one[7] & mask1);

    //secp256k1_gej pj;
    //uint32_t gnb[8];
    //t = (uint64_t)a->d[0] + privkey_scalar[0];
    //gnb[0] = t & 0xFFFFFFFFULL; t >>= 32;
    //t += (uint64_t)a->d[1] + privkey_scalar[1];
    //gnb[1] = t & 0xFFFFFFFFULL; t >>= 32;
    //t += (uint64_t)a->d[2] + privkey_scalar[2];
    //gnb[2] = t & 0xFFFFFFFFULL; t >>= 32;
    //t += (uint64_t)a->d[3] + privkey_scalar[3];
    //gnb[3] = t & 0xFFFFFFFFULL; t >>= 32;
    //t += (uint64_t)a->d[4] + privkey_scalar[4];
    //gnb[4] = t & 0xFFFFFFFFULL; t >>= 32;
    //t += (uint64_t)a->d[5] + privkey_scalar[5];
    //gnb[5] = t & 0xFFFFFFFFULL; t >>= 32;
    //t += (uint64_t)a->d[6] + privkey_scalar[6];
    //gnb[6] = t & 0xFFFFFFFFULL; t >>= 32;
    //t += (uint64_t)a->d[7] + privkey_scalar[7];
    //gnb[7] = t & 0xFFFFFFFFULL; t >>= 32;



    secp256k1_pubkey pubkey;
    size_t in_outlen = 33;

    if (!secp256k1_ec_pubkey_create(secp256k1_ctx, &pubkey, (const unsigned char*)private_key)) {
        return;
    }

    if (!secp256k1_ec_pubkey_serialize(secp256k1_ctx, public_key, &in_outlen, &pubkey, SECP256K1_EC_COMPRESSED)) {
        return;
    }
    return;
}

uint8_t btc_hd_generate_key(const secp256k1_context* secp256k1_ctx, btc_hdnode* node, const char* keypath, const uint8_t* keymaster, const uint8_t* chaincode)
{
    static char delim[] = "/";
    static char prime[] = "phH\'";
    static char digits[] = "0123456789";
    uint64_t idx = 0;

    char* pch;
    char kp[] = "m/XX'/0'/0'/0/0";

    //if (!kp) {
    //    return false;
    //}


    memset(kp, 0, strlen(keypath) + 1);
    memcpy(kp, keypath, strlen(keypath));

    if (kp[0] != 'm' || kp[1] != '/') {
        goto err;
    }

    node->depth = 0;
    node->child_num = 0;
    node->fingerprint = 0;
    memcpy(node->chain_code, chaincode, BTC_BIP32_CHAINCODE_SIZE);
    memcpy(node->private_key, keymaster, BTC_ECKEY_PKEY_LENGTH);
    btc_ecc_get_compressed_pubkey(secp256k1_ctx, node->private_key, node->public_key);


    pch = strtok(kp + 2, delim);
    while (pch != NULL) {
        size_t i = 0;
        int prm = 0;
        for (; i < strlen(pch); i++) {
            if (strchr(prime, pch[i])) {
                prm = 1;
            }
            else if (!strchr(digits, pch[i])) {
                goto err;
            }
        }

        idx = strtoull(pch, NULL, 10);
        //if (idx > UINT32_MAX) {
        //    goto err;
        //}

        if (prm) {
            if (btc_hdnode_private_ckd(secp256k1_ctx, node, (uint32_t)((idx) | 0x80000000)) != (uint8_t)true) {
                goto err;
            }
        }
        else {
            if (btc_hdnode_private_ckd(secp256k1_ctx, node, idx) != (uint8_t)true) {
                goto err;
            }
        }
        pch = strtok(NULL, delim);
    }

    return true;

err:
    return false;
}

uint8_t btc_hdnode_private_ckd(const secp256k1_context* secp256k1_ctx, btc_hdnode* inout, uint32_t i)
{
    uint8_t data[1 + BTC_ECKEY_PKEY_LENGTH + 4];
    uint8_t I[BTC_ECKEY_PKEY_LENGTH + BTC_BIP32_CHAINCODE_SIZE];
    uint8_t fingerprint[BTC_BIP32_CHAINCODE_SIZE];
    uint8_t p[BTC_ECKEY_PKEY_LENGTH], z[BTC_ECKEY_PKEY_LENGTH];

    if (i & 0x80000000) { // private derivation
        data[0] = 0;
        memcpy(data + 1, inout->private_key, BTC_ECKEY_PKEY_LENGTH);
    }
    else { // public derivation
        memcpy(data, inout->public_key, BTC_ECKEY_COMPRESSED_LENGTH);
    }
    write_be(data + BTC_ECKEY_COMPRESSED_LENGTH, i);
    sha256_Raw(inout->public_key, BTC_ECKEY_COMPRESSED_LENGTH, fingerprint);
    ripemd160(fingerprint, 32, fingerprint);
    inout->fingerprint = (fingerprint[0] << 24) + (fingerprint[1] << 16) +
        (fingerprint[2] << 8) + fingerprint[3];

    memset(fingerprint, 0, sizeof(fingerprint));
    memcpy(p, inout->private_key, BTC_ECKEY_PKEY_LENGTH);

    hmac_sha512(inout->chain_code, BTC_BIP32_CHAINCODE_SIZE, data, sizeof(data), I);
    memcpy(inout->chain_code, I + BTC_ECKEY_PKEY_LENGTH, BTC_BIP32_CHAINCODE_SIZE);
    memcpy(inout->private_key, I, BTC_ECKEY_PKEY_LENGTH);

    memcpy(z, inout->private_key, BTC_ECKEY_PKEY_LENGTH);

    int failed = 0;
    //if (!btc_ecc_verify_privatekey(z)) {
    //    failed = 1;
    //    return false;
    //}

    memcpy(inout->private_key, p, BTC_ECKEY_PKEY_LENGTH);


    if (!secp256k1_ec_privkey_tweak_add(secp256k1_ctx, (unsigned char*)inout->private_key, (const unsigned char*)z)) {
        failed = 1;
    }

    if (!failed) {
        inout->depth++;
        inout->child_num = i;
        btc_ecc_get_compressed_pubkey(secp256k1_ctx, inout->private_key, inout->public_key);
    }

    return true;
}