/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <assert.h>
#include <stdlib.h>
#include "bitset.h"

bitset _bs_alloc(int n) {
    bitset b = malloc(sizeof(*b));
    assert(b);
    b->count = n;
    b->neg = 0;
    b->bits = malloc(b->count * sizeof(*b->bits));
    assert(b);
    return b;
}

bitset bs_new(int n) {
    bitset b;
    assert(n >= 0);
    b = malloc(sizeof(*b));
    assert(b);
    b->count = (n >> 5) + 1;
    b->neg = 0;
    b->bits = calloc(b->count, sizeof(*b->bits));
    assert(b);
    return b;
}

void bs_free(bitset b) {
    free(b->bits);
    free(b);
}

void _bs_resize(bitset b, int n) {
    int i;
    int oldcount = b->count;
    if (n == oldcount)
	return;
    b->count = n;
    b->bits = realloc(b->bits, b->count * sizeof(*b->bits));
    assert(b);
    for (i = 0; i < n - oldcount; i++)
	if (b->neg)
	    b->bits[n + i] = ~0;
	else
	    b->bits[n + i] = 0;
}

void bs_land(bitset b1, bitset b2) {
    int i;
    if ((b2->count > b1->count && b1->neg) ||
	(b1->count > b2->count && !b2->neg))
	_bs_resize(b1, b2->count);
    for (i = 0; i < b1->count; i++)
	b1->bits[i] &= b2->bits[i];
    b1->neg &= b2->neg;
}

void bs_lor(bitset b1, bitset b2) {
    int i;
    if (b2->count > b1->count)
	_bs_resize(b1, b2->count);
    for (i = 0; i < b2->count; i++)
	b1->bits[i] |= b2->bits[i];
    b1->neg |= b2->neg;
}

void bs_lxor(bitset b1, bitset b2) {
    int i;
    if (b2->count > b1->count)
	_bs_resize(b1, b2->count);
    for (i = 0; i < b2->count; i++)
	b1->bits[i] ^= b2->bits[i];
    b1->neg ^= b2->neg;
}

void bs_lnot(bitset b) {
    int i;
    for (i = 0; i < b->count; i++)
	b->bits[i] = ~b->bits[i];
    b->neg = !b->neg;
}


bitset _bs_copy(bitset b) {
    int i;
    bitset br = _bs_alloc(b->count);
    for (i = 0; i < b->count; i++)
	br->bits[i] = b->bits[i];
    br->count = b->count;
    br->neg = b->neg;
    return br;
}


bitset bs_and(bitset b1, bitset b2) {
    bitset b = _bs_copy(b1);
    bs_land(b, b2);
    return b;
}

bitset bs_or(bitset b1, bitset b2) {
    bitset b = _bs_copy(b1);
    bs_lor(b, b2);
    return b;
}

bitset bs_xor(bitset b1, bitset b2) {
    bitset b = _bs_copy(b1);
    bs_lxor(b, b2);
    return b;
}

bitset bs_not(bitset b0) {
    bitset b = _bs_copy(b0);
    bs_lnot(b);
    return b;
}

static unsigned count_bits (unsigned n) {
    unsigned c3 = 0x0f0f0f0f;
    unsigned c2 = c3 ^ (c3 << 2);  /* c2 == 0x33333333 */
    unsigned c1 = c2 ^ (c2 << 1);  /* c1 == 0x55555555 */
    unsigned left = (n >> 1) & c1;
    n = left + (n & c1);
    left = (n >> 2) & c2;
    n = left + (n & c2);
    left = (n >> 4) & c3;
    n = left + (n & c3);
    n += (n >> 8);
    n += (n >> 16);
    return (n & 0xff);
}

int bs_popcount(bitset b) {
    int i;
    int count = 0;
    assert (!b->neg);
    for (i = 0; i < b->count; i++)
	count += count_bits(b->bits[i]);
    return count;
}


bitset bs_mask(int n) {
    int i, j, k;
    bitset b;
    assert(n >= 0);
    b = bs_new(n);
    j = n >> 5;
    k = n & 0x1f;
    for (i = 0; i < j; i++)
	b->bits[i] = ~0;
    b->bits[j] = (1 << k) - 1;
    return b;
}


#ifdef NO_INLINE
#define INLINE /**/
#include "bs_bitops.c"
#endif
