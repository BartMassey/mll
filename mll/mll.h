#include "config.h"
#include <assert.h>
#include <stdio.h>

struct instances;
struct instance;
struct knowledge;
struct params;

typedef struct knowledge *learn_t(struct instances *, struct params *);
typedef int classify_t(struct knowledge *, struct instance *, struct params *);
typedef struct knowledge *read_t(FILE *, struct params *);
typedef int write_t(FILE *, struct knowledge *);
typedef void kfree_t(struct knowledge *);
typedef struct params* parseargs_t(int argc, char **argv);

extern learn_t learn_nbayes;
extern classify_t classify_nbayes;
extern read_t read_nbayes;
extern write_t write_nbayes;
extern kfree_t kfree_nbayes;
extern parseargs_t parseargs_nbayes;

extern learn_t learn_neuron;
extern classify_t classify_neuron;
extern read_t read_neuron;
extern write_t write_neuron;
extern kfree_t kfree_neuron;
extern parseargs_t parseargs_neuron;

extern learn_t learn_hamming;
extern classify_t classify_hamming;
extern read_t read_hamming;
extern write_t write_hamming;
extern kfree_t kfree_hamming;
extern parseargs_t parseargs_hamming;

extern learn_t learn_dtree;
extern classify_t classify_dtree;
extern read_t read_dtree;
extern write_t write_dtree;
extern kfree_t kfree_dtree;
extern parseargs_t parseargs_dtree;
