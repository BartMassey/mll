#include <stdlib.h>

#include "../mll.h"
#include "neuron.h"

/* XXX should be changed to use XML */

struct knowledge *read_neuron(FILE *f) {
    int i;
    struct knowledge *k = malloc(sizeof (*k));
    
    assert(k);

    if (fscanf(f, "%d\n", &k->nconditions) != 1)) {
	free(k);
	return 0;
    }

    k->weights = malloc((k->nconditions + 1) * sizeof(*k->weights));
    assert(k->weights);

    if (fscanf(f, "k %lg\n", k->weights + k->nconditions) != 1) {
	free(k);
	return 0;
    }

    for (i=0; i < nconditions; i++) {
	if (fscanf(f, "%d %lg\n", &t, weights + i) != 2) {
	    free(k);
	    return 0;
	}
    }

    return k;
}
