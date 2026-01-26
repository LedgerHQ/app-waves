#include "ledger_crypto.h"
#include "keypair.h"
#include "os.h"
#include "cx.h"
#include "os_io_seproxyhal.h"
// В некоторых версиях SDK это помогает для декларации os_perso...
#include "os_seed.h" 
#include "../globals.h"

// Конвертация публичного ключа из LE в BE
void public_key_le_to_be(cx_ecfp_public_key_t *public_key) {
  uint8_t public_key_be[32];
  for (uint8_t i = 0; i < 32; i++) {
    public_key_be[i] = public_key->W[64 - i];
  }
  if ((public_key->W[32] & 1) != 0) {
    public_key_be[31] |= 0x80;
  }
  memset(public_key->W, 0, 65);
  memmove(public_key->W, public_key_be, 32);
}

cx_err_t get_keypair_by_path(const uint32_t *path, cx_ecfp_public_key_t *public_key,
                         cx_ecfp_private_key_t *private_key) {
  uint8_t privateKeyData[64];
  
  // Используем системный вызов напрямую для деривации
  cx_err_t error = os_derive_bip32_with_seed_no_throw(HDW_ED25519_SLIP10, CX_CURVE_Ed25519,
                                      path, 5, privateKeyData, NULL,
                                      (unsigned char *)"ed25519 seed", 12);
  if (error) {
    return error; 
  }
                                
  if(cx_ecfp_init_private_key_no_throw(CX_CURVE_Ed25519, privateKeyData, 32, private_key)) {
    return INIT_PRIVATE_KEY_ERROR;
  }
 
  if(cx_ecfp_init_public_key_no_throw(CX_CURVE_Ed25519, NULL, 0, public_key)) {
    return INIT_PUBLIC_KEY_ERROR;
  }
  if(cx_ecfp_generate_pair_no_throw(CX_CURVE_Ed25519, public_key, private_key, 1)) {
    return GENERATE_PAIR_ERROR;
  }

  // Clear secret after use
  explicit_bzero(privateKeyData, sizeof(privateKeyData));
  return CX_OK;
}

cx_err_t get_curve25519_public_key_for_path(const uint32_t *path,
                                        cx_ecfp_public_key_t *public_key) {
  cx_ecfp_private_key_t private_key;
  
  cx_err_t error = get_keypair_by_path(path, public_key, &private_key);
  if (error) {
    return error;
  }
  // Clear secret after use
  explicit_bzero(&private_key, sizeof(cx_ecfp_private_key_t));

  public_key_le_to_be(public_key);

  error = ed25519_pk_to_curve25519(public_key->W, public_key->W);

  if(error) {
    return D25519_TO_C25519_ERROR;
  }
  return CX_OK;
}

void blake2b_256(const unsigned char *msg, size_t msg_len, void *out) {
  cx_err_t error = blake2b_256_no_throw(msg, msg_len, out);
  if (error) {
    THROW(error);
  }
}

cx_err_t blake2b_256_no_throw(const unsigned char *msg, size_t msg_len, void *out) {
  cx_blake2b_t ctx;
  cx_err_t error;
  
  error = cx_blake2b_init_no_throw(&ctx, 256);
  if (error) {
    return error;
  }
  
  error = cx_hash_no_throw(&ctx.header, CX_LAST, msg, msg_len, out, 32);
  if (error) {
    return error;
  }

  return CX_OK;
}

cx_err_t keccak_256_no_throw(const unsigned char *msg, size_t msg_len, void *out) {
  cx_sha3_t ctx;
  cx_err_t error;
  error = cx_keccak_init_no_throw(&ctx, 256);
  if (error) {
    return error;
  }
  error = cx_hash_no_throw(&ctx.header, CX_LAST, msg, msg_len, out, 32);
  if (error) {
    return error;
  }
  return CX_OK;
}

void keccak_256(const unsigned char *msg, size_t msg_len, void *out) {
   cx_err_t error = keccak_256_no_throw(msg, msg_len, out);
  if (error) {
    THROW(error);
  }
}