#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "../mll.h"
#include "neuron.h"

static char *usage_msg = "learner: neuron: usage: -a neuron [-r rate] [-t trials] [-c clip] [-k sigmoid_k] [-d] [-f] [-A]";

static char *options = "r:t:c:k:dAf";
static struct option long_options[] = {
    {"rate", 1, 0, 'r'},
    {"trials", 1, 0, 't'},
    {"clip", 1, 0, 'c'},
    {"sigmoid", 1, 0, 'k'},
    {"delta", 0, 0, 'd'},
    {"average", 0, 0, 'A'},
    {"confidence", 0, 0, 'f'},
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
    p->conf = 0;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
	case 'r':  p->rate = atoi(optarg); break;
	case 't':  p->trials = atoi(optarg); p->alltrials = 1; break;
	case 'c':  p->clip = atof(optarg); break;
	case 'k':  p->sigmoid_k = atof(optarg); p->use_sigmoid = 1;
	/* XXX fall through */
	case 'd':  p->use_diff = 1; break;
	case 'A':  p->average = 0; break;
        case 'f':  p->conf = 1; break;
        default:  usage();
        }
    }

    return p;
}

