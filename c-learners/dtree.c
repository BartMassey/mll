/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

/* decision trees for spam data
 * Bart Massey & Mick Thomure  2002/11
 */

/* XXX instances mask is "positive"
   (shows instances included),
   attributes mask is negative */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <assert.h>
#include <stdarg.h>
#include <math.h>
#include <getopt.h>
#include <sys/times.h>

#include "bitset.h"
#include "read_instances.h"

int benchmark = 0;
int verbose = 1;

void debug(char *fmt, ...) {
    va_list ap;
#ifndef DEBUG
    if (benchmark || !verbose)
	return;
#endif
    va_start(ap, fmt);
    vfprintf(stderr, fmt, ap);
    fprintf(stderr, "\n");
    va_end(ap);
}


inline static float log2(float n) {
    static float kconv = 0;
    if (kconv == 0)
        kconv = 1.0 / log(2.0);
    return log(n) * kconv;
}

int ntraining;

double min_gain = 0.001;
double min_chisquare = 0;  /* 3.84 for p = 0.05 1 df */

char *usage = "dtree: usage: dtree [-g mingain] [-c minchisquare] [-s seed] [-B]";
char *options = "g:c:s:B";
struct option long_options[] = {
    {"mingain", 1, 0, 'g'},
    {"minchisquare", 1, 0, 'c'},
    {"seed", 1, 0, 's'},
    {"benchmark", 0, 0, 'B'},
    {0, 0, 0, 0}
};

bitset instance_mask, condition_mask;

void init_masks(void) {
    instance_mask = bs_mask(ntraining);
    condition_mask = bs_mask(nconditions);
}

bitset *pos_conditions, *neg_conditions;

void init_condition_cache() {
    int i, j;

    pos_conditions = malloc(nconditions * sizeof(bitset));
    assert(pos_conditions);
    neg_conditions = malloc(nconditions * sizeof(bitset));
    assert(neg_conditions);
    for (i = 0; i < nconditions; i++) {
	bitset ntmp, ptmp;
	ptmp = bs_new(ntraining);
	for (j = 0; j < ntraining; j++) {
	    if (bs_isset(instances[j].conditions, i))
		bs_lset(ptmp, j);
	}
	pos_conditions[i] = ptmp;
	ntmp = bs_not(ptmp);
	bs_land(ntmp, instance_mask);
	neg_conditions[i] = ntmp;
#ifdef DEBUG
	debug("condition %d: %d pos %d neg",
	      i, bs_popcount(pos_conditions[i]),
	      bs_popcount(neg_conditions[i]));
#endif
    }
}

/* bitset denoting positive instances */
bitset pos_instances, neg_instances;

void init_instances() {
    int i;

    pos_instances = bs_new(ntraining);
    for (i = 0; i < ntraining; i++)
	if (instances[i].sign > 0)
	    bs_lset(pos_instances, i);
    neg_instances = bs_not(pos_instances);
    bs_land(neg_instances, instance_mask);
}

float an_entropy(int n, int tot) {
    return -((float)n / tot) * log2((float)n / tot);
}

float instances_entropy(bitset mask) {
    bitset tmp;
    int pos, neg, tot;

    tmp = bs_and(pos_instances, mask);
    pos = bs_popcount(tmp);
    bs_free(tmp);
    tmp = bs_and(neg_instances, mask);
    neg = bs_popcount(tmp);
    bs_free(tmp);
    if (pos == 0 || neg == 0)
	return 0;
    tot = pos + neg;
    return an_entropy(pos, tot) + an_entropy(neg, tot);
}

float gain(float entropy, bitset mask, int attr) {
    bitset posmask = bs_and(mask, pos_conditions[attr]);
    int pos = bs_popcount(posmask);
    bitset negmask = bs_and(mask, neg_conditions[attr]);
    int neg = bs_popcount(negmask);
    int tot = pos + neg;
    float gtmp = entropy -
	pos * instances_entropy(posmask) / tot -
	neg * instances_entropy(negmask) / tot;
    bs_free(posmask);
    bs_free(negmask);
    return gtmp;
}

struct dtree;

typedef struct {
    int pos, neg;
} dtree_leaf;

typedef struct {
    int attr;
    struct dtree *pos, *neg;
} dtree_node;

typedef enum {DT_POS, DT_NEG, DT_MIXED, DT_EMPTY, DT_NODE} dtree_type;

typedef struct dtree {
    dtree_type tag;
    union {
	dtree_leaf leaf;
	dtree_node node;
    } val;
} dtree;



dtree_leaf count_instances(bitset mask) {
    bitset tmp;
    int pos, neg;
    dtree_leaf l;

    tmp = bs_and(pos_instances, mask);
    pos = bs_popcount(tmp);
    bs_free(tmp);
    tmp = bs_and(neg_instances, mask);
    neg = bs_popcount(tmp);
    bs_free(tmp);
    l.pos = pos;
    l.neg = neg;
    return l;
}

float chisquare(int pos, int neg) {
    float avg = (pos + neg) / 2;
    float dpos = pos - avg;
    float dneg = neg - avg;
    return (dpos * dpos + dneg * dneg) / avg;
}

dtree *make_dtree(bitset attr_mask, bitset inst_mask) {
    dtree_leaf m;
    int best_attr, i;
    float best_gain, entropy, g;
    dtree *d, *pos, *neg;
    bitset posmask, negmask;

    d = malloc(sizeof(*d));
    assert(d);

    m = count_instances(inst_mask);
    /* all positive */
    if (m.neg == 0 && m.pos > 0) {
#ifdef DEBUG
	debug("positive leaf");
#endif
	d->val.leaf = m;
	d->tag = DT_POS;
	return d;
    }
    /* all negative */
    if (m.pos == 0 && m.neg > 0) {
#ifdef DEBUG
	debug("negative leaf");
#endif
	d->val.leaf = m;
	d->tag = DT_NEG;
	return d;
    }
    /* corner case: no more attrs */
    if (bs_equal(attr_mask, condition_mask)) {
	debug("mixed leaf (no split)");
	d->val.leaf = m;
	d->tag = DT_MIXED;
	return d;
    }
    /* corner case: no more instances */
    if (m.pos == 0 && m.neg == 0) {
	debug("empty leaf");
	d->tag = DT_EMPTY;
	return d;
    }

#ifdef DEBUG
    debug("considering %d instances", bs_popcount(inst_mask));
#endif
    /* find highest gain attr */

    entropy = instances_entropy(inst_mask);
    best_attr = -1;
    best_gain = -1;

    for (i = 0; i < nconditions; i++) {
	if (bs_isset(attr_mask, i))
	    continue;
	g = gain(entropy, inst_mask, i);
	if (g > best_gain) {
	    best_attr = i;
	    best_gain = g;
	}
    }

    assert(best_attr >= 0);  /* weird gain */
#ifdef DEBUG
    debug("best gain %g for %d", best_gain, best_attr);
#endif

    /* corner case: no more attrs */
    if (best_gain < min_gain) {
#ifdef DEBUG
	debug("mixed leaf (low gain)");
#endif
	d->val.leaf = m;
	d->tag = DT_MIXED;
	return d;
    }

    /* corner case: no more significance */
    if (min_chisquare > 0 &&
	chisquare(m.pos, m.neg) < min_chisquare) {
#ifdef DEBUG
	debug("mixed leaf (low significance)");
#endif
	d->val.leaf = m;
	d->tag = DT_MIXED;
	return d;
    }

    /* build pos and neg masks and recurse */
    bs_lset(attr_mask, best_attr);
#ifdef DEBUG
    debug("build neg subtree");
#endif
    negmask = bs_and(inst_mask, neg_conditions[best_attr]);
    neg = make_dtree(attr_mask, negmask);
    bs_free(negmask);
#ifdef DEBUG
    debug("build pos subtree");
#endif
    posmask = bs_and(inst_mask, pos_conditions[best_attr]);
    pos = make_dtree(attr_mask, posmask);
    bs_free(posmask);
    bs_lclear(attr_mask, best_attr);
#ifdef DEBUG
    debug("finished interior node");
#endif

    d->val.node.attr = best_attr;
    d->val.node.neg = neg;
    d->val.node.pos = pos;
    d->tag = DT_NODE;

    return d;
}


void free_dtree(dtree *d) {
    if (d->tag == DT_NODE) {
	free_dtree(d->val.node.neg);
	free_dtree(d->val.node.pos);
    }
    free(d);
}

void print_dtree(dtree *d, int level) {
    int i;
    printf("%02d", level);
    for (i = 0; i <= level; i++)
	printf("  ");
    switch(d->tag) {
    case DT_POS:
	printf("+ %d\n", d->val.leaf.pos);
	return;
    case DT_NEG:
	printf("- %d\n", d->val.leaf.neg);
	return;
    case DT_MIXED:
	printf("* %d %d\n", d->val.leaf.pos, d->val.leaf.neg);
	return;
    case DT_NODE:
	printf("? %d\n", d->val.node.attr);
	print_dtree(d->val.node.pos, level + 1);
	print_dtree(d->val.node.neg, level + 1);
	return;
    case DT_EMPTY:
	abort();
    }
    abort();
}

dtree_type classify(dtree *d, int inst) {
    switch(d->tag) {
    case DT_POS:
    case DT_NEG:
	return d->tag;
    case DT_MIXED:
	if (d->val.leaf.pos > d->val.leaf.neg)
	    return DT_POS;
	if (d->val.leaf.pos < d->val.leaf.neg)
	    return DT_NEG;
	return DT_MIXED;
    case DT_NODE:
	if (bs_isset(instances[inst].conditions, d->val.node.attr))
	    return classify(d->val.node.pos, inst);
	return classify(d->val.node.neg, inst);
    case DT_EMPTY:
	abort();
    }
    abort();
}

int main(int argc, char **argv) {
    int ambig = 0, false_pos = 0, false_neg = 0;
    int i, ch, seed;
    bitset no_attrs;
    dtree *d;
    struct tms t1, t2;

    seed = time(NULL) ^ getpid();

    while ((ch = getopt_long(argc, argv, options, long_options, 0)) > 0) {
	switch(ch) {
	case 'g':  min_gain = atof(optarg); break;
	case 'c':  min_chisquare = atof(optarg); break;
	case 's':  seed = atoi(optarg); break;
	case 'B':  verbose = 0; benchmark = 1; break;
	default:  fprintf(stderr, "%s\n", usage); exit(1);
	}
    }

    if (verbose)
	printf("seed: %d\n", seed);

    srandom(seed);

    debug("reading instances");
    times(&t1);
    read_instances();
    times(&t2);
    printf("time(read): %ld\n", t2.tms_utime + t2.tms_stime - (t1.tms_utime + t1.tms_stime));
    shuffle_instances();
    ntraining = ninstances - ninstances / 3;

    debug("learning");
    init_masks();
    init_instances();
    init_condition_cache();

    no_attrs = bs_new(nconditions);
    d = make_dtree(no_attrs, instance_mask);

    times(&t1);
    printf("time(train): %ld\n", t1.tms_utime + t1.tms_stime - (t2.tms_utime + t2.tms_stime));

#if 0
    print_dtree(d, 0);
#endif

    debug("classifying");
    for (i = ntraining; i < ninstances; i++) {
	dtree_type c = classify(d, i);
	if (c == DT_POS) {
	    if (instances[i].sign < 0) {
		false_pos++;
		if (verbose || benchmark)
		    printf("mistake: %d %d\n", instances[i].name, 1);
	    }
	} else if (c == DT_NEG) {
	    if (instances[i].sign > 0) {
		false_neg++;
		if (verbose || benchmark)
		    printf("mistake: %d %d\n", instances[i].name, -1);
	    }
	} else if (c == DT_MIXED) {
	    ambig++;
	    if (verbose || benchmark)
		printf("mistake: %d %d\n", instances[i].name, 0);
	} else {
	    abort();  /* bad class */
	}
    }

    times(&t2);
    printf("time(class): %ld\n", t2.tms_utime + t2.tms_stime - (t2.tms_utime + t2.tms_stime));

    printf("%d %d %d %d\n",
	   ninstances - ntraining,
	   ambig, false_pos, false_neg);

    free_dtree(d);

    exit(0);
}
