#include <stdlib.h>

#include "../mll.h"
#include "hamming.h"

/* XXX should be changed to use XML */

struct knowledge *read_hamming(FILE *f) {
    int i, t;
    struct knowledge *k = malloc(sizeof (*k));
    
    assert(k);

    k->iip = read_instances(f);

    return k;
}
