/*
 * Private definitions for hamming learner
 */

#include "../instances.h"

struct knowledge {
    int ninstances;
    struct instance **instances;
}
