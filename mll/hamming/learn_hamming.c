#include <stdlib.h>

#include "../mll.h"
#include "hamming.h"

struct knowledge *learn_hamming(struct instances *ip,
			        struct params *p) {
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);

    // XXX may want to copy data since theres no guarantee that 
    //     caller will keep data around [ip] around until 
    //     write_hamming() is called.

    k->ninstances = ip->ninstances;
    k->instances = ip->instances;

    return k;
}
