#include <math.h>

#include "../mll.h"
#include "../instances.h"
#include "hamming.h"


int classify_hamming(struct knowledge *k,
		     struct instance *ip,
		     struct params *p) {

    int i;
    int *ds = malloc(k->ninstances * sizeof(*ds));
    int min_hdist = k->ninstances + 1;
    int npos = 0, nneg = 0;

    for (i = 0; i < k->ninstances; i++) {
        bitset x = bs_xor(k->instances[i].conditions, ip->conditions);
        ds[i] = bs_popcount(x);
        bs_free(x);
        if (ds[i] < min_hdist)
            min_hdist = ds[i];
    }
    for (i = 0; i < k->ninstances; i++)
        if (ds[i] == min_hdist) {
            if (k->instances[i].sign == 1)
                npos++;
            else
                nneg++;
        }
    free(ds);
    return npos - nneg;
}
