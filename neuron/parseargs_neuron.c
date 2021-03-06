#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "../mll.h"
#include "neuron.h"

static char *usage_msg = "learner: neuron: usage: -a neuron [-r rate] [-t/-T trials] [-c clip] [-k sigmoid_k] [-e stoperror] [-d] [-D] [-A]";

static char *options = "r:t:T:c:k:e:dAD";
static struct option long_options[] = {
    {"rate", 1, 0, 'r'},
    {"trials", 1, 0, 't'},
    {"forcetrials", 1, 0, 'T'},
    {"clip", 1, 0, 'c'},
    {"sigmoid", 1, 0, 'k'},
    {"stoperror", 1, 0, 'e'},
    {"delta", 0, 0, 'd'},
    {"average", 0, 0, 'A'},
    {"ldebug", 0, 0, 'D'},
    {0, 0, 0, 0}
};

static void usage(void) {
    fprintf(stderr, "%s\n", usage_msg);
    exit(1);
}

struct params *parseargs_neuron(int argc, char **argv) {
    int ch;
    struct params *p = malloc(sizeof (*p));

    assert(p);

    // Init with default values
    p->trials = 100;
    p->alltrials = 0;
    p->use_diff = 0;
    p->rate = 0.001;
    p->sigmoid_k = 3;
    p->clip = 10.0;
    p->average = 1;
    p->debug = 0;
    p->stopfrac = 0.005;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
	case 't':  p->trials = atoi(optarg); break;
	case 'T':  p->trials = atoi(optarg); p->alltrials = 1; break;
	case 'r':  p->rate = atof(optarg); break;
	case 'c':  p->clip = atof(optarg); break;
        case 'e':  p->stopfrac = atof(optarg); break;
	case 'k':  p->sigmoid_k = atof(optarg); p->use_sigmoid = 1;
	/* XXX fall through */
	case 'd':  p->use_diff = 1; break;
	case 'A':  p->average = 0; break;
        case 'D':  p->debug = 1; break;
        default:  usage();
        }
    }

    return p;
}

