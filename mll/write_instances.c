/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <stdlib.h>

#include "mll.h"
#include "instances.h"


void write_instances(FILE *f, struct instances *iip) {
    int i, j;

    // echo ninstances/nconditions
    fprintf(f, "%d %d\n", iip->ninstances, iip->nconditions);

    char *buff = malloc(iip->nconditions + 1);
    assert(buff);
    buff[iip->nconditions] = '\0';

    for (i=0; i < iip->ninstances; i++) {
	// avoid lots of calls to fprintf()
	for (j=0; j < iip->nconditions; j++) {
	    if (bs_isset(iip->instances[i]->conditions, j))
		buff[j] = '1';
	    else
		buff[j] = '0';
	}

	// name sign conditions
	fprintf(f, "%s %d %s\n", iip->instances[i]->name, iip->ninstances, buff);
    }

    free(buff);
}
