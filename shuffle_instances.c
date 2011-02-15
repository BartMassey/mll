#include "mll.h"
#include "instances.h"

void shuffle_instances(struct instances *ip) {
    int i;
    extern unsigned long random();
    
    for (i = 0; i < ip->ninstances - 1; i++) {
	int j = random() % (ip->ninstances - i);
	int k = ip->ninstances - i - 1;
	struct instance *tmp = ip->instances[j];
	ip->instances[j] = ip->instances[k];
	ip->instances[k] = tmp;
    }
}

