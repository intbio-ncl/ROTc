#include <list>
#include "Model.hpp"

using namespace std;

#ifndef FEATURE_SELECTION_H
#define FEATURE_SELECTION_H

list<Model*>* exhaustive_search(double** data, int* labels, int feature_no,
                                int sample_no, int depth, double* thresholds,
                                int threshold_size);
void increment_counter(int* counter, int dim, int max);

#endif
