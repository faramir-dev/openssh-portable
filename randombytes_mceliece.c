/* Bridge between libmceliece and OpenSSH random functions */

#include "includes.h"

#ifdef USE_MCELIECE348864X25519

void randombytes_internal_void_voidstar_longlong(void *buf, long long len)
{
	arc4random_buf(buf, (size_t)len);
}

#endif
