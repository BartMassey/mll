#include "config.h"
#include <assert.h>
#include <stdio.h>

struct instances;
struct instance;
struct knowledge;

typedef struct knowledge *learn_t(struct instances *);
typedef int classify_t(struct knowledge *, struct instance *);
typedef struct knowledge *read_t(FILE *);
typedef int write_t(FILE *, struct knowledge *);
typedef void kfree_t(struct knowledge *);

extern learn_t learn_nbayes;
extern classify_t classify_nbayes;
extern read_t read_nbayes;
extern write_t write_nbayes;
extern kfree_t kfree_nbayes;
