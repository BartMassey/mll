/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <stdlib.h>

#include "mll.h"
#include "instances.h"


void free_instances(struct instances *iip) {
    int i;

    for (i=0; i < iip->ninstances; i++) {
	bs_free(iip->instances[i]->conditions);
	free(iip->instances[i]->name);
    }

    free(iip->instances);
}
