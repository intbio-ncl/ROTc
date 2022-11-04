#ifndef FEATURE_H
#define FEATURE_H

#include <string>
#include "Util.hpp"

typedef unsigned long long bit_container;

class Feature
{
  public:
    Feature(double* data, int* labels, int sample_no, int id, std::string name);
    ~Feature();
    void print();

    double* get_distances(int orientation) const;
    int get_data_size() const;

    double get_bound(int orientation) const;

    int* get_index(int orientation) const;

    bit_container* get_misclassified(int orientation) const;
    int get_misclassified_size() const;
    int id;
  private:
    std::string name;

    double max_pos;    // Maximum positive datapoint
    double min_pos;    // Minimum positive datapoint
    double* pos_data;  // Positive datapoints
    int pos_no;        // Number of positive datapoints

    double* neg_data_distance_min;           // Negative datapoints distance from min_pos
    double* inverted_neg_data_distance_max;  // Inverted negative datapoints from max_pos
    int* descending_indices;                 // Provides a descending sorted index into the negative_data
    int* ascending_indices;                  // Provides an ascending sorted index into the negative_data
    int neg_no;                              // Number of negative datapoints

    bit_container* is_above;  // 64bits representing which datapoints lie above min_pos
    bit_container* is_below;  // 64bits representing which datapoints lie below max_pos
    int is_above_below_no;    // number of 64bits required to represent all negative data
};

#endif
