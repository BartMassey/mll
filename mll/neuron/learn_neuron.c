#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "neuron.h"

/*
 * Returns one of -0.5, -0.4 , ... , -0.1, 0.1, 0.2, ..., 0.5
 * selected via a PRNG.
 */
double small_rand(void) {
    long v = ((unsigned long) random()) % 10 - 5;
    assert(v >= -5 && v <= 4);
    if (v < 0)
        return v / (double) 10;
    return (v + 1) / (double) 10;
}

struct knowledge *learn_neuron(struct instances *ip,
			       struct params *p) {
    int i;
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);
    k->weights = malloc((ip->nconditions + 1) * sizeof(*k->weights));
    assert(k->weights);
    k->nconditions = ip->nconditions;

    for (i = 0; i < k->nconditions + 1; i++)
        k->weights[i] = small_rand() / 100;

    k->delta = malloc((k->nconditions + 1) * sizeof(*k->delta));
    assert(k->delta);
    k->best_weights = malloc((k->nconditions + 1) * sizeof(*k->best_weights));
    k->best_score = -1;

    if (p->average) {
        k->averages = malloc(k->nconditions * sizeof(*k->averages));
        assert(k->averages);
        for (i = 0; i < k->nconditions; i++) {
            int j;
            double t = 0;

            for (j = 0; j < ip->ninstances; j++)
                if (bs_isset(ip->instances[j]->conditions, i))
                    t += 1.0;
            k->averages[i] = t / ip->ninstances;
        }

    } else {
	// make sure averages is null so kfree() knows not to free it
	k->averages = NULL;
    }

    return k;
}
