#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "nbayes.h"

struct knowledge *learn_nbayes(struct instances *ip,
			       struct params *p) {
    int i, j;
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);
    k->counts = malloc(ip->nconditions * sizeof(*k->counts));
    assert(k->counts);
    k->nconditions = ip->nconditions;

    // initialize attribute counts
    for (i = 0; i < ip->nconditions; i++)
	for (j = 0; j < 2; j++)
	    k->counts[i][j] = 0;

    // initialize class counts
    for (i = 0; i < 2; i++)
	k->nsign[i] = 0;

    // compute instance counts for each class
    for (i = 0; i < ip->ninstances; i++) {
	int s = !(ip->instances[i]->sign < 0);
	k->nsign[s]++;
    }

    // compute instance counts for each attribute
    for (i = 0; i < ip->nconditions; i++) {
        // count instances with attribute [i] set/unset
	for (j = 0; j < ip->ninstances; j++) {
	    int s = !(ip->instances[j]->sign < 0);
	    if (bs_isset(ip->instances[j]->conditions, i))
		k->counts[i][s]++;
	}
    }
    return k;
}
