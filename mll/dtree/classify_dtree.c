#include <math.h>

#include "../mll.h"
#include "../instances.h"
#include "dtree.h"


dtree_type classify_subtree(dtree *d, struct instance *ip) {
    switch(d->tag) {
    case DT_POS:
    case DT_NEG:
        return d->tag;
    case DT_MIXED:
        if (d->val.leaf.pos > d->val.leaf.neg)
            return DT_POS;
        if (d->val.leaf.pos < d->val.leaf.neg)
            return DT_NEG;
        return DT_MIXED;
    case DT_NODE:
        if (bs_isset(ip->conditions, d->val.node.attr))
            return classify(d->val.node.pos, ip);
        return classify(d->val.node.neg, ip);
    case DT_EMPTY:
        abort();
    }
    abort();
}


int classify_dtree(struct knowledge *k,
		   struct instance *ip,
		   struct params *p) {

    dtree_type c = classify_subtree(k->tree, ip);

    if (c == DT_POS)
      return 1;
    if (c == DT_NEG)
      return -1;

    return 0;
}
