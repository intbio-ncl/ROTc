#include "Feature.hpp"
#include "Util.hpp"
#include <float.h>
#include <algorithm>
#include <string>
#include <iostream>

using namespace std;

Feature::Feature(double* data, int* labels, int sample_no, int id, string name)
{
  this->name = name;
  this->id = id;

  // Find number of negative datapoints
  // Find minimum positive datapoint
  neg_no = 0;
  min_pos = DBL_MAX;
  max_pos = -DBL_MAX;
  for (int i = 0; i < sample_no; i++)
  {
    if(labels[i])
    {
      if(min_pos > data[i])
      {
        min_pos = data[i];
      }
      if(max_pos < data[i])
      {
        max_pos = data[i];
      }
    }
    else
    {
      neg_no++;
    }
  }

  // Find number of 64bits needed to represent all negative data
  int container_size = 8*sizeof(bit_container);
  is_above_below_no = (neg_no + container_size - 1) / container_size;
  is_above = new bit_container[is_above_below_no];
  is_below = new bit_container[is_above_below_no];
  for (int i = 0; i < is_above_below_no; i++)
  {
    is_above[i] = 0;
    is_below[i] = 0;
  }

  // Move positve and negative datapoints to arrays
  // Set up sort_index array for sorting
  // Find points which lie above min_pos
  pos_no = sample_no - neg_no;
  pos_data = new double[pos_no];
  neg_data_distance_min = new double[neg_no];
  inverted_neg_data_distance_max = new double[neg_no];
  sort_index sort_data[neg_no];
  int neg_position = 0;
  int pos_position = 0;
  for (int i = 0; i < sample_no; i++)
  {
    if(!labels[i])
    {
      if(data[i] >= min_pos)
      {
        is_above[neg_position / container_size] += ((bit_container) 1) << (neg_position % container_size);
      }
      if (data[i] <= max_pos)
      {
        is_below[neg_position / container_size] += ((bit_container) 1) << (neg_position % container_size);
      }

      sort_data[neg_position] = {data[i], neg_position};
      neg_data_distance_min[neg_position] = min_pos - data[i];
      inverted_neg_data_distance_max[neg_position++] = data[i] - max_pos;
    }
    else
    {
      pos_data[pos_position++] = data[i];
    }
  }

  // Provide sorted indices into array
  descending_indices = new int[neg_no];
  ascending_indices = new int[neg_no];
  sort(sort_data, sort_data + neg_no, index_comparator_dec);
  for (int i = 0; i < neg_no; i++)
  {
    descending_indices[i] = sort_data[i].second;
    ascending_indices[(neg_no - i) - 1] = sort_data[i].second;
  }
}

Feature::~Feature()
{
  free(pos_data);
  free(neg_data_distance_min);
  free(inverted_neg_data_distance_max);

  free(is_below);
  free(is_above);

  free(ascending_indices);
  free(descending_indices);
}


double* Feature::get_distances(int orientation) const
{
  if (orientation)
  {
    return inverted_neg_data_distance_max;
  }
  return neg_data_distance_min;
}

int Feature::get_data_size() const
{
  return neg_no;
}

double Feature::get_bound(int orientation) const
{
  if (orientation)
  {
    return -max_pos;
  }
  return min_pos;
}

int* Feature::get_index(int orientation) const
{
  if (orientation)
  {
    return ascending_indices;
  }
  return descending_indices;
}

bit_container* Feature::get_misclassified(int orientation) const
{
  if (orientation)
  {
    return is_below;
  }
  return is_above;
}

int Feature::get_misclassified_size() const 
{
  return is_above_below_no;
}
