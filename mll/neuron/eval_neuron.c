#include <math.h>

#include "../mll.h"
#include "../instances.h"
#include "neuron.h"


double eval_neuron(struct knowledge *k,
		   struct instance *ip,
		   struct params *p,
		   int use_linear) {
    int i;
    double total = 0;

    if (p->average) {
        for (i = 0; i < k->nconditions; i++)
            total += k->weights[i] * (bs_isset(ip->conditions, i) - k->averages[i]);
    } else {
        for (i = 0; i < k->nconditions; i++)
            total += k->weights[i] * bs_isset(ip->conditions, i);
    }

    total += k->weights[k->nconditions];

    // Print confidence value for this classifiction
    if (p->conf)
        printf("conf(%s): %f\n", ip->name, fabs(sigmoid(total, p->sigmoid_k)));

    if (use_linear) {
        if (p->use_sigmoid)
            return sigmoid(total, p->sigmoid_k);
        return total;
    }
    return sgn(total);
}
