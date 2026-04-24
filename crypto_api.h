/* $OpenBSD: crypto_api.h,v 1.10 2025/10/30 23:19:33 djm Exp $ */

/*
 * Assembled from generated headers and source files by Markus Friedl.
 * Placed in the public domain.
 */

#ifndef crypto_api_h
#define crypto_api_h

#include "includes.h"

#include <stdint.h>
#include <stdlib.h>

typedef int8_t crypto_int8;
typedef uint8_t crypto_uint8;
typedef int16_t crypto_int16;
typedef uint16_t crypto_uint16;
typedef int32_t crypto_int32;
typedef uint32_t crypto_uint32;
typedef int64_t crypto_int64;
typedef uint64_t crypto_uint64;

#define randombytes(buf, buf_len) arc4random_buf((buf), (buf_len))
#define small_random32() arc4random()

#define crypto_hash_sha512_BYTES 64U

#ifdef WITH_OPENSSL
#include <openssl/evp.h>
static inline int
crypto_hash_sha512(unsigned char *out, const unsigned char *in,
    unsigned long long inlen)
{

	if (!EVP_Digest(in, inlen, out, NULL, EVP_sha512(), NULL))
		return -1;
	return 0;
}
#else /* WITH_OPENSSL */
# ifdef HAVE_SHA2_H
#  include <sha2.h>
# endif
static inline int
crypto_hash_sha512(unsigned char *out, const unsigned char *in,
    unsigned long long inlen)
{

	SHA2_CTX ctx;

	SHA512Init(&ctx);
	SHA512Update(&ctx, in, inlen);
	SHA512Final(out, &ctx);
	return 0;
}
#endif /* WITH_OPENSSL */

#define crypto_sign_ed25519_SECRETKEYBYTES 64U
#define crypto_sign_ed25519_PUBLICKEYBYTES 32U
#define crypto_sign_ed25519_BYTES 64U

int	crypto_sign_ed25519(unsigned char *, unsigned long long *,
    const unsigned char *, unsigned long long, const unsigned char *);
int	crypto_sign_ed25519_open(unsigned char *, unsigned long long *,
    const unsigned char *, unsigned long long, const unsigned char *);
int	crypto_sign_ed25519_keypair(unsigned char *, unsigned char *);

#define crypto_kem_sntrup761_PUBLICKEYBYTES 1158
#define crypto_kem_sntrup761_SECRETKEYBYTES 1763
#define crypto_kem_sntrup761_CIPHERTEXTBYTES 1039
#define crypto_kem_sntrup761_BYTES 32

int	crypto_kem_sntrup761_enc(unsigned char *cstr, unsigned char *k,
    const unsigned char *pk);
int	crypto_kem_sntrup761_dec(unsigned char *k,
    const unsigned char *cstr, const unsigned char *sk);
int	crypto_kem_sntrup761_keypair(unsigned char *pk, unsigned char *sk);

#define crypto_kem_mlkem768_PUBLICKEYBYTES 1184
#define crypto_kem_mlkem768_SECRETKEYBYTES 2400
#define crypto_kem_mlkem768_CIPHERTEXTBYTES 1088
#define crypto_kem_mlkem768_BYTES 32

#ifdef USE_MCELIECE348864X25519
#include "mceliece.h"
#define crypto_kem_mceliece348864_PUBLICKEYBYTES mceliece_kem_348864_PUBLICKEYBYTES
#define crypto_kem_mceliece348864_SECRETKEYBYTES mceliece_kem_348864_SECRETKEYBYTES
#define crypto_kem_mceliece348864_CIPHERTEXTBYTES mceliece_kem_348864_CIPHERTEXTBYTES
#define crypto_kem_mceliece348864_BYTES mceliece_kem_348864_BYTES
#define crypto_kem_mceliece348864_enc mceliece_kem_348864_enc
#define crypto_kem_mceliece348864_dec mceliece_kem_348864_dec
#define crypto_kem_mceliece348864_keypair mceliece_kem_348864_keypair
#endif

#ifdef USE_MCELIECE460896X25519
#include "mceliece.h"
#define crypto_kem_mceliece460896_PUBLICKEYBYTES mceliece_kem_460896_PUBLICKEYBYTES
#define crypto_kem_mceliece460896_SECRETKEYBYTES mceliece_kem_460896_SECRETKEYBYTES
#define crypto_kem_mceliece460896_CIPHERTEXTBYTES mceliece_kem_460896_CIPHERTEXTBYTES
#define crypto_kem_mceliece460896_BYTES mceliece_kem_460896_BYTES
#define crypto_kem_mceliece460896_enc mceliece_kem_460896_enc
#define crypto_kem_mceliece460896_dec mceliece_kem_460896_dec
#define crypto_kem_mceliece460896_keypair mceliece_kem_460896_keypair
#endif

#ifdef USE_MCELIECE6688128X25519
#include "mceliece.h"
#define crypto_kem_mceliece6688128_PUBLICKEYBYTES mceliece_kem_6688128_PUBLICKEYBYTES
#define crypto_kem_mceliece6688128_SECRETKEYBYTES mceliece_kem_6688128_SECRETKEYBYTES
#define crypto_kem_mceliece6688128_CIPHERTEXTBYTES mceliece_kem_6688128_CIPHERTEXTBYTES
#define crypto_kem_mceliece6688128_BYTES mceliece_kem_6688128_BYTES
#define crypto_kem_mceliece6688128_enc mceliece_kem_6688128_enc
#define crypto_kem_mceliece6688128_dec mceliece_kem_6688128_dec
#define crypto_kem_mceliece6688128_keypair mceliece_kem_6688128_keypair
#endif

#ifdef USE_MCELIECE6960119X25519
#include "mceliece.h"
#define crypto_kem_mceliece6960119_PUBLICKEYBYTES mceliece_kem_6960119_PUBLICKEYBYTES
#define crypto_kem_mceliece6960119_SECRETKEYBYTES mceliece_kem_6960119_SECRETKEYBYTES
#define crypto_kem_mceliece6960119_CIPHERTEXTBYTES mceliece_kem_6960119_CIPHERTEXTBYTES
#define crypto_kem_mceliece6960119_BYTES mceliece_kem_6960119_BYTES
#define crypto_kem_mceliece6960119_enc mceliece_kem_6960119_enc
#define crypto_kem_mceliece6960119_dec mceliece_kem_6960119_dec
#define crypto_kem_mceliece6960119_keypair mceliece_kem_6960119_keypair
#endif

#ifdef USE_MCELIECE8192128X25519
#include "mceliece.h"
#define crypto_kem_mceliece8192128_PUBLICKEYBYTES mceliece_kem_8192128_PUBLICKEYBYTES
#define crypto_kem_mceliece8192128_SECRETKEYBYTES mceliece_kem_8192128_SECRETKEYBYTES
#define crypto_kem_mceliece8192128_CIPHERTEXTBYTES mceliece_kem_8192128_CIPHERTEXTBYTES
#define crypto_kem_mceliece8192128_BYTES mceliece_kem_8192128_BYTES
#define crypto_kem_mceliece8192128_enc mceliece_kem_8192128_enc
#define crypto_kem_mceliece8192128_dec mceliece_kem_8192128_dec
#define crypto_kem_mceliece8192128_keypair mceliece_kem_8192128_keypair
#endif

#endif /* crypto_api_h */
