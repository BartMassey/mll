/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <time.h>
#include <sys/times.h>

#include "mll.h"
#include "instances.h"

char *usage_msg = "psam: usage: psam [-b bias] [-s seed] [-dB] "
                  "(-l|-c|-C) knowledge < instances";
char *options = "b:s:dBl:c:C:";
struct option long_options[] = {
    {"bias", 1, 0, 'b'},
    {"seed", 1, 0, 's'},
    {"debug", 0, 0, 'd'},
    {"benchmark", 0, 0, 'B'},
    {"learn", 1, 0, 'l'},
    {"classify", 1, 0, 'c'},
    {"check", 1, 0, 'C'},
    {0, 0, 0, 0}
};

int ntraining;
int debug = 0;
int verbose = 1;
int benchmark = 0;
int bias = 0;

void usage(void) {
    fprintf(stderr, "%s\n", usage_msg);
    exit(1);
}

void print_times(char *what, struct tms *t1, struct tms *t2) {
    printf("time(%s): %ld\n", what,
	   t2->tms_utime + t2->tms_stime - (t1->tms_utime + t1->tms_stime));
}

int main(int argc, char **argv) {
    int i, ch, seed;
    int ambig = 0, false_pos = 0, false_neg = 0;
    struct tms t1, t2;
    enum mode {
	MODE_NONE, MODE_LEARN, MODE_CLASSIFY, MODE_CHECK
    } m = MODE_NONE;
    char *kfn = 0;
    FILE *kf;
    struct knowledge *k;
    struct instances *iip;
    struct instance *ip;

    seed = time(NULL) ^ getpid();

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
	switch(ch) {
	case 'b': bias = atoi(optarg); break;
	case 's': seed = atoi(optarg); break;
	case 'd': debug = 1; break;
	case 'B': verbose = 0; benchmark = 1; break;
	case 'l':
	    if (m != MODE_NONE)
		usage();
	    m = MODE_LEARN;
	    kfn = optarg;
	    break;
	case 'c':
	    if (m != MODE_NONE)
		usage();
	    m = MODE_CLASSIFY;
	    kfn = optarg;
	    break;
	case 'C':
	    if (m != MODE_NONE)
		usage();
	    m = MODE_CHECK;
	    kfn = optarg;
	    break;
	default:  usage();
	}
    }

    switch(m) {
    case MODE_LEARN:
	if (verbose)
	    printf("seed: %d\n", seed);
	srandom(seed);

	times(&t1);
	kf = fopen(kfn, "w");
	if (!kf) {
	    perror(kfn);
	    exit(1);
	}
	iip = read_instances(stdin);
	shuffle_instances(iip);
	times(&t2);
	print_times("read", &t1, &t2);

	t1 = t2;
	k = learn_nbayes(iip);
	times(&t2);
	print_times("train", &t1, &t2);

	t1 = t2;
	assert(write_nbayes(kf, k) >= 0);
	fclose(kf);
	times(&t2);
	print_times("write", &t1, &t2);
	break;
	

    case MODE_CHECK:
	times(&t1);
	kf = fopen(kfn, "w");
	if (!kf) {
	    perror(kfn);
	    exit(1);
	}
	iip = read_instances(stdin);
	k = read_nbayes(kf);
	times(&t2);
	print_times("read", &t1, &t2);

	t1 = t2;
	for (i = 0; i < iip->ninstances; i++) {
	    int sign = classify_nbayes(k, iip->instances[i]);
	    if (iip->instances[i]->sign > 0 && sign < 0) {
		if (verbose || benchmark)
		    printf("mistake: %s %d\n", iip->instances[i]->name, sign);
		false_neg++;
	    }
	    else if (iip->instances[i]->sign < 0 && sign > 0) {
		if (verbose || benchmark)
		    printf("mistake: %s %d\n", iip->instances[i]->name, sign);
		false_pos++;
	    } else if (sign == 0) {
		ambig++;
		if (verbose || benchmark)
		    printf("mistake: %s 0\n", iip->instances[i]->name);
	    }
	}
	printf("%d %d %d %d\n", iip->ninstances,
	       ambig, false_pos, false_neg);
	times(&t2);
	print_times("check", &t1, &t2);
	break;
    default:
	usage();
    }

    exit(0);
}
