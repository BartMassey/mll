/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#ifndef __BITSET_H__
#define __BITSET_H__

typedef struct {
    // Number of bytes in [bits]
    short count;

    // Flag: indicates [bits] represents negative value
    short neg;

    // Bit buffer
    unsigned *bits;
} *bitset;

extern bitset str2bitset(char *s);
extern char *bitset2str(bitset b, char *s);

extern bitset bs_new(int n);
extern void bs_free(bitset b);
extern void bs_land(bitset b1, bitset b2);
extern void bs_lor(bitset b1, bitset b2);
extern void bs_lxor(bitset b1, bitset b2);
extern void bs_lnot(bitset b);
extern bitset bs_and(bitset b1, bitset b2);
extern bitset bs_or(bitset b1, bitset b2);
extern bitset bs_xor(bitset b1, bitset b2);
extern bitset bs_not(bitset b0);
extern int bs_popcount(bitset b);
extern bitset bs_mask(int n);

extern bitset _bs_alloc(int n);
extern void _bs_resize(bitset b, int n);
extern bitset _bs_copy(bitset b);

#ifdef NO_INLINE
extern void bs_lset(bitset b, int i);
extern void bs_lclear(bitset b, int i);
extern int bs_isset(bitset b, int i);
#else
#define INLINE inline static
#include "bs_bitops.c"
#endif


#endif
