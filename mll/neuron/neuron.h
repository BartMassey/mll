/*
 * Private definitions for neuron learner
 */

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

