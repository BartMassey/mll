#include <stdlib.h>

#include "../mll.h"
#include "nbayes.h"

void kfree_nbayes(struct knowledge *k) {
    free(k->counts);
    free(k);
}
