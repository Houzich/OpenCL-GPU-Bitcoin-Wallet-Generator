#include "13_just_address.cl"

__kernel void just_seed(ulong mnemonic_start_hi,ulong mnemonic_start_lo, __global uchar * target_mnemonic, __global uchar * found_mnemonic) {
  ulong idx = get_global_id(0);

  ulong seed_start = idx*64;
  ulong mnemonic_lo = mnemonic_start_lo + idx;
  ulong mnemonic_hi = mnemonic_start_hi;

  uchar bytes[16];
  bytes[0] = mnemonic_lo & 0xFF;
  bytes[1] = (mnemonic_lo >> 8) & 0xFF;
  bytes[2] = (mnemonic_lo >> 16) & 0xFF;
  bytes[3] = (mnemonic_lo >> 24) & 0xFF;
  bytes[4] = (mnemonic_lo >> 32) & 0xFF;
  bytes[5] = (mnemonic_lo >> 40) & 0xFF;
  bytes[6] = (mnemonic_lo >> 48) & 0xFF;
  bytes[7] = (mnemonic_lo >> 56) & 0xFF;
  bytes[8] = mnemonic_hi & 0xFF;
  bytes[9] = (mnemonic_hi >> 8) & 0xFF;
  bytes[10] = (mnemonic_hi >> 16) & 0xFF;
  bytes[11] = (mnemonic_hi >> 24) & 0xFF;
  bytes[12] = (mnemonic_hi >> 32) & 0xFF;
  bytes[13] = (mnemonic_hi >> 40) & 0xFF;
  bytes[14] = (mnemonic_hi >> 48) & 0xFF;
  bytes[15] = (mnemonic_hi >> 56) & 0xFF;

  uchar mnemonic_hash[32];
  sha256(&bytes, 16, &mnemonic_hash);
  uchar checksum = mnemonic_hash[0] >> 4;
  
  ushort indices[12];
  indices[0] = (mnemonic_hi & (2047 << 53)) >> 53;
  indices[1] = (mnemonic_hi & (2047 << 42)) >> 42;
  indices[2] = (mnemonic_hi & (2047 << 31)) >> 31;
  indices[3] = (mnemonic_hi & (2047 << 20)) >> 20;
  indices[4] = (mnemonic_hi & (2047 << 9)) >> 9;
  indices[5] = ((mnemonic_hi << 55) >> 53) | ((mnemonic_lo & (3 << 62)) >> 62);
  indices[6] = (mnemonic_lo & (2047 << 51)) >> 51;
  indices[7] = (mnemonic_lo & (2047 << 40)) >> 40;
  indices[8] = (mnemonic_lo & (2047 << 29)) >> 29;
  indices[9] = (mnemonic_lo & (2047 << 18)) >> 18;
  indices[10] = (mnemonic_lo & (2047 << 7)) >> 7;
  indices[11] = ((mnemonic_lo << 57) >> 53) | checksum;

  uchar mnemonic_length = 11 + word_lengths[indices[0]] + word_lengths[indices[1]] + word_lengths[indices[2]] + word_lengths[indices[3]] + word_lengths[indices[4]] + word_lengths[indices[5]] + word_lengths[indices[6]] + word_lengths[indices[7]] + word_lengths[indices[8]] + word_lengths[indices[9]] + word_lengths[indices[10]] + word_lengths[indices[11]];

  uchar mnemonic[180];
  int mnemonic_index = 0;
  
  for (int i=0; i < 12; i++) {
    int word_index = indices[i];
    int word_length = word_lengths[word_index];
    
    for(int j=0;j<word_length;j++) {
      mnemonic[mnemonic_index] = words[word_index][j];
      mnemonic_index++;
    }
    mnemonic[mnemonic_index] = 32;
    mnemonic_index++;
  }
  mnemonic[mnemonic_index - 1] = 0;

  uchar ipad_key[128];
  uchar opad_key[128];
  for(int x=0;x<128;x++){
    ipad_key[x] = 0x36;
    opad_key[x] = 0x5c;
  }

  for(int x=0;x<mnemonic_length;x++){
    ipad_key[x] = ipad_key[x] ^ mnemonic[x];
    opad_key[x] = opad_key[x] ^ mnemonic[x];
  }

  uchar seed[64] = { 0 };
  uchar sha512_result[64] = { 0 };
  uchar key_previous_concat[256] = { 0 };
  uchar salt[12] = { 109, 110, 101, 109, 111, 110, 105, 99, 0, 0, 0, 1 };
  for(int x=0;x<128;x++){
    key_previous_concat[x] = ipad_key[x];
  }
  for(int x=0;x<12;x++){
    key_previous_concat[x+128] = salt[x];
  }

  sha512(&key_previous_concat, 140, &sha512_result);
  copy_pad_previous(&opad_key, &sha512_result, &key_previous_concat);
  sha512(&key_previous_concat, 192, &sha512_result);
  xor_seed_with_round(&seed, &sha512_result);

  for(int x=1;x<2048;x++){
    copy_pad_previous(&ipad_key, &sha512_result, &key_previous_concat);
    sha512(&key_previous_concat, 192, &sha512_result);
    copy_pad_previous(&opad_key, &sha512_result, &key_previous_concat);
    sha512(&key_previous_concat, 192, &sha512_result);
    xor_seed_with_round(&seed, &sha512_result);
  }

}
