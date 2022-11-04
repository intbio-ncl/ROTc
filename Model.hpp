#ifndef MODEL_H
#define MODEL_H
#include "Feature.hpp"

class Model
{
  public:
    Model(Feature** features, int dim);
    ~Model();
    void train();
    int test(double* datapoint);

    double* get_thresholds() const;
    double* get_margins() const;
    double* get_sorted_margins() const;
    int     get_dim() const;
    int*    get_feature_ids() const;
    bool separating;
  private:
    int dim;
    int neg_sample_no;


    int* feature_ids;

    Feature** features;
    double* thresholds;
    double* margins;
    double* sorted_margins;

};

bool model_comparator_asc (const Model* p1, const Model* p2);

#endif
