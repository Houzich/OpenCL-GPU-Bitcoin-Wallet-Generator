#ifndef _ADDRESS_CL
#define _ADDRESS_CL
#include "09_secp256k1.cl"
#include "02_ripemd.cl"

#define BITCOIN_MAINNET 0
#define BITCOIN_TESTNET 1

typedef struct {
  bool compressed;
  int network;
  uchar key[32];
} private_key_t;

typedef struct {
  bool compressed;
  secp256k1_pubkey key;
} public_key_t;

typedef struct {
  uchar network;
  uchar depth;
  uchar parent_fingerprint[4];
  uint child_number;
  uchar chain_code[32];
  private_key_t private_key;
} extended_private_key_t;

typedef struct {
  uchar network;
  uchar depth;
  uchar parent_fingerprint[4];
  uint child_number;
  uchar chain_code[32];
  public_key_t public_key;
} extended_public_key_t;

void hmac_sha512(uchar *key, int key_length_bytes, uchar *message, int message_length_bytes, uchar *output) {
  uchar ipad_key[128];
  uchar opad_key[128];
  for(int x=0;x<128;x++){
    ipad_key[x] = 0x36;
    opad_key[x] = 0x5C;
  }

  for(int x=0;x<key_length_bytes;x++){
    ipad_key[x] = ipad_key[x] ^ key[x];
    opad_key[x] = opad_key[x] ^ key[x];
  }

  uchar inner_concat[256] = { 0 };

  for(int x=0;x<128;x++){
    inner_concat[x] = ipad_key[x];
  }
  for(int x=0;x<message_length_bytes;x++){
    inner_concat[x+128] = message[x];
  }

  sha512((ulong *)&inner_concat, 128+message_length_bytes, (ulong *)output);

  for(int x=0;x<128;x++){
    inner_concat[x] = opad_key[x];
  }
  for(int x=0;x<64;x++){
    inner_concat[x+128] = output[x];
  }

  sha512((ulong *)&inner_concat, 192, (ulong *)output);
}

void new_master_from_seed(uchar network, uchar *seed, extended_private_key_t * master) {
  uchar key[12] = { 0x42, 0x69, 0x74, 0x63, 0x6f, 0x69, 0x6e, 0x20, 0x73, 0x65, 0x65, 0x64 };
  uchar hmacsha512_result[64] = { 0 };
  hmac_sha512((uchar *)&key, 12, seed, 64, (uchar *)&hmacsha512_result);
  private_key_t pkey;
  pkey.compressed = false;
  pkey.network = network;
  memcpy_offset((uchar *)&pkey.key, (const uchar *)&hmacsha512_result, 0, 32);
  master->network = network;
  master->depth = 0;
  master->parent_fingerprint[0] = 0x00;
  master->parent_fingerprint[1] = 0x00;
  master->parent_fingerprint[2] = 0x00;
  master->parent_fingerprint[3] = 0x00;
  master->child_number = 0;
  master->private_key = pkey;
  memcpy_offset((uchar *)&master->chain_code, (const uchar *)&hmacsha512_result, 32, 32);
}

void public_from_private(extended_private_key_t *priv, extended_public_key_t *pub) {
  pub->network = priv->network;
  pub->depth = priv->depth;
  pub->child_number = priv->child_number;
  memcpy((uchar *)&pub->parent_fingerprint,(const uchar *)&priv->parent_fingerprint, 4);
  memcpy((uchar *)&pub->chain_code, (const uchar *)&priv->chain_code, 32);
  secp256k1_ec_pubkey_create(&pub->public_key.key, (const uchar *)&priv->private_key.key);
}

void serialized_public_key(extended_public_key_t *pub, uchar *serialized_key) {
  secp256k1_ec_pubkey_serialize(serialized_key, 33, &pub->public_key.key, SECP256K1_EC_COMPRESSED);
}

void uncompressed_public_key(extended_public_key_t *pub, uchar *serialized_key) {
  secp256k1_ec_pubkey_serialize(serialized_key, 65, &pub->public_key.key, SECP256K1_EC_UNCOMPRESSED);
}

void sha256d(uchar *input, int input_len, char * output) {
  sha256((const uint*)input, input_len, (uint *)output);
  sha256((const uint*)output, 32, (uint *)output);
}

void hash160(uchar *input, int input_len, char * output) {
  uchar sha256_result[32] = { 0 };
  sha256((const uint*)input, input_len, (uint *)&sha256_result);
  ripemd160((const uchar *)&sha256_result, 32, (uchar *)output);
}

void identifier_for_public_key(extended_public_key_t *pub, uchar *identifier) {
  uchar serialized_key[33] = {0};
  serialized_public_key(pub, (uchar *)&serialized_key);
  hash160((uchar *)&serialized_key, 33, (char *)identifier);
}

void fingerprint_for_public_key(extended_public_key_t *pub, uchar *fingerprint) {
  uchar identifier[20] = { 0 };
  identifier_for_public_key(pub, (uchar *)&identifier);
  fingerprint[0] = identifier[0];
  fingerprint[1] = identifier[1];
  fingerprint[2] = identifier[2];
  fingerprint[3] = identifier[3];
}

void hash160_for_public_key(extended_public_key_t *pub, uchar *address_bytes) {
  uchar serialized_pub_key[33] = {0};
  serialized_public_key(pub, (uchar *)&serialized_pub_key);
  hash160((uchar *)&serialized_pub_key, 33, (char *)address_bytes);
}

void p2shwpkh_address_for_public_key(extended_public_key_t *pub, uchar *address_bytes) {
  uchar pubkey_hash[20] = { 0 };
  identifier_for_public_key(pub, (uchar *)&pubkey_hash);

  uchar wpkh_script_bytes[22] = { 0 };
  wpkh_script_bytes[0] = 0x00; // version byte
  wpkh_script_bytes[1] = 0x14; // hash160 length of 20
  for(int i=0;i<20;i++){
    wpkh_script_bytes[i+2] = pubkey_hash[i]; // hash160(pubkey)
  }

  uchar wpkh_script_hash[20] = { 0 };
  hash160((uchar *)&wpkh_script_bytes, 22, (char *)&wpkh_script_hash);

  address_bytes[0] = 5; // bitcoin mainnet; 196 for testnet

  for(int i=0;i<20;i++) {
    address_bytes[i+1] = wpkh_script_hash[i];
  }
  
  uchar sha256d_result[32] = { 0 };
  sha256d(address_bytes, 21, (char *)&sha256d_result);

  // append checksum
  address_bytes[21] = sha256d_result[0];
  address_bytes[22] = sha256d_result[1];
  address_bytes[23] = sha256d_result[2];
  address_bytes[24] = sha256d_result[3];
}

void normal_private_child_from_private(extended_private_key_t *parent, extended_private_key_t *child, uint normal_child_number) {
  uchar hmacsha512_result[64] = { 0 };
  extended_public_key_t pub;
  public_from_private(parent, &pub);
  uchar hmac_input[37] = {0};
  serialized_public_key(&pub, (uchar *)&hmac_input);
  hmac_input[33] = normal_child_number >> 24;
  hmac_input[34] = (normal_child_number & 0x00FF0000) >> 16;
  hmac_input[35] = (normal_child_number & 0x0000FF00) >> 8;
  hmac_input[36] = (normal_child_number & 0x000000FF);
  hmac_sha512((uchar *)&parent->chain_code, 32, (uchar *)&hmac_input, 37, (uchar *)&hmacsha512_result);

  private_key_t sk;
  sk.compressed = true;
  sk.network = parent->network;
  memcpy((uchar *)&sk.key, (const uchar *)&hmacsha512_result, 32);
  secp256k1_ec_seckey_tweak_add((uchar *)&sk.key, (const uchar *)&parent->private_key.key);
  child->network = parent->network;
  child->depth = parent->depth + 1;
  child->child_number = normal_child_number;
  child->private_key = sk;
  memcpy_offset((uchar *)&child->chain_code, (const uchar *)&hmacsha512_result, 32, 32);
}

void hardened_private_child_from_private(extended_private_key_t *parent, extended_private_key_t *child, uint hardened_child_number) {

  uint child_number = (1 << 31) + hardened_child_number;
  uchar hmacsha512_result[64] = { 0 };
  uchar hmac_input[37] = {0};
  for(int x=0;x<32;x++){
    hmac_input[x+1] = parent->private_key.key[x];
  }
  hmac_input[33] = child_number >> 24;
  hmac_input[34] = (child_number & 0x00FF0000) >> 16;
  hmac_input[35] = (child_number & 0x0000FF00) >> 8;
  hmac_input[36] = (child_number & 0x000000FF);
  
  hmac_sha512((uchar *)&parent->chain_code, 32, (uchar *)&hmac_input, 37, (uchar *)&hmacsha512_result);
  
  private_key_t sk;
  sk.compressed = true;
  sk.network = parent->network;
  memcpy((uchar *)&sk.key, (const uchar *)&hmacsha512_result, 32);
  secp256k1_ec_seckey_tweak_add((uchar *)&sk.key, (const uchar *)&parent->private_key.key);
  child->network = parent->network;
  child->depth = parent->depth + 1;
  child->child_number = child_number;
  child->private_key = sk;
  memcpy_offset((uchar *)&child->chain_code, (const uchar *)&hmacsha512_result, 32, 32);
}

#endif //_ADDRESS_CL