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

double rate = 0.001;
int trials = 100;
int use_diff = 0;
int use_sigmoid = 0;
int average = 1;
double sigmoid_k = 3;
double clip = 10.0;
int ntraining, nvalidation;

char *usage = "neuron: usage: neuron [-r rate] [-t trials] [-c clip] [-s seed] [-o weights_file] [-k sigmoid_k] [-davB]";
char *options = "r:t:c:s:o:k:davB";
struct option long_options[] = {
    {"rate", 1, 0, 'r'},
    {"trials", 1, 0, 't'},
    {"clip", 1, 0, 'c'},
    {"seed", 1, 0, 's'},
    {"offline", 1, 0, 'o'},
    {"sigmoid", 1, 0, 'k'},
    {"delta", 0, 0, 'd'},
    {"average", 0, 0, 'a'},
    {"verbose", 0, 0, 'v'},
    {"benchmark", 0, 0, 'B'},
    {0, 0, 0, 0}
};

double *averages;
double *weights, *best_weights;
int best_score;
double *delta;

int verbose = 1;
int benchmark = 0;

/*
 * Returns one of -0.5, -0.4 , ... , -0.1, 0.1, 0.2, ..., 0.5
 * selected via a PRNG.
 */
double small_rand(void) {
    long v = ((unsigned long) random()) % 10 - 5;
    assert(v >= -5 && v <= 4);
    if (v < 0)
	return v / (double) 10;
    return (v + 1) / (double) 10;
}

void init_weights(void) {
    int i;

    weights = malloc((nconditions + 1) * sizeof(*weights));
    assert(weights);
    for (i = 0; i < nconditions + 1; i++)
	weights[i] = small_rand() / 100;

    delta = malloc((nconditions + 1) * sizeof(*delta));
    assert(delta);
    best_weights = malloc((nconditions + 1) * sizeof(*best_weights));
    best_score = -1;

    if (average) {
	averages = malloc(nconditions * sizeof(*averages));
	assert(averages);
	for (i = 0; i < nconditions; i++) {
	    int j;
	    double t = 0;

	    for (j = 0; j < ninstances; j++)
		if (bs_isset(instances[j].conditions, i))
		    t += 1.0;
	    averages[i] = t / ninstances;
	}
    }
}

double sgn(double x) {
    if (x > 0)
	return 1;
    if (x < 0)
	return -1;
    return 0;
}

double sigmoid(double x, double k) {
    double ex = exp(-x * k);
    return (1 - ex) / (1 + ex);
}


/* Maple sez this is ok... */
double ddx_sigmoid(double x, double k) {
    double ex = exp(-x * k);
    double den = 1 + ex;
    den *= den;
    return 2 * k * ex / den;
}

double evaluate(struct instance *x, int use_linear) {
    int i;
    double total = 0;

    if (average) {
	for (i = 0; i < nconditions; i++)
	    total += weights[i] * (bs_isset(x->conditions, i) - averages[i]);
    } else {
	for (i = 0; i < nconditions; i++)
	    total += weights[i] * bs_isset(x->conditions, i);
    }

    total += weights[nconditions];
    if (use_linear) {
	if (use_sigmoid)
	    return sigmoid(total, sigmoid_k);
	return total;
    }
    return sgn(total);
}

double score_weights(void) {
    int i;
    enum dataset { TRAINING, VALIDATION };
    enum category { GOOD, FALSE_POSITIVE, FALSE_NEGATIVE };
    int count[2][3] = {{0,0,0},{0,0,0}};
    enum dataset dataset;
    enum category category;
    int validerr;

    // Score against validation set (middle of input set)
    for (i = 0; i < nvalidation; i++) {
	double v = evaluate(&instances[i], 0);
	if (i < ntraining)
	    dataset = TRAINING;
	else
	    dataset = VALIDATION;
	if (v > 0 && instances[i].sign < 0)
	    category = FALSE_POSITIVE;
	else if (v < 0 && instances[i].sign > 0)
	    category = FALSE_NEGATIVE;
	else if (v == 0)
	    abort();
	else
	    category = GOOD;
	count[dataset][category]++;
    }
    if (verbose)
	printf("%d %d %d",
	   count[TRAINING][FALSE_POSITIVE] +
	   count[TRAINING][FALSE_NEGATIVE],
	   count[VALIDATION][FALSE_POSITIVE],
	   count[VALIDATION][FALSE_NEGATIVE]);
    if (count[VALIDATION][GOOD] > best_score) {
	for (i = 0; i < nconditions + 1; i++)
	    best_weights[i] = weights[i];
	best_score = count[VALIDATION][GOOD];
	if (verbose)
	    printf("*");
    }
    if (verbose)
	printf("\n");
    validerr = count[VALIDATION][FALSE_POSITIVE] +
	       count[VALIDATION][FALSE_NEGATIVE];
    return (double) validerr / (validerr + count[VALIDATION][GOOD]);
}

void classify(void) {
    int i;
    int false_pos = 0, false_neg = 0, ambig = 0;

    // Score against classification set (tail of input set)
    for (i = nvalidation; i < ninstances; i++) {
	double v = evaluate(&instances[i], 0);
	if (v > 0 && instances[i].sign < 0) {
	    if (verbose || benchmark)
		printf("mistake: %d %g\n", instances[i].name, v);
	    false_pos++;
	} else if (v < 0 && instances[i].sign > 0) {
	    if (verbose || benchmark)
		printf("mistake: %d %g\n", instances[i].name, v);
	    false_neg++;
	} else if (v == 0) {
	    if (verbose || benchmark)
		printf("mistake: %d %g\n", instances[i].name, v);
	    ambig++;
	}
    }
    printf("%d %d %d %d\n",
	   ninstances - nvalidation,
	   ambig,
	   false_pos,
	   false_neg);
}

inline double do_clip(double w) {
    if (clip == 0)
	return w;
    if (w > clip)
	return clip;
    if (w < -clip)
	return -clip;
    return w;
}

void train(void) {
    int i, j;
    double v, wf;

    if (verbose && use_sigmoid)
	printf("%g ", sigmoid_k);
    for (i = 0; i < nconditions + 1; i++)
	delta[i] = 0;
    for (i = 0; i < ntraining; i++) {
	v = evaluate(&instances[i], use_diff);

	if (use_sigmoid) {
	    wf = rate * (instances[i].sign - v) * ddx_sigmoid(v, sigmoid_k);
	} else {
	    wf =  rate * (instances[i].sign - v);
	}

	for (j = 0; j < nconditions; j++)
	    if (bs_isset(instances[i].conditions, j))
		delta[j] += wf;
	delta[nconditions] += wf;
    }
    for (i = 0; i < nconditions; i++) {
	weights[i] = do_clip(weights[i] + delta[i]);
    }
    weights[nconditions] =
      do_clip(weights[nconditions] + delta[nconditions]);
}

void offline_mode(char *weights_file) {
    int i, t, nfeatures;
    FILE *fweights;
    struct instance inst;
    char ch;

    // Disable global 'average' var
    average = 0;

    assert(fweights = fopen(weights_file, "r"));

    assert(fscanf(fweights, "%d\n", &nconditions) == 1);
    weights = malloc((nconditions + 1) * sizeof(*weights));
    assert(weights);

    assert(fscanf(fweights, "k %lg\n", weights + nconditions) == 1);

    for (i=0; i < nconditions; i++)
      assert(fscanf(fweights, "%d %lg\n", &t, weights + i) == 2);

    fclose(fweights);

    inst.name = 0;
    inst.sign = 0;
    inst.conditions = bs_new(nconditions + 1);

    assert(scanf("%d\n", &nfeatures) == 1);
    assert(nfeatures == nconditions);
    for (i = 0; i < nfeatures; i++) {
	switch (ch = getchar()) {
	case '0': break;
	case '1': bs_lset(inst.conditions, i); break;
	default: assert(0);
	}
    }

    printf("%g", evaluate(&inst, 0));
}

int main(int argc, char **argv) {
    int i, ch, seed;
    double frac = 1.0;
    int alltrials = 0;

    struct tms t1, t2;

    seed = time(NULL) ^ getpid();

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
	switch(ch) {
	case 'r':  rate = atof(optarg); break;
	case 't':  trials = atoi(optarg); alltrials = 1; break;
	case 'c':  clip = atof(optarg); break;
	case 'k':  sigmoid_k = atof(optarg); use_sigmoid = 1;
	/* XXX fall through */
	case 'd':  use_diff = 1; break;
	case 'a':  average = 0; break;
	case 's':  seed = atoi(optarg); break;
	case 'o':  offline_mode(optarg); exit(0); break;
	case 'v':  verbose = 1; break;
	case 'B':  benchmark = 1; verbose = 0; break;
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

    shuffle_instances();
    nvalidation = ninstances - ninstances / 3;
    ntraining = nvalidation - nvalidation / 3;
    init_weights();
    if (verbose)
	printf("%d\n", ntraining);
    frac = score_weights();
    for (i = 0; i < trials; i++) {
	train();
	if (verbose)
	    printf("%d ", i + 1);
	frac = score_weights();
	if (!alltrials && frac < 0.005)
	    break;
    }
    if (verbose)
	printf("\n");

    if (verbose) {
	printf("%d\nk %g\n", nconditions, best_weights[nconditions]);
	for(i = 0; i < nconditions; i++)
	    printf("%d %g\n", i, best_weights[i]);
    }

    for (i = 0; i < nconditions + 1; i++)
	weights[i] = best_weights[i];

    times(&t1);
    printf("time(train): %ld\n", t1.tms_utime + t1.tms_stime - (t2.tms_utime + t2.tms_stime));

    classify();
    times(&t2);
    printf("time(class): %ld\n", t2.tms_utime + t2.tms_stime - (t2.tms_utime + t2.tms_stime));

    exit(0);
}
