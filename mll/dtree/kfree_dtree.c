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

    free_dtree(d->tree);

    bs_free(pos_instances);
    bs_free(neg_instances);
    bs_free(instance_mask);
    bs_free(condition_mask);

    for (i=0; i < k->ninstances; i++) {
      bs_free(pos_conditions[i]);
      bs_free(neg_conditions[i]);
    }

    free(pos_conditions);
    free(neg_conditions);

    free(k);
}
