#include <stdlib.h>

#include "../mll.h"
#include "hamming.h"

void kfree_hamming(struct knowledge *k) {
    // we don't manage [instances], so don't free it

    free(k);
}
