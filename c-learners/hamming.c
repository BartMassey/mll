/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <sys/times.h>

#include "bitset.h"
#include "read_instances.h"

int ntraining;
int verbose = 1;
int benchmark = 0;

char *usage = "usage: hamming [-s seed] [-B]";
char *options = "s:B";
struct option long_options[] = {
    {"seed", 1, 0, 's'},
    {"benchmark", 0, 0, 'B'},
    {0, 0, 0, 0}
};

int vote(int t) {
    int i;
    int *ds = malloc(ninstances * sizeof(*ds));
    int min_hdist = ntraining + 1;
    int npos = 0, nneg = 0;
    for (i = 0; i < ntraining; i++) {
	bitset x = bs_xor(instances[i].conditions, instances[t].conditions);
	ds[i] = bs_popcount(x);
	bs_free(x);
	if (ds[i] < min_hdist)
	    min_hdist = ds[i];
    }
    for (i = 0; i < ntraining; i++)
	if (ds[i] == min_hdist) {
	    if (instances[i].sign == 1)
		npos++;
            else
		nneg++;
	}
    free(ds);
    return npos - nneg;
}

int sgn(int x) {
    if (x > 0)
	return 1;
    if (x < 0)
	return -1;
    return 0;
}

/*ARGSUSED*/
int main(int argc, char **argv) {
    int i, ch, seed;
    int false_pos = 0, false_neg = 0, ambig = 0;
    struct tms t1, t2;

    seed = time(NULL) ^ getpid();

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
        case 's':  seed = atoi(optarg); break;
        case 'B':  verbose = 0; benchmark = 1; break;
        default:  fprintf(stderr, "%s\n", usage); exit(1);
        }
    }

    if (verbose)
	printf("seed: %d\n", seed);

    srandom(seed);

    times(&t1);
    read_instances();
    times(&t2);
    printf("time(read): %ld\n", t2.tms_utime + t2.tms_stime - (t1.tms_utime + t1.tms_stime));

    printf("time(train): 0\n");

    shuffle_instances();
    ntraining = ninstances - ninstances / 3;
    for(i = ntraining; i < ninstances; i++) {
	int j = vote(i);
	if (sgn(j) != instances[i].sign) {
	    if (j == 0) {
		if(verbose || benchmark)
		    printf("mistake: %d 0\n", instances[i].name);
		ambig++;
	    } else if (j > 0) {
		if(verbose || benchmark)
		    printf("mistake: %d %d\n", instances[i].name, j);
		false_pos++;
	    } else {
		if(verbose || benchmark)
		    printf("mistake: %d %d\n", instances[i].name, j);
		false_neg++;
	    }
	}
    }
    printf("%d %d %d %d\n",
	   ninstances - ntraining,
	   ambig, false_pos, false_neg);

    times(&t1);
    printf("time(class): %ld\n", t1.tms_utime + t1.tms_stime - (t2.tms_utime + t2.tms_stime));

    exit(0);
}

