#include <stdarg.h>
#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "dtree.h"


/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
void debug(int debugmode, char *fmt, ...) {
    va_list ap;
    if (!debugmode)
	return;
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}


/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
float chisquare(int pos, int neg) {
    float avg = (pos + neg) / 2;
    float dpos = pos - avg;
    float dneg = neg - avg;
    return (dpos * dpos + dneg * dneg) / avg;
}

/********************************************************
 * PARAM:
 *   k    - knowledge info
 *   mask - total instances left
 * DESC: Create leaf node with counts of positive and
 *       negative instances in [mask].
 * RETURN: created leaf node
 ********************************************************/
dtree_leaf count_instances(struct knowledge *k,
                           bitset mask) {
    bitset tmp;
    int pos, neg;
    dtree_leaf l;

    // Compute number of positive instances in [mask]
    tmp = bs_and(k->pos_instances, mask);
    pos = bs_popcount(tmp);
    bs_free(tmp);

    // Compute number of negative instances in [mask]
    tmp = bs_and(k->neg_instances, mask);
    neg = bs_popcount(tmp);
    bs_free(tmp);

    // Assign computed counts
    l.pos = pos;
    l.neg = neg;

    return l;
}

/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
float an_entropy(int n, int tot) {
    return -((float)n / tot) * log2((float)n / tot);
}

/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
float instances_entropy(struct knowledge *k, bitset mask) {
    bitset tmp;
    int pos, neg, tot;

    tmp = bs_and(k->pos_instances, mask);
    pos = bs_popcount(tmp);
    bs_free(tmp);

    tmp = bs_and(k->neg_instances, mask);
    neg = bs_popcount(tmp);
    bs_free(tmp);

    if (pos == 0 || neg == 0)
        return 0;
    tot = pos + neg;
    return an_entropy(pos, tot) + an_entropy(neg, tot);
}

/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
float gain(struct knowledge *k, float entropy, bitset mask, int attr) {
    bitset posmask = bs_and(mask, k->pos_conditions[attr]);
    int pos = bs_popcount(posmask);

    bitset negmask = bs_and(mask, k->neg_conditions[attr]);
    int neg = bs_popcount(negmask);

    int tot = pos + neg;
    float gtmp = entropy -
        pos * instances_entropy(k, posmask) / tot -
        neg * instances_entropy(k, negmask) / tot;
    bs_free(posmask);
    bs_free(negmask);
    return gtmp;
}

/********************************************************
 * PARAM:
 *   attr_mask - mask of attributes already accounted 
 *               for by parent nodes
 * DESC: 
 * RETURN: 
 ********************************************************/
dtree *make_dtree(struct knowledge *k, struct params *p,
                  bitset attr_mask, bitset inst_mask) {
    dtree_leaf m;
    int best_attr, i;
    float best_gain, entropy, g;
    dtree *d, *pos, *neg;
    bitset posmask, negmask;

    d = malloc(sizeof(*d));
    assert(d);

    // Compute number of positive and negative instances
    m = count_instances(k, inst_mask);

    /* all positive */
    if (m.neg == 0 && m.pos > 0) {
#ifdef DEBUG_DTREE
        debug(p->debug, "positive leaf");
#endif
        d->val.leaf = m;
        d->tag = DT_POS;
        return d;
    }
    /* all negative */
    if (m.pos == 0 && m.neg > 0) {
#ifdef DEBUG_DTREE
        debug(p->debug, "negative leaf");
#endif
        d->val.leaf = m;
        d->tag = DT_NEG;
        return d;
    }
    /* corner case: no more attrs */
    if (bs_equal(attr_mask, k->condition_mask)) {
        debug(p->debug, "mixed leaf (no split)");
        d->val.leaf = m;
        d->tag = DT_MIXED;
        return d;
    }
    /* corner case: no more instances */
    if (m.pos == 0 && m.neg == 0) {
        debug(p->debug, "empty leaf");
        d->tag = DT_EMPTY;
        return d;
    }

#ifdef DEBUG_DTREE
    debug(p->debug, "considering %d instances", bs_popcount(inst_mask));
#endif
    /* find highest gain attr */

    entropy = instances_entropy(k, inst_mask);
    best_attr = -1;
    best_gain = -1;

    for (i = 0; i < k->nconditions; i++) {
        if (bs_isset(attr_mask, i))
            continue;
        g = gain(k, entropy, inst_mask, i);
        if (g > best_gain) {
            best_attr = i;
            best_gain = g;
        }
    }

    assert(best_attr >= 0);  /* weird gain */
#ifdef DEBUG_DTREE
    debug(p->debug, "best gain %g for %d", best_gain, best_attr);
#endif

    /* corner case: no more attrs */
    if (best_gain < p->min_gain) {
#ifdef DEBUG_DTREE
        debug(p->debug, "mixed leaf (low gain)");
#endif
        d->val.leaf = m;
        d->tag = DT_MIXED;
        return d;
    }

    /* corner case: no more significance */
    if (p->min_chisquare > 0 &&
        chisquare(m.pos, m.neg) < p->min_chisquare) {
#ifdef DEBUG_DTREE
        debug(p->debug, "mixed leaf (low significance)");
#endif
        d->val.leaf = m;
        d->tag = DT_MIXED;
        return d;
    }

    /* build pos and neg masks and recurse */
    bs_lset(attr_mask, best_attr);

#ifdef DEBUG_DTREE
    debug(p->debug, "build neg subtree");
#endif
    // build neg subtree
    negmask = bs_and(inst_mask, k->neg_conditions[best_attr]);
    neg = make_dtree(k, p, attr_mask, negmask);
    bs_free(negmask);

#ifdef DEBUG_DTREE
    debug(p->debug, "build pos subtree");
#endif
    // build pos subtree
    posmask = bs_and(inst_mask, k->pos_conditions[best_attr]);
    pos = make_dtree(k, p, attr_mask, posmask);
    bs_free(posmask);

    bs_lclear(attr_mask, best_attr);

#ifdef DEBUG_DTREE
    debug(p->debug, "finished interior node");
#endif

    d->val.node.attr = best_attr;
    d->val.node.neg = neg;
    d->val.node.pos = pos;
    d->tag = DT_NODE;

    return d;
}

