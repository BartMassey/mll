#include <stdlib.h>

#include "../mll.h"
#include "neuron.h"

/* XXX should be changed to use XML */

struct knowledge *read_neuron(FILE *f, struct params *p) {
    int i, averages;
    char c;
    struct knowledge *k = malloc(sizeof (*k));
    assert(k);

    if (fscanf(f, "%d %d\n", &k->nconditions, &averages) != 2) {
	free(k);
	return 0;
    }

    k->weights = malloc((k->nconditions + 1) * sizeof(*k->weights));
    assert(k->weights);

    if (fscanf(f, "k %lg\n", k->weights + k->nconditions) != 1) {
        free(k->weights);
	free(k);
	return 0;
    }

    k->delta = NULL;
    k->best_weights = NULL;
    k->averages = NULL;
    p->average = 0;

    // Read weights
    if (fscanf(f, "%c ", &c) != 1 || c != 'w') {
        free(k->weights);
        free(k);
        return 0;
    }

    for (i=0; i < k->nconditions; i++) {
	if (fscanf(f, " %lg", k->weights + i) != 1) {
            free(k->weights);
	    free(k);
	    return 0;
	}
    }

    // Remove newline
    fscanf(f, "\n");

    // TODO: record [averages] in knowledge file [f] and read back here
    //       also, add nconditions check to each classify_*.c file
    if (averages) {
        if (fscanf(f, "%c ", &c) != 1 || c != 'a') {
            free(k->weights);
            free(k);
            return 0;
        }

        k->averages = malloc((k->nconditions + 1) * sizeof(*k->averages));
        assert(k->averages);

        for (i = 0; i < k->nconditions; i++) {
            if (fscanf(f, " %lg", k->averages + i) != 1) {
                free(k->weights);
                free(k);
                return 0;
            }
        }

        p->average = 1;
    }

    return k;
}
