/*
 * Private definitions for dtree learner
 */

#include <math.h>
#include "../bitset.h"

#define max(x, y)       ((x < y) ? y : x)


struct dtree;

typedef struct {
    int pos, neg;
} dtree_leaf;

typedef struct {
    int attr;
    struct dtree *pos, *neg;
} dtree_node;

typedef enum {DT_POS, DT_NEG, DT_MIXED, DT_EMPTY, DT_NODE} dtree_type;

typedef struct dtree {
    dtree_type tag;
    union {
        dtree_leaf leaf;
        dtree_node node;
    } val;
} dtree;


struct knowledge {
    dtree *tree;

    /* bitset denoting positive and negative instances */
    bitset pos_instances,
	   neg_instances;

    bitset instance_mask,
	   condition_mask;

    // Array of bitsets, essentially creating an 
    // [nconditions] X [ninstances] 2d array.  Each bitset entry
    // flags which instances displayed the given condition.
    bitset *pos_conditions,
	   *neg_conditions;

    // track instance count for sizeof [pos/neg_conditions]
    int ninstances,
        nconditions;
};

struct params {
    double min_gain,
	   min_chisquare;
    int debug;
};


dtree *make_dtree(struct knowledge *k, struct params *p,
                  bitset, bitset);


/********************************************************
 * PARAM:
 * DESC: 
 * RETURN: 
 ********************************************************/
inline static float mylog2(float n) {
    static float kconv = 0;
    if (kconv == 0)
        kconv = 1.0 / log(2.0);
    return log(n) * kconv;
}
