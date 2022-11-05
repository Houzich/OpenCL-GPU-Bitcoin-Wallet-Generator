#include "12_int_to_address.cl"

__kernel void just_address(ulong mnemonic_start_hi,ulong mnemonic_start_lo, __global uchar * target_mnemonic, __global uchar * found_mnemonic) {
  ulong idx = get_global_id(0);

  ulong seed_start = idx*64;

  ulong seed[8] = { 0 };
  seed[0] = idx;
  seed[1] = idx + 1;
  seed[2] = idx + 2;
  seed[3] = idx + 3;
  seed[4] = idx + 4;
  seed[5] = idx + 5;
  seed[6] = idx + 6;
  seed[7] = idx + 7;


  uchar network = BITCOIN_MAINNET;
  extended_private_key_t master_private;
  extended_public_key_t master_public;
  
  new_master_from_seed(network, &seed, &master_private);
  public_from_private(&master_private, &master_public);

  uchar serialized_master_public[33];
  serialized_public_key(&master_public, &serialized_master_public);
  extended_private_key_t target_key;
  extended_public_key_t target_public_key;
  hardened_private_child_from_private(&master_private, &target_key, 49z);
  hardened_private_child_from_private(&target_key, &target_key, 0);
  hardened_private_child_from_private(&target_key, &target_key, 0);
  normal_private_child_from_private(&target_key, &target_key, 0);
  normal_private_child_from_private(&target_key, &target_key, 0);
  public_from_private(&target_key, &target_public_key);

  uchar raw_address[25] = {0};
  p2shwpkh_address_for_public_key(&target_public_key, &raw_address);
}
