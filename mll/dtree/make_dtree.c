#include "../mll.h"
#include "../instances.h"
#include "dtree.h"

dtree *make_dtree(bitset attr_mask, bitset inst_mask) {
    dtree_leaf m;
    int best_attr, i;
    float best_gain, entropy, g;
    dtree *d, *pos, *neg;
    bitset posmask, negmask;

    d = malloc(sizeof(*d));
    assert(d);

    m = count_instances(inst_mask);
    /* all positive */
    if (m.neg == 0 && m.pos > 0) {
#ifdef DEBUG_DTREE
        debug("positive leaf");
#endif
        d->val.leaf = m;
        d->tag = DT_POS;
        return d;
    }
    /* all negative */
    if (m.pos == 0 && m.neg > 0) {
#ifdef DEBUG_DTREE
        debug("negative leaf");
#endif
        d->val.leaf = m;
        d->tag = DT_NEG;
        return d;
    }
    /* corner case: no more attrs */
    if (bs_equal(attr_mask, condition_mask)) {
        debug("mixed leaf (no split)");
        d->val.leaf = m;
        d->tag = DT_MIXED;
        return d;
    }
    /* corner case: no more instances */
    if (m.pos == 0 && m.neg == 0) {
        debug("empty leaf");
        d->tag = DT_EMPTY;
        return d;
    }

#ifdef DEBUG_DTREE
    debug("considering %d instances", bs_popcount(inst_mask));
#endif
    /* find highest gain attr */

    entropy = instances_entropy(inst_mask);
    best_attr = -1;
    best_gain = -1;

    for (i = 0; i < nconditions; i++) {
        if (bs_isset(attr_mask, i))
            continue;
        g = gain(entropy, inst_mask, i);
        if (g > best_gain) {
            best_attr = i;
            best_gain = g;
        }
    }

    assert(best_attr >= 0);  /* weird gain */
#ifdef DEBUG_DTREE
    debug("best gain %g for %d", best_gain, best_attr);
#endif

    /* corner case: no more attrs */
    if (best_gain < min_gain) {
#ifdef DEBUG_DTREE
        debug("mixed leaf (low gain)");
#endif
        d->val.leaf = m;
        d->tag = DT_MIXED;
        return d;
    }

    /* corner case: no more significance */
    if (min_chisquare > 0 &&
        chisquare(m.pos, m.neg) < min_chisquare) {
#ifdef DEBUG_DTREE
        debug("mixed leaf (low significance)");
#endif
        d->val.leaf = m;
        d->tag = DT_MIXED;
        return d;
    }

    /* build pos and neg masks and recurse */
    bs_lset(attr_mask, best_attr);
#ifdef DEBUG_DTREE
    debug("build neg subtree");
#endif
    negmask = bs_and(inst_mask, neg_conditions[best_attr]);
    neg = make_dtree(attr_mask, negmask);
    bs_free(negmask);
#ifdef DEBUG_DTREE
    debug("build pos subtree");
#endif
    posmask = bs_and(inst_mask, pos_conditions[best_attr]);
    pos = make_dtree(attr_mask, posmask);
    bs_free(posmask);
    bs_lclear(attr_mask, best_attr);
#ifdef DEBUG_DTREE
    debug("finished interior node");
#endif

    d->val.node.attr = best_attr;
    d->val.node.neg = neg;
    d->val.node.pos = pos;
    d->tag = DT_NODE;

    return d;
}

dtree_leaf count_instances(bitset mask) {
    bitset tmp;
    int pos, neg;
    dtree_leaf l;

    tmp = bs_and(pos_instances, mask);
    pos = bs_popcount(tmp);
    bs_free(tmp);
    tmp = bs_and(neg_instances, mask);
    neg = bs_popcount(tmp);
    bs_free(tmp);
    l.pos = pos;
    l.neg = neg;
    return l;
}

float instances_entropy(bitset mask) {
    bitset tmp;
    int pos, neg, tot;

    tmp = bs_and(pos_instances, mask);
    pos = bs_popcount(tmp);
    bs_free(tmp);
    tmp = bs_and(neg_instances, mask);
    neg = bs_popcount(tmp);
    bs_free(tmp);
    if (pos == 0 || neg == 0)
        return 0;
    tot = pos + neg;
    return an_entropy(pos, tot) + an_entropy(neg, tot);
}

float an_entropy(int n, int tot) {
    return -((float)n / tot) * log2((float)n / tot);
}

float gain(float entropy, bitset mask, int attr) {
    bitset posmask = bs_and(mask, pos_conditions[attr]);
    int pos = bs_popcount(posmask);
    bitset negmask = bs_and(mask, neg_conditions[attr]);
    int neg = bs_popcount(negmask);
    int tot = pos + neg;
    float gtmp = entropy -
        pos * instances_entropy(posmask) / tot -
        neg * instances_entropy(negmask) / tot;
    bs_free(posmask);
    bs_free(negmask);
    return gtmp;
}

float chisquare(int pos, int neg) {
    float avg = (pos + neg) / 2;
    float dpos = pos - avg;
    float dneg = neg - avg;
    return (dpos * dpos + dneg * dneg) / avg;
}

