/*
 * Private definitions for neuron learner
 */

struct knowledge {
    double *weights;
    int nconditions;
};

struct params {
    double rate,
	   clip,
	   sigmoid_k;
    int trials,
	alltrials,
	average,
	use_diff;
};

