#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "hamming.h"


int classify_hamming(struct knowledge *k,
		     struct instance *ip,
		     struct params *p) {

    int i;
    int *ds = malloc(k->iip->ninstances * sizeof(*ds));
    int min_hdist = k->iip->ninstances + 1;
    int npos = 0, nneg = 0;

    for (i = 0; i < k->iip->ninstances; i++) {
        bitset x = bs_xor(k->iip->instances[i]->conditions, ip->conditions);
        ds[i] = bs_popcount(x);
        bs_free(x);
        if (ds[i] < min_hdist)
            min_hdist = ds[i];
    }
    for (i = 0; i < k->iip->ninstances; i++)
        if (ds[i] == min_hdist) {
            if (k->iip->instances[i]->sign == 1)
                npos++;
            else
                nneg++;
        }
    free(ds);

    // Print confidence value for this classifiction
    if (p->conf)
        printf("conf: %f\n", fabs((float)(npos - nneg) / (npos + nneg)));

    if (npos - nneg < 0)
        return -1;

    return 0;
}
