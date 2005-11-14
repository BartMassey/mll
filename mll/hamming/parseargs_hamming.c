#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "hamming.h"
#include "../mll.h"

static char *usage_msg = "learner: hamming: usage: -a hamming [-r radius] [-D]";

static char *options = "Dr:";
static struct option long_options[] = {
    {"radius", 1, 0, 'r'},
    {"ldebug", 0, 0, 'D'},
    {0, 0, 0, 0}
};

static void usage(void) {
    fprintf(stderr, "%s\n", usage_msg);
    exit(1);
}

struct params *parseargs_hamming(int argc, char **argv) {
    int ch;
    struct params *p = malloc(sizeof (*p));

    assert(p);

    // Init with default values
    p->radius = 1;
    p->debug = 0;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
	case 'r':  p->radius = atoi(optarg); break;
        case 'D':  p->debug = 1; break;
        default:  usage();
        }
    }

    return p;
}

