#include <stdlib.h>

#include "../mll.h"
#include "hamming.h"

void kfree_hamming(struct knowledge *k) {
    // free [k]'s instance data
    free_instances(k->iip);

    free(k);
}
