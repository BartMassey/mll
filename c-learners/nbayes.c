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
#include <string.h>
#include <sys/times.h>

#include "bitset.h"
#include "read_instances.h"


#ifdef USE_LOGS
inline static float log2(float n) {
    static float kconv = 0;
    if (kconv == 0)
	kconv = 1.0 / log(2.0);
    return log(n) * kconv;
}
#endif

float m = 1.0;
float graham_probability = 0.9;
float graham_threshold;
int graham_mincount = 5;
float bias = 1.0;
enum { C_GRAHAM, C_ROBINSON, C_NBAYES } classifier = C_NBAYES;

char *usage = "nbayes: usage: nbayes [-c graham|robinson|naive] [-m equiv-samples] [-p graham_probability] [-n graham_mincount] [-b bias] [-s seed] [-Bd]";
char *options = "m:c:p:n:b:s:Bd";
struct option long_options[] = {
    {"c", 1, 0, 'c'},
    {"m", 1, 0, 'm'},
    {"p", 1, 0, 'p'},
    {"n", 1, 0, 'n'},
    {"b", 1, 0, 'b'},
    {"s", 1, 0, 's'},
    {"d", 0, 0, 'd'},
    {"B", 0, 0, 's'},
    {0, 0, 0, 0}
};

int ntraining;
int debug = 0;
int verbose = 1;
int benchmark = 0;

/* index is sign of instance */
typedef int count[2];

count *counts;

int nsign[2];

void init_counts(void) {
    int i, j;
    
    counts = malloc(ntraining * sizeof(*counts));
    assert(counts);

    for (i = 0; i < nconditions; i++)
	for (j = 0; j < 2; j++)
	    counts[i][j] = 0;

    for (i = 0; i < 2; i++)
	nsign[i] = 0;
    for (i = 0; i < ntraining; i++) {
	int s = !(instances[i].sign < 0);
	nsign[s]++;
    }

    for (i = 0; i < nconditions; i++) {
	for (j = 0; j < ntraining; j++) {
	    int s = !(instances[j].sign < 0);
	    if (bs_isset(instances[j].conditions, i))
		counts[i][s]++;
	}
    }
}


float *memory = 0;
int nmemory = 15;

void forget(void) {
    int i;
    if (!memory) {
	memory = malloc(nmemory * sizeof(*memory));
	assert(memory);
    }
    for (i = 0; i < nmemory; i++)
	memory[i] = 0.5;
}

void remember(float p) {
    int i, imin;
    float min;
    for (i = 0; i < nmemory; i++)
	if (fabs(p - 0.5) > fabs(memory[i] - 0.5))
	    break;
    if (i >= nmemory)
	return;
    min = fabs(memory[0] - 0.5);
    imin = 0;
    for (i = 1; i < nmemory; i++) {
	float f = fabs(memory[i] - 0.5);
	if (f < min) {
	    min = f;
	    imin = i;
	}
    }
    memory[imin] = p;
}

INLINE float clamp(float f) {
    if (f < 0.01)
	return 0.01;
    if (f > 0.99)
	return 0.99;
    return f;
}

int classify(int i) {
    int j, s;
    double score[2];
    double tprob = 0.0;
    switch (classifier) {
    case C_NBAYES:
	for (j = 0; j < 2; j++) {
#ifdef USE_LOGS
	    score[j] = 0.0;
#else
	    score[j] = 1.0;
#endif
	}
	for (j = 0; j < nconditions; j++) {
	    int c;
	    c = bs_isset(instances[i].conditions, j);
	    for (s = 0; s < 2; s++) {
		int n;
		if (c)
		    n = counts[j][s];
		else
		    n = nsign[s] - counts[j][s];
#ifdef USE_LOGS
		score[s] += log2(n + 0.5 * m) - log2(nsign[s] + m);
#else
		score[s] *= (n + 0.5 * m) / (nsign[s] + m);
#endif
	    }
	}
#ifdef USE_LOGS
	for (s = 0; s < 2; s++)
	    score[s] += log2(nsign[s]);
#else
	for (s = 0; s < 2; s++)
	    score[s] *= nsign[s];
#endif
	if (score[0] > score[1])
	    return -1;
	if (score[1] > score[0])
	    return 1;
	return 0;
    case C_GRAHAM:
	forget();
	for (j = 0; j < nconditions; j++) {
	    float p0, p1;
	    if (!bs_isset(instances[i].conditions, j))
		continue;
	    if (bias * counts[j][0] +
		counts[j][1] < graham_mincount)
		continue;

	    p0 = clamp((float)(bias * counts[j][0]) / nsign[0]);
	    p1 = clamp((float)(counts[j][1]) / nsign[1]);
	    remember(clamp(p1 / (p0 + p1)));
	}
	if (debug) {
	    printf("memory is:");
	    for (j = 0; j < nmemory; j++)
		printf(" %g", memory[j]);
	    printf("\n");
	}
#ifdef USE_LOGS
	tprob = 0.0;
	for (j = 0; j < nmemory; j++)
	    tprob += log2(1.0 - memory[j]) - log2(memory[j]);
	if (graham_threshold > tprob)
	    return 1;
#else
	tprob = 1.0;
	for (j = 0; j < nmemory; j++)
	    tprob *= (1.0 - memory[j]) / memory[j];
	if (graham_threshold > tprob)
	    return 1;
#endif
	return -1;
    case C_ROBINSON:
	abort(); /* not yet */
    }
    abort(); /* case screwup */
}

/*ARGSUSED*/
int main(int argc, char **argv) {
    int i, ch, seed;
    int ambig = 0, false_pos = 0, false_neg = 0;
    struct tms t1, t2;

    seed = time(NULL) ^ getpid();

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
	switch(ch) {
	case 'm':  m = atof(optarg); break;
	case 'p':  graham_probability = atof(optarg); break;
	case 'b':  bias = atof(optarg); break;
	case 'n':  graham_mincount = atoi(optarg); break;
	case 'c':
	    if (!strcmp(optarg, "graham")) {
		classifier = C_GRAHAM;
		bias = 2.0;
		break;
	    } else if (!strcmp(optarg, "nbayes")) {
		classifier = C_NBAYES;
		break;
	    } else if (!strcmp(optarg, "robinson")) {
		classifier = C_ROBINSON;
		break;
	    }
	    /* fall through */
	case 's': seed = atoi(optarg); break;
	case 'B': verbose = 0; benchmark = 1; break;
	case 'd': debug = 1; break;
	default:  fprintf(stderr, "%s\n", usage); exit(1);
	}
    }

    if (verbose)
	printf("seed: %d\n", seed);

    srandom(seed);

    graham_threshold = 1.0 / graham_probability - 1.0;
#ifdef USE_LOGS
    graham_threshold = log2(graham_threshold);
#endif

    times(&t1);
    read_instances();
    times(&t2);
    printf("time(read): %ld\n", t2.tms_utime + t2.tms_stime - (t1.tms_utime + t1.tms_stime));

    shuffle_instances();
    ntraining = ninstances - ninstances / 3;

    init_counts();

    times(&t1);
    printf("time(train): %ld\n", t1.tms_utime + t1.tms_stime - (t2.tms_utime + t2.tms_stime));

    for (i = ntraining; i < ninstances; i++) {
	int sign = classify(i);
	if (instances[i].sign > 0 && sign < 0) {
	    if (verbose || benchmark)
		printf("mistake: %d %d\n", instances[i].name, sign);
	    false_neg++;
	}
	else if (instances[i].sign < 0 && sign > 0) {
	    if (verbose || benchmark)
		printf("mistake: %d %d\n", instances[i].name, sign);
	    false_pos++;
	} else if (sign == 0) {
	    ambig++;
	    if (verbose || benchmark)
		printf("mistake: %d 0\n", instances[i].name);
	}
    }
    printf("%d %d %d %d\n",
	   ninstances - ntraining,
	   ambig, false_pos, false_neg);

    times(&t2);
    printf("time(class): %ld\n", t2.tms_utime + t2.tms_stime - (t1.tms_utime + t1.tms_stime));

    exit(0);
}
