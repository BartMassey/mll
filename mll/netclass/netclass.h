/*
 * Private definitions for neclass learner
 */

/* index is sign of instance */
typedef int count[2];

struct knowledge {
    int nsign[2];
    int nconditions;
    count *counts;
};

