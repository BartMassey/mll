#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "dtree.h"


/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
void init_masks(struct knowledge *k) {
    k->instance_mask = bs_mask(k->ninstances);
    k->condition_mask = bs_mask(k->nconditions);
}

/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
void init_instances(struct knowledge *k, 
                    struct instances *iip) {
    int i;

    // Create mask of positive instances
    k->pos_instances = bs_new(k->ninstances);
    for (i = 0; i < k->ninstances; i++)
        if (iip->instances[i]->sign > 0)
            bs_lset(k->pos_instances, i);

    // Create negation of positive instances
    k->neg_instances = bs_not(k->pos_instances);

    // AND negative instances with instance_mask
    // XXX Bart, is this necessary since all bits 
    //     in [instance_mask] are set?
    bs_land(k->neg_instances, k->instance_mask);
}

/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
void init_condition_cache(struct knowledge *k, 
                          struct instances *iip) {
    int i, j;

    // Create bitset arrays of size [nconditions]
    k->pos_conditions = malloc(iip->nconditions * sizeof(bitset));
    assert(k->pos_conditions);
    k->neg_conditions = malloc(iip->nconditions * sizeof(bitset));
    assert(k->neg_conditions);

    for (i = 0; i < iip->nconditions; i++) {
        // Set which instances displayed condition [i]
        bitset ntmp, ptmp;
        ptmp = bs_new(k->ninstances);
        for (j = 0; j < k->ninstances; j++) {
            if (bs_isset(iip->instances[j]->conditions, i))
                bs_lset(ptmp, j);
        }
        k->pos_conditions[i] = ptmp;
        ntmp = bs_not(ptmp);
        bs_land(ntmp, k->instance_mask);
        k->neg_conditions[i] = ntmp;
    }
}

/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
struct knowledge *learn_dtree(struct instances *iip,
			      struct params *p) {
    bitset no_attrs;
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);
    k->ninstances = iip->ninstances;
    k->nconditions = iip->nconditions;

    init_masks(k);
    init_instances(k, iip);
    init_condition_cache(k, iip);

    no_attrs = bs_new(iip->nconditions);

    // Make empty dtree
    k->tree = make_dtree(k, p, no_attrs, k->instance_mask);

    bs_free(no_attrs);

    return k;
}
