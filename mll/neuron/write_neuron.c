#include "../mll.h"
#include "neuron.h"

/* XXX should be changed to use XML */
/* XXX should be able to fail */

int write_neuron(FILE *f, struct knowledge *k) {
    int i, has_averages = 0;

    if (k->averages)
        has_averages = 1;

    fprintf(f, "%d %d\n", k->nconditions, has_averages);
    fprintf(f, "k %lg\n", k->weights[k->nconditions]);

    fprintf(f, "w");
    for (i=0; i < k->nconditions; i++)
	fprintf(f, " %lg", k->weights[i]);

    fprintf(f, "\n");

    if (k->averages) {
        fprintf(f, "a");
        for (i=0; i < k->nconditions; i++)
            fprintf(f, " %lg", k->averages[i]);
        fprintf(f, "\n");
    }

    return k->nconditions;
}
