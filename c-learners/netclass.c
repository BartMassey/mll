#include <stdlib.h>
#include <unistd.h>
#include <getopt.h>
#include <math.h>
#include <assert.h>
#include <stdio.h>
#include <time.h>
#include <sys/times.h>

#include "bitset.h"
#include "read_instances.h"

#include NNHDR

float thresh = 0.5; /* threshold of detection */

float * fvector; /* array of feature counters */

int benchmark = 0;

char *usage = "usage: netclass [-s seed] [-B]";
char *options = "s:B";
struct option long_options[] = {
    {"seed", 1, 0, 's'},
    {"benchmark", 0, 0, 'B'},
    {0, 0, 0, 0}
};

int main(int argc, char ** argv)
{
    int seed = time(NULL) ^ getpid();
    int false_pos = 0, false_neg = 0, ambig = 0;
    struct tms t1, t2;
    int i, ch;

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
        switch(ch) {
        case 's':  seed = atoi(optarg); break;
        case 'B':  benchmark = 1; break;
        default:  fprintf(stderr, "%s\n", usage); exit(1);
        }
    }

    srandom(seed);

    times(&t1);
    read_instances();
    times(&t2);
    printf("time(read): %ld\n", t2.tms_utime + t2.tms_stime - (t1.tms_utime + t1.tms_stime));

    fvector = malloc(nconditions * sizeof(*fvector));

    for (i = 0; i < ninstances; i++) {
	float output;
	int j;
	
	for (j = 0; j < nconditions; j++)
	    fvector[j] = bs_isset(instances[i].conditions, j);

	/* the moment of truth */
	nn_classify(fvector, &output, 0);
	if (output < thresh && instances[i].sign == 1) {
	    if(benchmark)
		printf("mistake: %d %d\n", instances[i].name, -1);
	    false_neg++;
	} else if (output > thresh && instances[i].sign == -1) {
	    if(benchmark)
		printf("mistake: %d %d\n", instances[i].name, 1);
	    false_pos++;
	} else if (output == thresh) {
	    if(benchmark)
		printf("mistake: %d 0\n", instances[i].name);
	    ambig++;
	}
    }
    
    printf("%d %d %d %d\n",
	   ninstances, ambig, false_pos, false_neg);

    times(&t1);
    printf("time(class): %ld\n", t1.tms_utime + t1.tms_stime - (t2.tms_utime + t2.tms_stime));

    exit(0);
}

