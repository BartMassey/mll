#include <stdlib.h>

#include "../mll.h"
#include "dtree.h"

/* XXX should be changed to use XML */

struct dtree *read_tree(FILE *f) {
    char c;
    struct dtree *d = malloc(sizeof(*d));
    assert(d);

    if (fscanf(f, "%c", &c) != 1) {
        free(d);
        return 0;
    }

    switch (c) {
    case 'n':
        // read node
        d->tag = DT_NODE;
        if (!(fscanf(f, " %d\n", &d->val.node.attr) == 1) ||
            !(d->val.node.neg = read_tree(f)) ||
            !(d->val.node.pos = read_tree(f))) {
            free(d);
            return 0;
        }
        break;

    case 'E':
        // read empty leaf
        d->tag = DT_EMPTY;
        break;

    case 'N':
        // read negative leaf
        d->tag = DT_NEG;
        d->val.leaf.pos = 0;
        if (fscanf(f, " %d\n", &d->val.leaf.neg) != 1) {
            free(d);
            return 0;
        }
        break;

    case 'P':
        d->tag = DT_POS;
        d->val.leaf.neg = 0;
        if (fscanf(f, " %d\n", &d->val.leaf.pos) != 1) {
            free(d);
            return 0;
        }
        break;

    case 'M':
        d->tag = DT_MIXED;
        if (fscanf(f, " %d,%d\n", &d->val.leaf.neg, &d->val.leaf.pos) != 2) {
            free(d);
            return 0;
        }
        break;

    default:
        abort();
    }

    return d;
}

struct knowledge *read_dtree(FILE *f, struct params *p) {
    int l, i;
    char *s;
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);

    if (fscanf(f, "%d\n%d\n", &k->ninstances, &k->nconditions) != 2) {
        free(k);
        return 0;
    }

    l = max(k->ninstances, k->nconditions);

    // Set [l] to nearest multiple of 32
    if (l % 32)
        l += 32 - l % 32;

    s = malloc(l * sizeof(char) + 1);
    assert(s);

    if (!(fscanf(f, "%s\n", s) == 1) ||
        !(k->instance_mask = str2bitset(s))) {
        free(k);
        free(s);
        return 0;
    }

    if (!(fscanf(f, "%s\n", s) == 1) ||
        !(k->condition_mask = str2bitset(s))) {
        free(k);
        free(s);
        return 0;
    }

    if (!(fscanf(f, "%s\n", s) == 1) ||
        !(k->pos_instances = str2bitset(s))) {
        free(k);
        free(s);
        return 0;
    }

    if (!(fscanf(f, "%s\n", s) == 1) || 
        !(k->neg_instances = str2bitset(s))) {
        free(k);
        free(s);
        return 0;
    }

    k->pos_conditions = malloc(k->nconditions * sizeof(bitset));
    assert(k->pos_conditions);

    for (i = 0; i < k->nconditions; i++) {
        if (!(fscanf(f, "%s\n", s) == 1) ||
            !(k->pos_conditions[i] = str2bitset(s))) {
            free(k);
            free(s);
            return 0;
        }
    }

    k->neg_conditions = malloc(k->nconditions * sizeof(bitset));
    assert(k->neg_conditions);

    for (i = 0; i < k->nconditions; i++) {
        if (!(fscanf(f, "%s\n", s) == 1) ||
            !(k->neg_conditions[i] = str2bitset(s))) {
            free(k);
            free(s);
            return 0;
        }
    }

    if (!(k->tree = read_tree(f))) {
        free(k);
        free(s);
        return 0;
    }

    free(s);
    return k;
}
