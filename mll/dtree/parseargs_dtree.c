#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "../mll.h"

static char *usage_msg = "learner: dtree: usage: -a dtree";

static char *options = "";
static struct option long_options[] = {
    {"mingain", 1, 0, 'g'},
    {"minchisquare", 1, 0, 'c'},
    {0, 0, 0, 0}
};

static void usage(void) {
    fprintf(stderr, "%s\n", usage_msg);
    exit(1);
}

struct params *parseargs_dtree(int argc, char **argv) {
    int ch;
    struct params *p = malloc(sizeof (*p));

    assert(p);

    // Init with default values
    p->min_gain = 0.001;
    p->min_chisquare = 0;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
	case 'g':  p->min_gain = atof(optarg); break;
	case 'c':  p->min_chisquare = atof(optarg); break;
        default:  usage();
        }
    }

    return p;
}

