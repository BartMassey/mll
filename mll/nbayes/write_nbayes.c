#include "../mll.h"
#include "nbayes.h"

/* XXX should be changed to use XML */
/* XXX should be able to fail */

int write_nbayes(FILE *f, struct knowledge *k) {
    int i;
    
    fprintf(f, "%d %d\n", k->nsign[0], k->nsign[1]);
    fprintf(f, "%d\n", k->nconditions);
    for (i = 0; i < k->nconditions; i++)
	fprintf(f, "%d %d\n", k->counts[i][0], k->counts[i][1]);
    return k->nconditions;
}
