
#include "xorshift.h"

#ifdef __KERNEL__
#include <linux/random.h>
#else
#include <stdlib.h>
#include <time.h>
#endif

static uint32_t x, y, z, w;

static uint32_t get_seed(void)
{
#ifdef __KERNEL__
	uint32_t num;
	get_random_bytes(&num, sizeof(num));
	return num;
#else
	return time(NULL);
#endif
}

void xorshift_srand(void)
{
	x = get_seed();
	y = get_seed();
	w = get_seed();
	z = get_seed();
}

static uint32_t xorshift128(void)
{
	uint32_t t = x ^ (x << 11);
	x = y;
	y = z;
	z = w;
	return w = w ^ (w >> 19) ^ t ^ (t >> 8);
}

uint32_t xorshift_rand(void)
{
	return xorshift128();
}
