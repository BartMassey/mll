/*
 * Copyright (C) 2002 Bart Massey and Mick Thomure.
 * All Rights Reserved.  See the file COPYING in this directory
 * for licensing information.
 */

#include <stdlib.h>
#include <string.h>

#include "mll.h"
#include "instances.h"

static char *copyn(int nread, char *buf) {
    char *result = malloc(nread);

    assert(result);
    strncpy(result, buf, nread);
    return result;
}

static char *copy(char *buf) {
    return copyn(strlen(buf) + 1, buf);
}

static char *getline(FILE *f) {
    static int nbuf = 512;
    static char *buf = 0;
    int nread = 0;
    int ch;

    if (!buf) {
	buf = malloc(nbuf);
	assert(buf);
    }
    while (1) {
	switch(ch = fgetc(f)) {
	case EOF:
	    if (nread > 0) {
		fprintf(stderr, "unexpected EOF\n");
		buf[nread++] = '\0';
		return copyn(nread, buf);
	    }
	    return 0;
	case '\n':
	    buf[nread++] = '\0';
	    return copyn(nread, buf);
	default:
	    while (nread >= nbuf - 1) {
		nbuf <<= 1;
		buf = realloc(buf, nbuf);
		assert(buf);
	    }
	    buf[nread++] = ch;
	}
    }
    /*NOTREACHED*/
}
	      

static char *next_tok(char **tok) {
    char *s = *tok;
    char *t;
    
    while(*s == ' ' || *s == '\t')
	s++;
    t = s;
    while(*t != ' ' && *t != '\t' && *t != '\0')
	t++;
    if (*t == '\0') {
	if (t == s)
	    return 0;
	*tok = t;
	return s;
    }
    *t = '\0';
    *tok = t + 1;
    return s;
}

/* XXX clobbers buf */
static struct instance *read_buf_instance(char *buf) {
    struct instance *ip = malloc(sizeof(*ip));
    char *s_name, *s_sign, *s_conditions;
    int j;

    assert(ip);
    /* Gather the instance */
    /* name */
    s_name = next_tok(&buf);
    if (!s_name) {
	free(ip);
	return 0;
    }
    ip->name = copy(s_name);
    /* sign */
    s_sign = next_tok(&buf);
    if (!s_sign || sscanf(s_sign, "%d", &ip->sign) != 1) {
	free(ip->name);
	free(ip);
	return 0;
    }
    /* conditions */
    s_conditions = next_tok(&buf);
    if (!s_conditions) {
	free(ip->name);
	free(ip);
	return 0;
    }
    ip->nconditions = strlen(s_conditions);
    ip->conditions = bs_new(ip->nconditions);
    for (j = 0; j < ip->nconditions; j++) {
	switch (s_conditions[j]) {
	case '0': break;
	case '1': bs_lset(ip->conditions, j); break;
	default:
	    free(ip->name);
	    bs_free(ip->conditions);
	    free(ip);
	    return 0;
	}
    }
    if (next_tok(&buf)) {
	free(ip->name);
	bs_free(ip->conditions);
	free(ip);
	return 0;
    }
    return ip;
}


struct instance *read_instance(FILE *f) {
    char *buf = getline(f);

    if (!buf)
	return 0;
    return read_buf_instance(buf);
}

struct instances *read_instances(FILE *f) {
    int i;
    struct instances *iip = malloc(sizeof(*iip));
    struct instance *ip;
    char *buf, *buftmp;
    int ninstances = 0;
    int nconditions = 0;
    int curinstances = 512;

    assert(iip);
    buf = getline(f);
    buftmp = copy(buf);
    ip = read_buf_instance(buftmp);
    free(buftmp);
    if (!ip) {
        if (sscanf(buf, "%d %d", &iip->ninstances, &iip->nconditions) != 2) {
	    free(ip);
	    return 0;
	}
	ninstances = iip->ninstances;
	nconditions = iip->nconditions;
	iip->instances = malloc(ninstances * sizeof(*iip->instances));
	buf = getline(f);
	ip = read_buf_instance(buf);
    } else {
	iip->instances = malloc(curinstances * sizeof(*iip->instances));
    }
    assert(iip->instances);
    i = 0;
    if (!ip) {
	free(iip->instances);
	free(iip);
	return 0;
    }
    if(iip->nconditions > 0) {
	if(iip->nconditions != ip->nconditions) {
	    free(iip->instances);
	    free(iip);
	    return 0;
	}
    } else {
	iip->nconditions = ip->nconditions;
    }
    iip->instances[i++] = ip;

    while((buf = getline(f))) {
	if (i >= curinstances - 1) {
	    curinstances <<= 1;
	    iip->instances = realloc(iip->instances,
				     curinstances * sizeof(*iip->instances));
	    assert(iip->instances);
	}
	ip = read_buf_instance(buf);
	if (!ip || iip->nconditions != ip->nconditions) {
	    free(iip->instances);
	    free(iip);
	    return 0;
	}
	iip->instances[i++] = ip;
    }
    if (iip->ninstances && iip->ninstances != i) {
	    free(iip->instances);
	    free(iip);
	    return 0;
    }
    iip->ninstances = i;
    return iip;
}

