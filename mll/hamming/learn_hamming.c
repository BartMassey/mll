#include <stdlib.h>

#include "../mll.h"
#include "hamming.h"

struct knowledge *learn_hamming(struct instances *ip,
			        struct params *p) {
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);

    k->iip = copy_instances(ip);

    return k;
}
