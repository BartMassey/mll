/*
 * Private definitions for dtree learner
 */

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

    bitset *pos_conditions,
	   *neg_conditions;

    // track instance count for sizeof [pos/neg_conditions]
    int ninstances;
};

struct params {
    double min_gain,
	   min_chisquare;
};


dtree *make_dtree(bitset, bitset);


inline static float log2(float n) {
    static float kconv = 0;
    if (kconv == 0)
        kconv = 1.0 / log(2.0);
    return log(n) * kconv;
}
