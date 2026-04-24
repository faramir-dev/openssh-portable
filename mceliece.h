/* Classic McEliece parameter definitions */
#ifndef MCELIECE_H
#define MCELIECE_H

/* mceliece348864 parameters (base variant) */
#define mceliece_kem_348864_PUBLICKEYBYTES 261120
#define mceliece_kem_348864_SECRETKEYBYTES 6492
#define mceliece_kem_348864_CIPHERTEXTBYTES 96
#define mceliece_kem_348864_BYTES 32

/* mceliece460896 parameters (base variant) */
#define mceliece_kem_460896_PUBLICKEYBYTES 524160
#define mceliece_kem_460896_SECRETKEYBYTES 13608
#define mceliece_kem_460896_CIPHERTEXTBYTES 156
#define mceliece_kem_460896_BYTES 32

/* mceliece6688128 parameters (base variant) */
#define mceliece_kem_6688128_PUBLICKEYBYTES 1044992
#define mceliece_kem_6688128_SECRETKEYBYTES 13932
#define mceliece_kem_6688128_CIPHERTEXTBYTES 208
#define mceliece_kem_6688128_BYTES 32

/* mceliece6960119 parameters (base variant) */
#define mceliece_kem_6960119_PUBLICKEYBYTES 1047319
#define mceliece_kem_6960119_SECRETKEYBYTES 13948
#define mceliece_kem_6960119_CIPHERTEXTBYTES 194
#define mceliece_kem_6960119_BYTES 32

/* mceliece8192128 parameters (base variant) */
#define mceliece_kem_8192128_PUBLICKEYBYTES 1357824
#define mceliece_kem_8192128_SECRETKEYBYTES 14120
#define mceliece_kem_8192128_CIPHERTEXTBYTES 208
#define mceliece_kem_8192128_BYTES 32

/* Function declarations - must be provided by libmceliece */
extern int mceliece_kem_348864_keypair(unsigned char *pk, unsigned char *sk);
extern int mceliece_kem_348864_enc(unsigned char *c, unsigned char *key, const unsigned char *pk);
extern int mceliece_kem_348864_dec(unsigned char *key, const unsigned char *c, const unsigned char *sk);

extern int mceliece_kem_460896_keypair(unsigned char *pk, unsigned char *sk);
extern int mceliece_kem_460896_enc(unsigned char *c, unsigned char *key, const unsigned char *pk);
extern int mceliece_kem_460896_dec(unsigned char *key, const unsigned char *c, const unsigned char *sk);

extern int mceliece_kem_6688128_keypair(unsigned char *pk, unsigned char *sk);
extern int mceliece_kem_6688128_enc(unsigned char *c, unsigned char *key, const unsigned char *pk);
extern int mceliece_kem_6688128_dec(unsigned char *key, const unsigned char *c, const unsigned char *sk);

extern int mceliece_kem_6960119_keypair(unsigned char *pk, unsigned char *sk);
extern int mceliece_kem_6960119_enc(unsigned char *c, unsigned char *key, const unsigned char *pk);
extern int mceliece_kem_6960119_dec(unsigned char *key, const unsigned char *c, const unsigned char *sk);

extern int mceliece_kem_8192128_keypair(unsigned char *pk, unsigned char *sk);
extern int mceliece_kem_8192128_enc(unsigned char *c, unsigned char *key, const unsigned char *pk);
extern int mceliece_kem_8192128_dec(unsigned char *key, const unsigned char *c, const unsigned char *sk);

#endif /* MCELIECE_H */
