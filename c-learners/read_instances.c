/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include "bitset.h"
#include "read_instances.h"

int nconditions, ninstances;

struct instance *instances;

void read_instances(void) {
    int i, j, ch;

    assert(scanf("%d %d\n", &ninstances, &nconditions) == 2);
    instances = malloc(ninstances * sizeof(*instances));
    assert(instances);

    for (i = 0; i < ninstances; i++) {
	// Gather the instance
	instances[i].conditions = bs_new(nconditions + 1);
	assert(scanf("%d %d",
		     &instances[i].name,
		     &instances[i].sign) == 2);
	while ((ch = getchar()) == ' ')
	    /* do nothing */;
	ungetc(ch, stdin);
	for (j = 0; j < nconditions; j++) {
	    switch (ch = getchar()) {
	    case '0': break;
	    case '1': bs_lset(instances[i].conditions, j); break;
	    default: assert(0);
	    }
	}
	assert(getchar() == '\n');
    }
}

void shuffle_instances(void) {
    int i;
    
    for (i = 0; i < ninstances - 1; i++) {
	int j = random() % (ninstances - i);
	int k = ninstances - i - 1;
	struct instance tmp = instances[j];
	instances[j] = instances[k];
	instances[k] = tmp;
    }
}

