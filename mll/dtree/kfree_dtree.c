#include <stdlib.h>

#include "../mll.h"
#include "dtree.h"

void free_dtree(dtree *d) {
    if (d->tag == DT_NODE) {
        free_dtree(d->val.node.neg);
        free_dtree(d->val.node.pos);
    }
    free(d);
}


void kfree_dtree(struct knowledge *k) {
    int i;

    free_dtree(k->tree);

    bs_free(k->pos_instances);
    bs_free(k->neg_instances);
    bs_free(k->instance_mask);
    bs_free(k->condition_mask);

    for (i=0; i < k->ninstances; i++) {
      bs_free(k->pos_conditions[i]);
      bs_free(k->neg_conditions[i]);
    }

    free(k->pos_conditions);
    free(k->neg_conditions);

    free(k);
}
