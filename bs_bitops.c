/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <assert.h>

INLINE void bs_lset(bitset b, int i) {
    int j, k;
    assert(i >= 0);
    j = i >> 5;
    k = i & 0x1f;
    if (j >= b->count)
	_bs_resize(b, j + 1);
    b->bits[j] |= (1 << k);
}

INLINE void bs_lclear(bitset b, int i) {
    int j, k;
    assert(i >= 0);
    j = i >> 5;
    k = i & 0x1f;
    if (j >= b->count)
	_bs_resize(b, j + 1);
    b->bits[j] &= ~(1 << k);
}

INLINE int bs_isset(bitset b, int i) {
    int j, k;
    assert(i >= 0);
    j = i >> 5;
    k = i & 0x1f;
    if (j >= b->count)
	return b->neg;
    return (b->bits[j] & (1 << k)) > 0;
}

INLINE int bs_equal(bitset b1, bitset b2) {
    int i, n;
    if (b1->neg != b2->neg)
	return 0;
    n = b1->count;
    if (b1->count > b2->count)
	n = b2->count;
    for (i = 0; i < n; i++)
	if (b1->bits[i] != b2->bits[i])
	    return 0;
    if (b1->count == b2->count)
	return 1;
    if (b2->count > b1->count) {
	for (; i < b2->count; i++)
	    if (b2->bits[i] != (b1->neg ? ~0 : 0))
		return 0;
	return 1;
    }
    assert(b1->count <= b2->count);  /* weird count comparison */
    for (; i < b1->count; i++)
	if (b1->bits[i] != (b2->neg ? ~0 : 0))
	    return 0;
    return 1;
}

