#include <math.h>
#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "dtree.h"


dtree *classify_subtree(dtree *d, struct instance *ip) {
    switch(d->tag) {
    case DT_NODE:
        if (bs_isset(ip->conditions, d->val.node.attr))
            return classify_subtree(d->val.node.pos, ip);
        return classify_subtree(d->val.node.neg, ip);

    case DT_POS:
    case DT_NEG:
    case DT_MIXED:
        return d;

    default:
        abort();
    }
    abort();
}


int classify_dtree(struct knowledge *k,
		   struct instance *ip,
		   struct params *p) {
    int class;
    float conf;
    dtree *d = classify_subtree(k->tree, ip);

    // non-mixed leaf node, return given classification
    if (d->tag == DT_POS || d->tag == DT_NEG) {
        conf = 1;

        if (d->val.leaf.pos > d->val.leaf.neg)
            class = 1;
        if (d->val.leaf.pos < d->val.leaf.neg)
            class = -1;

    // mixed node, return most common classification
    } else if (d->tag == DT_MIXED) {
        // Get absolute value of conf
        conf = fabs( ((float)d->val.leaf.pos - d->val.leaf.neg) / (d->val.leaf.pos + d->val.leaf.neg) );

        if (d->val.leaf.pos > d->val.leaf.neg)
            class = 1;
        else if (d->val.leaf.pos < d->val.leaf.neg)
            class = -1;
        else
            class = 0;

    // bad node type
    } else {
        conf = 0.5;
        class = 0;
    }

    if (p->conf)
        printf("conf(%s): %f\n", ip->name, conf);

    return class;
}
