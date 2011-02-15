#include <math.h>
#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "neuron.h"

int classify_neuron(struct knowledge *k,
		    struct instance *ip,
		    struct params *p) {

    if (ip->nconditions != k->nconditions) {
        fprintf(stderr, "number of conditions of test instance doesn't match knowledge file");
        exit(1);
    }

    return eval_neuron(k, ip, p, 0);
}
