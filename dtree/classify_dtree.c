#include <assert.h>
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
    int sign;
    float conf;
    dtree *d = classify_subtree(k->tree, ip);

    if (ip->nconditions != k->nconditions) {
        fprintf(stderr, "number of conditions of test instance doesn't match knowledge file");
        exit(1);
    }

    // non-mixed leaf node, return given classification
    if (d->tag == DT_POS || d->tag == DT_NEG) {
        conf = 1;

        if (d->val.leaf.pos > d->val.leaf.neg)
            sign = 1;
        else if (d->val.leaf.pos < d->val.leaf.neg)
            sign = -1;
	else
	    assert(0);

    // mixed node, return most common classification
    } else if (d->tag == DT_MIXED) {
        // Get absolute value of conf
//        conf = fabs( ((float)d->val.leaf.pos - d->val.leaf.neg) / (d->val.leaf.pos + d->val.leaf.neg) );

        if (d->val.leaf.pos > d->val.leaf.neg) {
            sign = 1;
            conf = (float)d->val.leaf.pos / (d->val.leaf.pos + d->val.leaf.neg);
        } else if (d->val.leaf.pos < d->val.leaf.neg) {
            sign = -1;
            conf = (float)d->val.leaf.neg / (d->val.leaf.pos + d->val.leaf.neg);
        } else {
            sign = 0;
            conf = .5;
        }

    // bad node type
    } else {
        conf = 0.5;
        sign = 0;
    }

    if (p->debug)
        DEBUG_CLASS(ip->name, ip->sign, sign, conf);

    return sign;
}
