#include <math.h>

#include "../mll.h"
#include "../instances.h"
#include "neuron.h"

int classify_neuron(struct knowledge *k,
		    struct instance *ip,
		    struct params *p) {
    return eval_neuron(k, ip, p, 0);
}
