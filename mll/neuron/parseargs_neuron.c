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

/*
global
    {"bias", 1, 0, 'b'},
    {"seed", 1, 0, 's'},
    {"debug", 0, 0, 'd'},
    {"verbose", 0, 0, 'v'},
    {"benchmark", 0, 0, 'B'},
    {"learn", 1, 0, 'l'},
    {"classify", 1, 0, 'c'},
    {"check", 1, 0, 'C'},
    {"no-shuffle", 0, 0, 'S'},
    {"algorithm", 1, 0, 'a'},
neuron
    {"rate", 1, 0, 'r'},
    {"trials", 1, 0, 't'},
    {"clip", 1, 0, 'c'},
    {"sigmoid", 1, 0, 'k'},
    {"delta", 0, 0, 'd'},
    {"average", 0, 0, 'a'},
*/

struct params *parseargs_neuron(int argc, char **argv) {
    int ch;
    struct params *p = malloc((size_t) sizeof(struct params));

    if (!p)
	return NULL;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
	case 'r':
	    p->rate = optarg;
	    break;

	case 't':
	    p->trials = optarg;
	    break;

	case 'c':
	    p->clip = optarg;
	    break;

	case 'k':
	    p->sigmoid = optarg;
	    break;

	case 'd':
	    p->delta = optarg;
	    break;

	case 'a':
	    p->average = optarg;
	    break;

        default:  usage();
        }
    }
    return 0;
}

