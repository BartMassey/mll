/*
 * Private definitions for dtree learner
 */

struct knowledge {
    dtree *tree;
};

struct params {
    double min_gain,
	   min_chisquare;
};


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

