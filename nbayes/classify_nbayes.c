#include <math.h>
#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "nbayes.h"

int classify_nbayes(struct knowledge *k,
		    struct instance *ip,
		    struct params *p) {
    int j, s, c, n, sign;
    double score[2];
    float m = 1.0, conf;

    if (ip->nconditions != k->nconditions) {
        fprintf(stderr, "number of conditions of test instance doesn't match knowledge file");
        exit(1);
    }

    // Initialize default scores for each classification
    for (j = 0; j < 2; j++) {
#ifdef USE_LOGS
	score[j] = 0.0;
#else
	score[j] = 1.0;
#endif
    }

    // Compute probability of each attribute
    for (j = 0; j < ip->nconditions; j++) {

        // Lookup value of attribute [j]
	c = bs_isset(ip->conditions, j);

        // Compute number of instances with attribute [j] equal to [c]
	for (s = 0; s < 2; s++) {
	    if (c)
                // get instance count for this attribute and target class
		n = k->counts[j][s];
	    else
                // get instance count for this target class less the 
                // instance count for this attribute and target class
                // i.e. count(target) - count(target && attribute)
		n = k->nsign[s] - k->counts[j][s];

            // update the score for this target class
#ifdef USE_LOGS
	    score[s] += log(n + 0.5 * m) - log(k->nsign[s] + m);
#else
	    score[s] *= (n + 0.5 * m) / (k->nsign[s] + m);
#endif
	}
    }

#ifdef USE_LOGS
    for (s = 0; s < 2; s++)
	score[s] += log(k->nsign[s]);
#else
    for (s = 0; s < 2; s++)
	score[s] *= k->nsign[s];
#endif

    if (score[1] > score[0])
	sign = 1;
    else if (score[0] > score[1])
	sign = -1;
    else
        sign = 0;

    if (p->debug) {

#ifdef USE_LOGS
        // exponentiate to get probability estimate
        score[0] = exp(score[0]);
        score[1] = exp(score[1]);
#endif

        if (score[0] > score[1])
            conf = (float)score[0] / (score[0] + score[1]);
        else if (score[0] < score[1])
            conf = (float)score[1] / (score[0] + score[1]);
        else
            conf = .5;

        DEBUG_CLASS(ip->name, ip->sign, sign, conf);
    }

    return sign;
}
