/*
 * Private definitions for neuron learner
 */

#include <math.h>

struct knowledge {
    double *weights,
	   *delta,
	   *best_weights,
	   *averages;
    int nconditions,
	best_score;
};

struct params {
    double rate,
	   clip,
	   sigmoid_k;
    int trials,
	alltrials,
	average,
	use_diff,
	use_sigmoid;
};

/* these functions should be inlined for "speed" */

inline static double sgn(double x) {
    if (x > 0)
        return 1;
    if (x < 0)
        return -1;
    return 0;
}

inline static double sigmoid(double x, double k) {
    double ex = exp(-x * k);
    return (1 - ex) / (1 + ex);
}

inline static double do_clip(double clip, double w) {
    if (clip == 0)
        return w;
    if (w > clip)
        return clip;
    if (w < -clip)
        return -clip;
    return w;
}


double eval_neuron(struct knowledge *, struct instance *, struct params *, int);
