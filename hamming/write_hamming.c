#include "../mll.h"
#include "../instances.h"
#include "hamming.h"


/* XXX should be changed to use XML */
/* XXX should be able to fail */

int write_hamming(FILE *f, struct knowledge *k) {
    write_instances(f, k->iip);

    return 1;
}
