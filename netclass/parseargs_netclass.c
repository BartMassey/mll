#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <string.h>

#include "../mll.h"

static char *usage_msg = "learner: netclass: usage: -a netclass";

static char *options = "";
static struct option long_options[] = {
    {0, 0, 0, 0}
};

static void usage(void) {
    fprintf(stderr, "%s\n", usage_msg);
    exit(1);
}

struct params *parseargs_netclass(int argc, char **argv) {
    return NULL;
}
