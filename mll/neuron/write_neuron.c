#include "../mll.h"
#include "neuron.h"

/* XXX should be changed to use XML */
/* XXX should be able to fail */

int write_nbayes(FILE *f, struct knowledge *k) {
    int i;

    fprintf(f, "%d\n", k->nconditions);
    fprintf(f, "k %lg\n", k->weights[k->nconditions]);

    for (i=0; i < k->nconditions; i++)
	fprintf(f, "%d %lg\n", i, k->weights[i]);

    return k->nconditions;
}
