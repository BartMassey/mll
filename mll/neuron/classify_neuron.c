#include <math.h>

#include "../mll.h"
#include "../instances.h"
#include "neuron.h"

// XXX this needs to be param to old evaluate() code
int use_linear = 0;

double sgn(double x) {
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}



double sigmoid(double x, double k) {
    double ex = exp(-x * k);
    return (1 - ex) / (1 + ex);
}

int classify_neuron(struct knowledge *k,
		    struct instance *ip,
		    struct params *p) {
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
    if (use_linear) {
        if (p->use_sigmoid)
            return sigmoid(total, p->sigmoid_k);
        return total;
    }
    return sgn(total);
}
