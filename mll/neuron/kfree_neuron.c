#include <stdlib.h>

#include "../mll.h"
#include "neuron.h"

void kfree_neuron(struct knowledge *k) {
    free(k->weights);
    free(k);
}
