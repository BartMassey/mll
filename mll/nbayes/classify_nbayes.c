#include "mll.h"
#include "instances.h"
#include "nbayes.h"
#ifdef USE_LOGS
#include <math.h>
#endif

int classify_nbayes(struct knowledge *k,
		    struct instance *ip,
		    struct params *p) {
    int j, s;
    double score[2];
    float m = 1.0;

    for (j = 0; j < 2; j++) {
#ifdef USE_LOGS
	score[j] = 0.0;
#else
	score[j] = 1.0;
#endif
    }
    for (j = 0; j < ip->nconditions; j++) {
	int c;
	c = bs_isset(ip->conditions, j);
	for (s = 0; s < 2; s++) {
	    int n;
	    if (c)
		n = k->counts[j][s];
	    else
		n = k->nsign[s] - k->counts[j][s];
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
    if (score[0] > score[1])
	return -1;
    if (score[1] > score[0])
	return 1;
    return 0;
}
