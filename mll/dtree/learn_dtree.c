#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "dtree.h"


struct knowledge *learn_dtree(struct instances *ip,
			      struct params *p) {
    int i, j;
    bitset no_attrs;
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);
    k->ninstances = ip->ninstances;

// init_masks
    k->instance_mask = bs_mask(ip->ninstances);
    k->condition_mask = bs_mask(ip->nconditions);

// init_instances
    k->pos_instances = bs_new(ip->ninstances);
    for (i = 0; i < ip->ninstances; i++)
        if (ip->instances[i].sign > 0)
            bs_lset(k->pos_instances, i);
    k->neg_instances = bs_not(k->pos_instances);
    bs_land(k->neg_instances, k->instance_mask);

// init_condition_cache
    k->pos_conditions = malloc(ip->nconditions * sizeof(bitset));
    assert(k->pos_conditions);
    k->neg_conditions = malloc(ip->nconditions * sizeof(bitset));
    assert(k->neg_conditions);
    for (i = 0; i < ip->nconditions; i++) {
        bitset ntmp, ptmp;
        ptmp = bs_new(ip->ninstances);
        for (j = 0; j < ip->ninstances; j++) {
            if (bs_isset(ip->instances[j].conditions, i))
                bs_lset(ptmp, j);
        }
        k->pos_conditions[i] = ptmp;
        ntmp = bs_not(ptmp);
        bs_land(ntmp, k->instance_mask);
        k->neg_conditions[i] = ntmp;
    }

// xtras
    no_attrs = bs_new(nconditions);
    k->tree = make_dtree(k->no_attrs, k->instance_mask);

    // XXX not using [no_attrs] anymore?
    bs_free(no_attrs);

    return k;
}
