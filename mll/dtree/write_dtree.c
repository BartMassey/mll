#include <stdlib.h>
#include "../mll.h"
#include "../bitset.h"
#include "dtree.h"

/* XXX should be changed to use XML */
/* XXX should be able to fail */

/****************************************************************
 * PARAM:
 *   f - file handle
 *   d - decision tree
 * DESC: Write contents of [d] to [f].
 *       Format:
 *         Node:       nx   (x is attribute index)
 *         Neg Leaf:   Nx   (x is neg count)
 *         Pos Leaf:   Lx   (x is pos count)
 *         Mixed Leaf: Mx,y (x is neg count, y is pos count)
 *         Empty Leaf: E
 * RETURN: none
 ****************************************************************/
void write_tree(FILE *f, struct dtree *d) {
    if (d->tag == DT_NODE) {
        fprintf(f, "n %d\n", d->val.node.attr);

        // write left subtree
        write_tree(f, d->val.node.neg);

        // write right subtree
        write_tree(f, d->val.node.pos);

    } else if (d->tag == DT_EMPTY)
        fprintf(f, "E\n");

    else if (d->tag == DT_NEG)
        fprintf(f, "N %d\n", d->val.leaf.neg);

    else if (d->tag == DT_POS)
        fprintf(f, "P %d\n", d->val.leaf.pos);

    else if (d->tag == DT_MIXED)
        fprintf(f, "M %d,%d\n", d->val.leaf.neg, d->val.leaf.pos);

    // Invalid tree type, abort
    else
        abort();
}

int write_dtree(FILE *f, struct knowledge *k) {
    char *s;
    int l, m, i;

    m = l = max(k->ninstances, k->nconditions);

    // Make [s] big enough for bits in largest bitset
    // by setting [m] to nearest multiple of 32
    if (m % 32)
        m += 32 - m % 32;

    s = malloc(m * sizeof(char) + 1);
    assert(s);

    // write ninstances, nconditions, instance_mask
    fprintf(f, "%d\n%d\n%s\n", k->ninstances, k->nconditions, 
            bitset2str(k->instance_mask, s));

    // write condition_mask
    fprintf(f, "%s\n", bitset2str(k->condition_mask, s));

    // write pos_instances
    fprintf(f, "%s\n", bitset2str(k->pos_instances, s));

    // write neg_instances
    fprintf(f, "%s\n", bitset2str(k->neg_instances, s));

    // write pos_conditions
    for (i = 0; i < k->nconditions; i++)
        fprintf(f, "%s\n",  bitset2str(k->pos_conditions[i], s));

    // write neg_conditions
    for (i = 0; i < k->nconditions; i++)
        fprintf(f, "%s\n",  bitset2str(k->neg_conditions[i], s));

    // write decision tree
    write_tree(f, k->tree);

    return 1;
}
