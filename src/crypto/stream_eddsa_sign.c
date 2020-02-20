#include "stream_eddsa_sign.h"

static uint8_t const C_cx_Ed25519_a[] = {0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
                                         0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xec};
static uint8_t const C_cx_Ed25519_d[] = {
    // d:  0x52036cee2b6ffe738cc740797779e89800700a4d4141d8ab75eb4dca135978a3
    0x52, 0x03, 0x6c, 0xee, 0x2b, 0x6f, 0xfe, 0x73, 0x8c, 0xc7, 0x40, 0x79, 0x77, 0x79, 0xe8, 0x98,
    0x00, 0x70, 0x0a, 0x4d, 0x41, 0x41, 0xd8, 0xab, 0x75, 0xeb, 0x4d, 0xca, 0x13, 0x59, 0x78, 0xa3};
static uint8_t const C_cx_Ed25519_q[] = {
    // q:  0x7fffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffed
    0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xed};
static uint8_t const C_cx_Ed25519_Hq[] = {
    // Hq: 0x00000000000000000000000000000000000000000000000000000000000005a4
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x05, 0xa4};
static uint8_t const C_cx_Ed25519_Bx[] = {0x21, 0x69, 0x36, 0xd3, 0xcd, 0x6e, 0x53, 0xfe, 0xc0, 0xa4, 0xe2,
                                          0x31, 0xfd, 0xd6, 0xdc, 0x5c, 0x69, 0x2c, 0xc7, 0x60, 0x95, 0x25,
                                          0xa7, 0xb2, 0xc9, 0x56, 0x2d, 0x60, 0x8f, 0x25, 0xd5, 0x1a};
static uint8_t const C_cx_Ed25519_By[] = {0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                                          0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66,
                                          0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x66, 0x58};
static uint8_t const C_cx_Ed25519_l[]  = {
    // l:  0x1000000000000000000000000000000014DEF9DEA2F79CD65812631A5CF5D3ED
    0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x14, 0xDE, 0xF9, 0xDE, 0xA2, 0xF7, 0x9C, 0xD6, 0x58, 0x12, 0x63, 0x1A, 0x5C, 0xF5, 0xD3, 0xED};
static uint8_t const C_cx_Ed25519_Hl[] = {
    // Hl: 0x0399411b7c309a3dceec73d217f5be65d00e1ba768859347a40611e3449c0f01
    0x03, 0x99, 0x41, 0x1b, 0x7c, 0x30, 0x9a, 0x3d, 0xce, 0xec, 0x73, 0xd2, 0x17, 0xf5, 0xbe, 0x65,
    0xd0, 0x0e, 0x1b, 0xa7, 0x68, 0x85, 0x93, 0x47, 0xa4, 0x06, 0x11, 0xe3, 0x44, 0x9c, 0x0f, 0x01};
static uint8_t const C_cx_Ed25519_I[] = {
    // I: 0x2b8324804fc1df0b2b4d00993dfbd7a72f431806ad2fe478c4ee1b274a0ea0b0
    0x2b, 0x83, 0x24, 0x80, 0x4f, 0xc1, 0xdf, 0x0b, 0x2b, 0x4d, 0x00, 0x99, 0x3d, 0xfb, 0xd7, 0xa7,
    0x2f, 0x43, 0x18, 0x06, 0xad, 0x2f, 0xe4, 0x78, 0xc4, 0xee, 0x1b, 0x27, 0x4a, 0x0e, 0xa0, 0xb0};
static uint8_t const C_cx_Ed25519_Qplus3div8[] = {
    // q3: 0x0ffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffffe
    0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff,
    0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe};
#define C_cx_Ed25519_h 8
cx_curve_twisted_edward_t const C_cx_Ed25519 = {
    CX_CURVE_Ed25519,
    256,
    32,
    (uint8_t *)C_cx_Ed25519_q,
    (uint8_t *)C_cx_Ed25519_Hq,
    (uint8_t *)C_cx_Ed25519_Bx,
    (uint8_t *)C_cx_Ed25519_By,
    (uint8_t *)C_cx_Ed25519_l,
    (uint8_t *)C_cx_Ed25519_Hl,
    C_cx_Ed25519_h,
    (uint8_t *)C_cx_Ed25519_a,
    (uint8_t *)C_cx_Ed25519_d,
    (uint8_t *)C_cx_Ed25519_I,
    (uint8_t *)C_cx_Ed25519_Qplus3div8,
};

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
static void cx_encode_int( unsigned char *v, int len) {
    unsigned char t;
    int i,j;
    i = 0;
    j = len-1;
    len = len/2;
    while(len--) {
        t = v[i];
        v[i] = v[j];
        v[j] = t;
        i++;
        j--;
    }
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
// P is 2*size length, encoded as: x|y
// leave x untouched
static void cx_compress(unsigned char *P, int size) {
  if(P[size-1] & 1) {
    P[size] |= 0x80;
  }
  cx_encode_int(P+size,size);
}

#define cx_decode_int(v,l) cx_encode_int(v,l)

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
static void cx_eddsa_get_public_key_internal(const cx_ecfp_private_key_t *pv_key,cx_md_t hashID,
                                             cx_ecfp_public_key_t  *pu_key,
                                             unsigned char* a, unsigned int a_len,
                                             unsigned char* h, unsigned int h_len,
                                             unsigned char* scal /*tmp*/) {
  cx_curve_twisted_edward_t *domain = (cx_curve_twisted_edward_t *) &C_cx_Ed25519;
  cx_sha512_t hash_ctx;
  unsigned int size;

  size = domain->length;

//  cx_scc_assert_param(CX_CURVE_RANGE(domain->curve,TWISTED_EDWARD));
//  cx_scc_assert_param((pv_key->d_len == size) || (pv_key->d_len == (2*size)));
//  cx_scc_assert_param(pv_key->curve == domain->curve);
//  cx_scc_assert_param(((a==NULL)&&(a_len==0)) || ((a_len) && (a_len >= size)));
//  cx_scc_assert_param(((h==NULL)&&(h_len==0)) || ((h_len) && (h_len >= size)));

  switch(hashID) {
  case CX_SHA512:
    cx_sha512_init(&hash_ctx);
    break;
  default:
    THROW(INVALID_PARAMETER);
  }

  if (pv_key->d_len == size) {
    /* 1. Hash the 32/57-byte private key using SHA-512/shak256-114, storing the digest in
     * a 32/114 bytes large buffer, denoted h.  Only the lower [CME: first] 32/57 bytes are
     * used for generating the public key.
     */
    cx_hash(&hash_ctx.header, CX_NONE, pv_key->d, pv_key->d_len, NULL, 0);
    cx_hash(&hash_ctx.header, CX_LAST, NULL, 0, scal, 64);
    /* 2. Prune the buffer: The lowest 3 bits of the first octet are
     * cleared, the highest bit of the last octet is cleared, and the
     * second highest bit of the last octet is set.
     */
    scal[0]  &= 0xF8;
    scal[31]  = (scal[31] & 0x7F) | 0x40;

  } else {
    os_memmove(scal, pv_key->d, pv_key->d_len);
  }

  /* 3. Interpret the buffer as the little-endian integer, forming a
   * secret scalar a.  Perform a fixed-base scalar multiplication
   * [a]B.
   */
  cx_decode_int(scal, size);
  if (a) {
    os_memmove(a,scal,size);
  }
  if (h) {
    os_memmove(h, scal + size, size);
  }
  pu_key->curve = pv_key->curve;
  pu_key->W_len = 1+size*2;
  pu_key->W[0]  = 0x04;
  os_memmove(pu_key->W+1,      domain->Gx, size);
  os_memmove(pu_key->W+1+size, domain->Gy, size);
  cx_ecfp_scalar_mult(domain->curve, pu_key->W, pu_key->W_len, scal, size);
}

/* ----------------------------------------------------------------------- */
/*                                                                         */
/* ----------------------------------------------------------------------- */
//int stream_eddsa_sign(const cx_ecfp_private_key_t *pv_key,
//                  int mode,  cx_md_t hashID,
//                  const unsigned char  *hash ,unsigned int hash_len,
//                  const unsigned char  *ctx , unsigned int ctx_len,
//                  unsigned char *sig, unsigned int sig_len,
//                  unsigned int *info) {
//
//  cx_curve_twisted_edward_t *domain;
//  unsigned int size,hsize;
//  union {
//    unsigned char Q[32*2+1];
//    cx_ecfp_256_public_key_t internal_pu_key;
//  }u;
//  unsigned char Y[32]; //pu_key'
//  unsigned char a[32]; // a
//  unsigned char r[32]; //r
//  unsigned char scal[64];
//  cx_sha512_t hash_ctx;
//  UNUSED(mode);UNUSED(&ctx);UNUSED(&ctx_len);
//
//  domain = (cx_curve_twisted_edward_t *) &C_cx_Ed25519;
//  size = domain->length;
////  cx_scc_assert_param(CX_CURVE_RANGE(domain->curve,TWISTED_EDWARD));
////  cx_scc_assert_param((pv_key->d_len == size) || (pv_key->d_len == 2*size));
////  cx_scc_assert_param(((&ctx==NULL)&&(&ctx_len==0)) || (&ctx));
////  cx_scc_assert_param(sig_len >= 2*size);
//
//  //get domain
//  if (info) {
//    *info = 0;
//  }
//
//  //check hashID as H function
//  switch(hashID) {
//  case CX_SHA512:
//    break;
//
//  default:
//    THROW(INVALID_PARAMETER);
//  }
//  hsize = 2*size;
//
// // retrieve public key,private scalar a, and private prefix h (stored in r)
// cx_eddsa_get_public_key_internal(pv_key, hashID, (cx_ecfp_public_key_t *)&u.internal_pu_key, a, sizeof(a), r, sizeof(r), scal);
//
// //compress public_key
// cx_edward_compress_point(domain->curve, &u.internal_pu_key.W[0], u.internal_pu_key.W_len);
// memmove(Y, &u.internal_pu_key.W[1], size);
//
//  //compute r
//  // - last size (32/57) bytes of H(sk), h,  as big endian bytes ordered. stored in r
//  // - r = H(h,m) as little endian
//  cx_sha512_init(&hash_ctx);
//
//  cx_hash(&hash_ctx.header, CX_NONE, r, size, NULL, 0);
//  cx_hash(&hash_ctx.header, CX_NONE, hash, hash_len, NULL, 0);
//  cx_hash(&hash_ctx.header, CX_LAST, NULL, 0, scal, 64);
//  cx_encode_int(scal,hsize);
//  cx_math_modm(scal, hsize,  domain->n, size);
//  memmove(r,scal+size,size);                   // r
//
//  //compute R = r.B
//  u.Q[0] = 0x04;
//  memmove(u.Q+1,      domain->Gx, size);
//  memmove(u.Q+1+size, domain->Gy, size);
//  cx_ecfp_scalar_mult(CX_CURVE_Ed25519, u.Q, 1+2*domain->length, r, size);
//  cx_compress(u.Q+1,size);
//  memmove(sig,u.Q+1+size,size);                // sig <- R
//
//  //compute S = r+H(R,A,M).a
//  // - compute H(R,A,M)
//  cx_sha512_init(&hash_ctx);
//
//  cx_hash(&hash_ctx.header, CX_NONE, sig, size, NULL, 0);
//  cx_hash(&hash_ctx.header, CX_NONE,Y, size, NULL, 0);
//  cx_hash(&hash_ctx.header, CX_NONE,hash, hash_len, NULL, 0);
//  cx_hash(&hash_ctx.header, CX_LAST, NULL, 0, scal, 64);
//  cx_encode_int(scal, hsize);
//  // - compute S = r+H(.)a
//  cx_math_modm(scal, hsize, domain->n, size);
//  cx_math_modm(a, size, domain->n, size);
//  cx_math_multm(sig+size, scal+size, a, domain->n, size);
//  cx_math_addm (sig+size, sig+size,  r, domain->n, size);
//  cx_encode_int(sig+size, size);
//
//  return 2*size;
//  #undef H
//}

void stream_eddsa_sign_step1(streamEddsaContext_t *eddsa_context, const cx_ecfp_private_key_t *pv_key) {
    os_memset((unsigned char *) eddsa_context, 0, sizeof(streamEddsaContext_t));
    cx_curve_twisted_edward_t *domain = (cx_curve_twisted_edward_t *) &C_cx_Ed25519;

    unsigned int size = domain->length;

    unsigned char scal[64];
    // retrieve public key,private scalar a, and private prefix h (stored in r)
    cx_eddsa_get_public_key_internal(pv_key, CX_SHA512, (cx_ecfp_public_key_t *)&eddsa_context->u.internal_pu_key, eddsa_context->a, sizeof(eddsa_context->a), eddsa_context->r, sizeof(eddsa_context->r), scal);

    //compress public_key
    cx_edward_compress_point(domain->curve, &eddsa_context->u.internal_pu_key.W[0], eddsa_context->u.internal_pu_key.W_len);
    os_memmove(eddsa_context->Y, &eddsa_context->u.internal_pu_key.W[1], size);

    //compute r
    // - last size (32/57) bytes of H(sk), h,  as big endian bytes ordered. stored in r
    // - r = H(h,m) as little endian

    cx_sha512_init(&eddsa_context->hash_ctx);

    cx_hash(&eddsa_context->hash_ctx.header, CX_NONE, eddsa_context->r, size, NULL, 0);
    // ...   cx_hash(&eddsa_context->hash_ctx.header, CX_NONE, hash, hash_len, NULL, 0);
}

int stream_eddsa_sign_step2(streamEddsaContext_t *eddsa_context, const unsigned char  *hash ,unsigned int hash_len) {
     return cx_hash(&eddsa_context->hash_ctx.header, CX_NONE, hash, hash_len, NULL, 0);
}

void stream_eddsa_sign_step3(streamEddsaContext_t *eddsa_context) {
       unsigned char scal[64];
       cx_curve_twisted_edward_t *domain = (cx_curve_twisted_edward_t *) &C_cx_Ed25519;
       unsigned int size = domain->length;
       unsigned int hsize = 2*size;

       cx_hash(&eddsa_context->hash_ctx.header, CX_LAST, NULL, 0, scal, 64);
       cx_encode_int(scal,hsize);
       cx_math_modm(scal, hsize, domain->n, size);
       os_memmove(eddsa_context->r,scal+size,size);                   // r

       //compute R = r.B
       eddsa_context->u.Q[0] = 0x04;
       os_memmove(eddsa_context->u.Q+1, domain->Gx, size);
       os_memmove(eddsa_context->u.Q+1+size, domain->Gy, size);
       cx_ecfp_scalar_mult(CX_CURVE_Ed25519, eddsa_context->u.Q, 1+2*domain->length, eddsa_context->r, size);
       cx_compress(eddsa_context->u.Q+1,size);
       os_memmove(eddsa_context->sig,eddsa_context->u.Q+1+size,size);                // sig <- R

       //compute S = r+H(R,A,M).a
       // - compute H(R,A,M)
       cx_sha512_init(&eddsa_context->hash_ctx);

       cx_hash(&eddsa_context->hash_ctx.header, CX_NONE, eddsa_context->sig, size, NULL, 0);
       cx_hash(&eddsa_context->hash_ctx.header, CX_NONE, eddsa_context->Y, size, NULL, 0);
}

int stream_eddsa_sign_step4(streamEddsaContext_t *eddsa_context, const unsigned char  *hash ,unsigned int hash_len) {
     return stream_eddsa_sign_step2(eddsa_context, hash, hash_len);
}

int stream_eddsa_sign_step5(streamEddsaContext_t *eddsa_context, unsigned char *sig) {
      unsigned char scal[64];
      cx_curve_twisted_edward_t *domain = (cx_curve_twisted_edward_t *) &C_cx_Ed25519;
      unsigned int size = domain->length;
      unsigned int hsize = 2*size;

      cx_hash(&eddsa_context->hash_ctx.header, CX_LAST, NULL, 0, scal, 64);

      cx_encode_int(scal, hsize);
      // - compute S = r+H(.)a
      cx_math_modm(scal, hsize, domain->n, size);
      cx_math_modm(eddsa_context->a, size, domain->n, size);
      cx_math_multm(eddsa_context->sig+size, scal+size, eddsa_context->a, domain->n, size);
      cx_math_addm (eddsa_context->sig+size, eddsa_context->sig+size,  eddsa_context->r, domain->n, size);
      cx_encode_int(eddsa_context->sig+size, size);

      os_memmove(sig, eddsa_context->sig, 64);
      os_memset((unsigned char *) eddsa_context, 0, sizeof(streamEddsaContext_t));

      return 2*size;
}