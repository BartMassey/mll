#include <math.h>

#include "../mll.h"
#include "../instances.h"
#include "neuron.h"


double eval_neuron(struct knowledge *k,
		   struct instance *ip,
		   struct params *p,
		   int use_linear) {
    int i, sign;
    double total = 0;

    if (p->average) {
        for (i = 0; i < k->nconditions; i++)
            total += k->weights[i] * (bs_isset(ip->conditions, i) - k->averages[i]);
    } else {
        for (i = 0; i < k->nconditions; i++)
            total += k->weights[i] * bs_isset(ip->conditions, i);
    }

    total += k->weights[k->nconditions];

    if (use_linear) {
        if (p->use_sigmoid)
            sign = sigmoid(total, p->sigmoid_k);
        else
            sign = total;
    } else
        sign = sgn(total);

    // Print confidence value for this classifiction
    if (p->debug)
        DEBUG_CLASS(ip->name, ip->sign, sign, fabs(sigmoid(total, p->sigmoid_k)));

    return sign;
}
