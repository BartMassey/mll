#include <stdlib.h>
#include <math.h>

#include "../mll.h"
#include "../instances.h"
#include "hamming.h"


int classify_hamming(struct knowledge *k,
		     struct instance *ip,
		     struct params *p) {
    int i, j, sign;
    int *ds = malloc(k->iip->ninstances * sizeof(*ds));
    int min_hdist = 0;
    int prev_hdist = -1;
    int npos = 0, nneg = 0;
    float conf;

    if (ip->nconditions != k->iip->nconditions) {
        fprintf(stderr, "number of conditions of test instance doesn't match knowledge file");
        exit(1);
    }

    /* XXX hideously inefficient algorithm */
    for (j = 0; j < p->radius; j++) {
	min_hdist = k->iip->nconditions + 1;
	for (i = 0; i < k->iip->ninstances; i++) {
	    bitset x = bs_xor(k->iip->instances[i]->conditions,
			      ip->conditions);
	    ds[i] = bs_popcount(x);
	    bs_free(x);
	    if (ds[i] > prev_hdist && ds[i] < min_hdist)
		min_hdist = ds[i];
	}
	prev_hdist = min_hdist;
    }

    for (i = 0; i < k->iip->ninstances; i++)
        if (ds[i] <= min_hdist) {
            if (k->iip->instances[i]->sign == 1)
                npos++;
            else
                nneg++;
        }
    free(ds);

    if (npos - nneg > 0)
        sign = 1;
    else if (npos - nneg < 0)
        sign = -1;
    else
        sign = 0;

    // Print confidence value for this classifiction
    if (p->debug) {

        // normalize hamming distance by number of conditions
        // XXX: don't normalize since other learners' conf 
        //      measure isn't normalized
//        conf = (1 - (float)min_hdist / ip->nconditions);
        conf = 1;

        // compute proportion of majority min distance samples
        if (npos > nneg)
            conf *= (float)npos / (npos + nneg);
        else
            conf *= (float)nneg / (npos + nneg);

        DEBUG_CLASS(ip->name, ip->sign, sign, conf);
    }

    return sign;
}
