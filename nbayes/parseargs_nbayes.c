#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "../mll.h"
#include "nbayes.h"

#ifdef NBAYES_DEMO_ARG

static char *usage_msg = "learner: nbayes: usage: -a nbayes [-x demo] [-D]";

static char *options = "x:D";
static struct option long_options[] = {
    {"xoptions", 1, 0, 'x'},
    {"ldebug", 1, 0, 'D'},
    {0, 0, 0, 0}
};

#else

static char *usage_msg = "learner: nbayes: usage: -a nbayes [-D]";

static char *options = "D";
static struct option long_options[] = {
    {"ldebug", 1, 0, 'D'},
    {0, 0, 0, 0}
};

#endif

static void usage(void) {
    fprintf(stderr, "%s\n", usage_msg);
    exit(1);
}

struct params *parseargs_nbayes(int argc, char **argv) {
    int ch;
    struct params *p = malloc(sizeof(*p));
    assert(p);

    p->debug = 0;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
	switch(ch) {
#ifdef NBAYES_DEMO_ARG
	case 'x':
	    if (strcmp(optarg, "demo"))
		usage();
	    break;
#endif
        case 'D':  p->debug = 1; break;
	default:  usage();
	}
    }
    return p;
}
