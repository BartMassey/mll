#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "dtree.h"


struct knowledge *learn_dtree(struct instances *iip,
			      struct params *p) {

    init_masks();
    init_instances();
    init_condition_cache();

    no_attrs = bs_new(nconditions);
    // Start with initial tree with no
    d = make_dtree(no_attrs, instance_mask);



    int i, j;
    bitset no_attrs;
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);
    k->ninstances = iip->ninstances;

// init_masks
    k->instance_mask = bs_mask(iip->ninstances);
    k->condition_mask = bs_mask(iip->nconditions);

// init_instances
    k->pos_instances = bs_new(iip->ninstances);
    for (i = 0; i < iip->ninstances; i++)
        if (iip->instances[i]->sign > 0)
            bs_lset(k->pos_instances, i);
    k->neg_instances = bs_not(k->pos_instances);
    bs_land(k->neg_instances, k->instance_mask);

// init_condition_cache
    k->pos_conditions = malloc(iip->nconditions * sizeof(bitset));
    assert(k->pos_conditions);
    k->neg_conditions = malloc(iip->nconditions * sizeof(bitset));
    assert(k->neg_conditions);
    for (i = 0; i < iip->nconditions; i++) {
        bitset ntmp, ptmp;
        ptmp = bs_new(iip->ninstances);
        for (j = 0; j < iip->ninstances; j++) {
            if (bs_isset(iip->instances[j]->conditions, i))
                bs_lset(ptmp, j);
        }
        k->pos_conditions[i] = ptmp;
        ntmp = bs_not(ptmp);
        bs_land(ntmp, k->instance_mask);
        k->neg_conditions[i] = ntmp;
    }


    no_attrs = bs_new(iip->nconditions);

    // Make empty dtree
    k->tree = make_dtree(no_attrs, k->instance_mask);

    bs_free(no_attrs);

    return k;
}
