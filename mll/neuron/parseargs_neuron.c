#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "../mll.h"

static char *usage_msg = "learner: neuron: usage: -a neuron";

static char *options = "";
static struct option long_options[] = {
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

    /* Bart: average is disabled by default in "offline mode" of old code,
       should it be disabled here? 					   */
    p->average = 1;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
	case 'r':  p->rate = atoi(optarg); break;
	case 't':  p->trials = atoi(optarg); p->alltrials = 1; break;
	case 'c':  p->clip = atof(optarg); break;
	case 'k':  p->sigmoid = atof(optarg); p->use_sigmoid = 1;
	/* XXX fall through */
	case 'd':  p->delta = 1; break;
	case 'a':  p->average = 0; break;
        default:  usage();
        }
    }

    return p;
}

