/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <stdlib.h>
#include <string.h>

#include "mll.h"
#include "instances.h"


struct instances *copy_instances(struct instances *iip) {
    int i;
    struct instances *niip = malloc(sizeof(*niip));

    assert(niip);

    // create instances array
    niip->instances = malloc(sizeof(*niip->instances) * iip->ninstances);
    assert(niip->instances);

    for (i=0; i < iip->ninstances; i++) {
	// allocate instance memory
	niip->instances[i] = malloc(sizeof(struct instance));
	assert(niip->instances[i]);

	niip->instances[i]->sign = iip->instances[i]->sign;
	niip->instances[i]->nconditions = iip->instances[i]->nconditions;

	// copy instance name
	niip->instances[i]->name = malloc(sizeof(char) * (strlen(iip->instances[i]->name) + 1));
	strcpy(niip->instances[i]->name, iip->instances[i]->name);

	// copy condition bits
	niip->instances[i]->conditions = _bs_copy(iip->instances[i]->conditions);
    }

    return niip;
}
