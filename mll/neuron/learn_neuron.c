#include <stdlib.h>

#include "../mll.h"
#include "../instances.h"
#include "neuron.h"


double ddx_sigmoid(double x, double k) {
    double ex = exp(-x * k);
    double den = 1 + ex;
    den *= den;
    return 2 * k * ex / den;
}

/*
 * Returns one of -0.5, -0.4 , ... , -0.1, 0.1, 0.2, ..., 0.5
 * selected via a PRNG.
 */
double small_rand(void) {
    long v = ((unsigned long) random()) % 10 - 5;
    assert(v >= -5 && v <= 4);
    if (v < 0)
        return v / (double) 10;
    return (v + 1) / (double) 10;
}

double score_weights(struct knowledge *k,
		     struct instances *iip,
		     struct params *p) {
    int i;
    enum dataset { TRAINING, VALIDATION };
    enum category { GOOD, FALSE_POSITIVE, FALSE_NEGATIVE };
    int count[2][3] = {{0,0,0},{0,0,0}};
    enum dataset dataset;
    enum category category;
    int validerr;

    int ntraining = iip->ninstances / 3;

    // Score against validation set (middle of input set)
    for (i = 0; i < iip->ninstances; i++) {
        double v = eval_neuron(k, iip->instances[i], p, 0);

        if (i < ntraining)
            dataset = TRAINING;
        else
            dataset = VALIDATION;

        if (v > 0 && iip->instances[i]->sign < 0)
            category = FALSE_POSITIVE;
        else if (v < 0 && iip->instances[i]->sign > 0)
            category = FALSE_NEGATIVE;
        else if (v == 0)
            abort();
        else
            category = GOOD;
        count[dataset][category]++;
    }

    if (count[VALIDATION][GOOD] > k->best_score) {
        for (i = 0; i < iip->nconditions + 1; i++)
            k->best_weights[i] = k->weights[i];
        k->best_score = count[VALIDATION][GOOD];
    }

    validerr = count[VALIDATION][FALSE_POSITIVE] +
               count[VALIDATION][FALSE_NEGATIVE];
    return (double) validerr / (validerr + count[VALIDATION][GOOD]);
}


struct knowledge *init_knowledge(struct instances *ip,
				 struct params *p) {
    int i;
    struct knowledge *k = malloc(sizeof(*k));

    assert(k);
    k->weights = malloc((ip->nconditions + 1) * sizeof(*k->weights));
    assert(k->weights);
    k->nconditions = ip->nconditions;

    for (i = 0; i < k->nconditions + 1; i++)
        k->weights[i] = small_rand() / 100;

    k->delta = malloc((k->nconditions + 1) * sizeof(*k->delta));
    assert(k->delta);
    k->best_weights = malloc((k->nconditions + 1) * sizeof(*k->best_weights));
    k->best_score = -1;

    if (p->average) {
        k->averages = malloc(k->nconditions * sizeof(*k->averages));
        assert(k->averages);
        for (i = 0; i < k->nconditions; i++) {
            int j;
            double t = 0;

            for (j = 0; j < ip->ninstances; j++)
                if (bs_isset(ip->instances[j]->conditions, i))
                    t += 1.0;
            k->averages[i] = t / ip->ninstances;
        }

    } else {
	// make sure averages is null so kfree() knows not to free it
	k->averages = NULL;
    }

    return k;
}

void train(struct knowledge *k,
	   struct instances *iip,
	   struct params *p) {
    int i, j;
    double v, wf;

    int ntraining = iip->ninstances / 3;

    for (i = 0; i < k->nconditions + 1; i++)
        k->delta[i] = 0;

    for (i = 0; i < ntraining; i++) {
        v = eval_neuron(k, iip->instances[i], p, p->use_diff);

        if (p->use_sigmoid) {
            wf = p->rate * (iip->instances[i]->sign - v) * ddx_sigmoid(v, p->sigmoid_k);
        } else {
            wf = p->rate * (iip->instances[i]->sign - v);
        }

        for (j = 0; j < k->nconditions; j++)
            if (bs_isset(iip->instances[i]->conditions, j))
                k->delta[j] += wf;
        k->delta[k->nconditions] += wf;
    }
    for (i = 0; i < k->nconditions; i++) {
        k->weights[i] = do_clip(p->clip, k->weights[i] + k->delta[i]);
    }
    k->weights[k->nconditions] =
      do_clip(p->clip, k->weights[k->nconditions] + k->delta[k->nconditions]);
}


struct knowledge *learn_neuron(struct instances *ip,
			       struct params *p) {
    double frac = 1.0;
    int i;

    // no need to check k, program will exit on error
    struct knowledge *k = init_knowledge(ip, p);

    frac = score_weights(k, ip, p);
    for (i = 0; i < p->trials; i++) {
        train(k, ip, p);
        frac = score_weights(k, ip, p);
        if (!p->alltrials && frac < 0.005)
            break;
    }

    return k;
}
