#include "03_sha2.cl"
#include "05_secp256k1_scalar.cl"
#include "07_secp256k1_group.cl"
#include "08_secp256k1_prec.cl"

static void secp256k1_ecmult_gen(secp256k1_gej *r,  secp256k1_scalar *gn) {
    secp256k1_ge add;
    secp256k1_ge_storage adds;
    int bits;
    int i, j;

    memset((uchar *)&adds, 0, sizeof(adds));
    secp256k1_gej_set_infinity(r);

    add.infinity = 0;
    for (j = 0; j < ECMULT_GEN_PREC_N; j++) {
        bits = secp256k1_scalar_get_bits(gn, j * ECMULT_GEN_PREC_B, ECMULT_GEN_PREC_B);
        for (i = 0; i < ECMULT_GEN_PREC_G; i++) {
            uint mask0, mask1;
            mask0 = (i == bits) + ~((uint)0);
            mask1 = ~mask0;
            
            adds.x.n[0] = (adds.x.n[0] & mask0) | (prec[j][i].x.n[0] & mask1);
            adds.x.n[1] = (adds.x.n[1] & mask0) | (prec[j][i].x.n[1] & mask1);
            adds.x.n[2] = (adds.x.n[2] & mask0) | (prec[j][i].x.n[2] & mask1);
            adds.x.n[3] = (adds.x.n[3] & mask0) | (prec[j][i].x.n[3] & mask1);
            adds.x.n[4] = (adds.x.n[4] & mask0) | (prec[j][i].x.n[4] & mask1);
            adds.x.n[5] = (adds.x.n[5] & mask0) | (prec[j][i].x.n[5] & mask1);
            adds.x.n[6] = (adds.x.n[6] & mask0) | (prec[j][i].x.n[6] & mask1);
            adds.x.n[7] = (adds.x.n[7] & mask0) | (prec[j][i].x.n[7] & mask1);

            adds.y.n[0] = (adds.y.n[0] & mask0) | (prec[j][i].y.n[0] & mask1);
            adds.y.n[1] = (adds.y.n[1] & mask0) | (prec[j][i].y.n[1] & mask1);
            adds.y.n[2] = (adds.y.n[2] & mask0) | (prec[j][i].y.n[2] & mask1);
            adds.y.n[3] = (adds.y.n[3] & mask0) | (prec[j][i].y.n[3] & mask1);
            adds.y.n[4] = (adds.y.n[4] & mask0) | (prec[j][i].y.n[4] & mask1);
            adds.y.n[5] = (adds.y.n[5] & mask0) | (prec[j][i].y.n[5] & mask1);
            adds.y.n[6] = (adds.y.n[6] & mask0) | (prec[j][i].y.n[6] & mask1);
            adds.y.n[7] = (adds.y.n[7] & mask0) | (prec[j][i].y.n[7] & mask1);
        }
        secp256k1_ge_from_storage(&add, &adds);
        secp256k1_gej_add_ge(r, r, &add);
    }
    bits = 0;
    secp256k1_ge_clear(&add);
}

static void secp256k1_pubkey_save(secp256k1_pubkey* pubkey, secp256k1_ge* ge) {
  secp256k1_fe_normalize_var(&ge->x);
  secp256k1_fe_normalize_var(&ge->y);
  secp256k1_fe_get_b32(pubkey->data, &ge->x);
  secp256k1_fe_get_b32(pubkey->data + 32, &ge->y);
}

int secp256k1_ec_pubkey_create(secp256k1_pubkey *pubkey, const uchar *seckey) {
    secp256k1_gej pj;
    secp256k1_ge p;
    secp256k1_scalar sec;
    secp256k1_scalar secp256k1_scalar_one = SECP256K1_SCALAR_CONST(0, 0, 0, 0, 0, 0, 0, 1);
    int ret = 0;

    memset((uchar *)pubkey, 0, sizeof(*pubkey));
    
    ret = secp256k1_scalar_set_b32_seckey(&sec, seckey);
    
    secp256k1_scalar_cmov(&sec, &secp256k1_scalar_one, !ret);

    secp256k1_ecmult_gen(&pj, &sec);
    secp256k1_ge_set_gej(&p, &pj);
    secp256k1_pubkey_save(pubkey, &p);
    
    memczero(pubkey, sizeof(*pubkey), !ret);

    secp256k1_scalar_clear(&sec);
    return ret;
}

static int secp256k1_eckey_privkey_tweak_add(secp256k1_scalar *key, const secp256k1_scalar *tweak) {
    secp256k1_scalar_add(key, key, tweak);
    return !secp256k1_scalar_is_zero(key);
}

int secp256k1_ec_seckey_tweak_add(uchar *seckey, const uchar *tweak) {
  secp256k1_scalar term;
  secp256k1_scalar sec;
  int ret = 0;
  int overflow = 0;
  secp256k1_scalar_set_b32(&term, tweak, &overflow);
  ret = secp256k1_scalar_set_b32_seckey(&sec, seckey);

  ret &= (!overflow) & secp256k1_eckey_privkey_tweak_add(&sec, &term);
  secp256k1_scalar secp256k1_scalar_zero = SECP256K1_SCALAR_CONST(0, 0, 0, 0, 0, 0, 0, 0);
  secp256k1_scalar_cmov(&sec, &secp256k1_scalar_zero, !ret);
  secp256k1_scalar_get_b32(seckey, &sec);

  secp256k1_scalar_clear(&sec);
  secp256k1_scalar_clear(&term);
  return ret;
}

static int secp256k1_pubkey_load(secp256k1_ge* ge, const secp256k1_pubkey* pubkey) {
  secp256k1_fe x, y;
  secp256k1_fe_set_b32(&x, pubkey->data);
  secp256k1_fe_set_b32(&y, pubkey->data + 32);
  secp256k1_ge_set_xy(ge, &x, &y);
   
  return 1;
}

static int secp256k1_eckey_pubkey_serialize(secp256k1_ge *elem, uchar *pub, size_t *size, int compressed) {
    if (secp256k1_ge_is_infinity(elem)) {
        return 0;
    }
    secp256k1_fe_normalize_var(&elem->x);
    secp256k1_fe_normalize_var(&elem->y);
    secp256k1_fe_get_b32(&pub[1], &elem->x);
    if (compressed) {
        *size = 33;
        pub[0] = secp256k1_fe_is_odd(&elem->y) ? SECP256K1_TAG_PUBKEY_ODD : SECP256K1_TAG_PUBKEY_EVEN;
    } else {
        *size = 65;
        pub[0] = SECP256K1_TAG_PUBKEY_UNCOMPRESSED;
        secp256k1_fe_get_b32(&pub[33], &elem->y);
    }
    return 1;
}

int secp256k1_ec_pubkey_serialize(uchar *output, size_t outputlen, const secp256k1_pubkey* pubkey, uint flags) {
    secp256k1_ge Q;
    int ret = 0;
    memset(output, 0, outputlen);
    if (secp256k1_pubkey_load(&Q, pubkey)) {
        ret = secp256k1_eckey_pubkey_serialize(&Q, output, &outputlen, flags & SECP256K1_FLAGS_BIT_COMPRESSION);
    }
    return ret;
}