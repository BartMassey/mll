/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include "bitset.h"

struct instance {
    char *name;
    int sign;
    int nconditions;
    bitset conditions;
};

struct instances {
    int nconditions, ninstances;
    struct instance **instances;
};

extern struct instances *read_instances(FILE *);
extern void shuffle_instances(struct instances *);
