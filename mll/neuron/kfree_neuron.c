#include <stdlib.h>

#include "../mll.h"
#include "neuron.h"

void kfree_neuron(struct knowledge *k) {
    free(k->weights);
    free(k);

    if (k->delta)
	free(k->delta);
    if (k->best_weights)
	free(k->best_weights);
    if (k->averages)
	free(k->averages);
}
