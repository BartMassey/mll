/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

extern int nconditions, ninstances;

struct instance {
    int name;
    int sign;
    bitset conditions;
};

extern struct instance *instances;

extern void read_instances(void);
extern void shuffle_instances(void);
