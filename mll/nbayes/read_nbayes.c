#include <stdlib.h>

#include "mll.h"
#include "nbayes.h"

/* XXX should be changed to use XML */

struct knowledge *read_nbayes(FILE *f) {
    int i;
    struct knowledge *k = malloc(sizeof (*k));
    
    assert(k);
    if (fscanf(f, "%d %d\n", &k->nsign[0], &k->nsign[1]) != 2)
	return 0;
    if (fscanf(f, "%d\n", &k->nconditions) != 1)
	return 0;
    k->counts = malloc(k->nconditions * sizeof(*k->counts));
    assert(k->counts);
    for (i = 0; i < k->nconditions; i++)
	if (fscanf(f, "%d %d\n", &k->counts[i][0], &k->counts[i][1]) != 2)
	    return 0;
    return k;
}
