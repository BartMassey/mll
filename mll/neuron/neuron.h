/*
 * Private definitions for neuron learner
 */

/* index is sign of instance */
typedef int count[2];

struct knowledge {
    int nsign[2];
    int nconditions;
    count *counts;
};


/*
    {"rate", 1, 0, 'r'},
    {"trials", 1, 0, 't'},
    {"clip", 1, 0, 'c'},
    {"sigmoid", 1, 0, 'k'},
    {"delta", 0, 0, 'd'},
    {"average", 0, 0, 'a'},
*/

struct params {
    
};
