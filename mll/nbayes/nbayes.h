/*
 * Private definitions for nbayes learner
 */

/* index is sign of instance */
typedef int count[2];

struct knowledge {
    int nsign[2],
        nconditions;
    count *counts;
};

struct params {
    int conf;
};
