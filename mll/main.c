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
#include <string.h>

#include "mll.h"
#include "instances.h"

static char *usage_msg = "learner: usage: learner [-b bias] [-s seed] [-dBL] \n"
                  "\t(-l|-c|-C) <knowledge-filename> "
                  "-a algorithm [<algorithm-options>] < instances\n"
		  "  -b   bias\n"
		  "  -s   PRNG seed\n"
		  "  -d   enable debugging message\n"
		  "  -B   enable benchmarking\n"
		  "  -v   verbose mode\n"
		  "  -L   list available learners and exit\n"
		  "  -S   disable shuffling of instances\n"
		  "  -l   learn instance set\n"
		  "  -c   classify instance\n"
		  "  -C   check against instance set\n"
		  "  -a   learning algorithm\n"
                  "  -V   echo classes while checking";
static char *options = "b:s:dvBLl:c:C:Sa:V";
static struct option long_options[] = {
    {"bias", 1, 0, 'b'},
    {"seed", 1, 0, 's'},
    {"debug", 0, 0, 'd'},
    {"verbose", 0, 0, 'v'},
    {"benchmark", 0, 0, 'B'},
    {"learners", 0, 0, 'L'},
    {"learn", 1, 0, 'l'},
    {"classify", 1, 0, 'c'},
    {"check", 1, 0, 'C'},
    {"no-shuffle", 0, 0, 'S'},
    {"algorithm", 1, 0, 'a'},

    {"verbosecheck", 0, 0, 'V'},

    {0, 0, 0, 0}
};

static int debug = 0;
static int verbose = 0;
static int benchmark = 0;
static int bias = 0;
static int shuffle = 1;

static struct learners {
    char *name;
    learn_t *learn;
    classify_t *classify;
    read_t *read;
    write_t *write;
    kfree_t *kfree;
    parseargs_t *parseargs;
} learners[] = {
    {"nbayes", learn_nbayes, classify_nbayes, read_nbayes,
     write_nbayes, kfree_nbayes, parseargs_nbayes},
    {"neuron", learn_neuron, classify_neuron, read_neuron,
     write_neuron, kfree_neuron, parseargs_neuron},
    {"hamming", learn_hamming, classify_hamming, read_hamming,
     write_hamming, kfree_hamming, parseargs_hamming},
    {"dtree", learn_dtree, classify_dtree, read_dtree,
     write_dtree, kfree_dtree, parseargs_dtree},
};

static void usage(void) {
    fprintf(stderr, "%s\n", usage_msg);
    exit(1);
}

static void print_times(char *what, struct tms *t1, struct tms *t2) {
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
    struct params *params = 0;
    struct learners *learner = 0;

    int verbose_check = 0;

    seed = time(NULL) ^ getpid();

    while (!learner &&
	   (ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
	switch(ch) {
	case 'b': bias = atoi(optarg); break;
	case 's': seed = atoi(optarg); break;
	case 'd': debug = 1; break;
	case 'v': verbose = 1; break;
	case 'B': benchmark = 1; break;
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
	case 'S':
	    shuffle = 0;
	    break;
	case 'a':
	    for (i = 0;
		 i < sizeof(learners)/sizeof(struct learners);
		 i++) {
		if (!strcmp(learners[i].name, optarg)) {
		    learner = &learners[i];
		    break;
		}
	    }
	    if (!learner) {
		fprintf(stderr,
			"learner: unknown learning algorithm %s\n",
			optarg);
		exit(1);
	    }
	    /* XXX: algorithm name becomes argv[0] */
	    argc -= optind - 1;
	    argv += optind - 1;
	    optind = 0;
	    params = learner->parseargs(argc, argv);
	    break;
	case 'L':
	    for (i = 0; i < sizeof(learners)/sizeof(struct learners); i++)
		printf("%s\n", learners[i].name);
	    exit(1);

	    // unnessessary, but consistent
	    break;

        case 'V':  verbose_check = 1; break;

	default:  usage();
	}
    }

    if (!learner) {
        fprintf(stderr, "missing learning algorithm\n");
        usage();
    }

    switch(m) {
    case MODE_LEARN:
	if (shuffle) {
	    if (verbose)
		printf("seed: %d\n", seed);
	    srandom(seed);
	}
	
	if (benchmark)
	    times(&t1);
	kf = fopen(kfn, "w");
	if (!kf) {
	    perror(kfn);
	    exit(1);
	}
	iip = read_instances(stdin);
	if (!iip) {
	    fprintf(stderr,"instance read failure\n");
	    exit(1);
	}
	if (shuffle)
	    shuffle_instances(iip);
	if (benchmark) {
	    times(&t2);
	    print_times("read", &t1, &t2);
	}

	if (benchmark)
	    t1 = t2;
	k = learner->learn(iip, params);
	if (benchmark) {
	    times(&t2);
	    print_times("train", &t1, &t2);
	}

	if (benchmark)
	    t1 = t2;
	assert(learner->write(kf, k) >= 0);
	fclose(kf);
	if (benchmark) {
	    times(&t2);
	    print_times("write", &t1, &t2);
	}

	learner->kfree(k);

	break;
	

    case MODE_CHECK:
    case MODE_CLASSIFY:
	if (benchmark)
	    times(&t1);
	kf = fopen(kfn, "r");
	if (!kf) {
	    perror(kfn);
	    exit(1);
	}
	k = learner->read(kf, params);
	if (!k) {
	    fprintf(stderr, "knowledge read failure\n");
	    exit(1);
	}
	iip = read_instances(stdin);
        if (!iip) {
            fprintf(stderr, "instance read failure\n");
            exit(1);
        }

	if (benchmark) {
	    times(&t2);
	    print_times("read", &t1, &t2);
	}

	if (benchmark)
	    t1 = t2;
	switch (m) {
	case MODE_CHECK:
	    for (i = 0; i < iip->ninstances; i++) {
		int sign = learner->classify(k, iip->instances[i], params);
		if (iip->instances[i]->sign > 0 && sign < 0) {
		    if (verbose || benchmark)
			printf("mistake: %s %d\n",
			       iip->instances[i]->name, sign);
		    false_neg++;
		}
		else if (iip->instances[i]->sign < 0 && sign > 0) {
		    if (verbose || benchmark)
			printf("mistake: %s %d\n",
			       iip->instances[i]->name, sign);
		    false_pos++;
		} else if (sign == 0) {
		    ambig++;
		    if (verbose || benchmark)
			printf("mistake: %s 0\n",
			       iip->instances[i]->name);
		}

                if (verbose_check)
                    printf("class(%s): %d %d\n", iip->instances[i]->name, iip->instances[i]->sign, sign);
	    }
	    printf("%d %d %d %d\n", iip->ninstances,
		   ambig, false_pos, false_neg);
	    break;
	case MODE_CLASSIFY:
	    for (i = 0; i < iip->ninstances; i++) {
		int sign = learner->classify(k, iip->instances[i], params);
		if (iip->instances[i]->sign != 0 &&
		    iip->instances[i]->sign != sign) {
		    if (verbose || benchmark)
			printf("mistake: %s %d\n",
			       iip->instances[i]->name, sign);
		}
		printf("%s %d\n", iip->instances[i]->name, sign);
	    }
	    break;
	default:
	    assert(0);
	}
	if (benchmark) {
	    times(&t2);
	    print_times("classify", &t1, &t2);
	}

	learner->kfree(k);

	break;
    default:
	usage();
    }

    if (params)
	free(params);

    exit(0);
}
