#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "hamming.h"


/* XXX should be changed to use XML */

struct knowledge *read_hamming(FILE *f, struct params *p) {
    struct knowledge *k = malloc(sizeof (*k));
    
    assert(k);
    k->iip = read_instances(f);

    return k;
}
