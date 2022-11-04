#include "Model.hpp"
#include "Feature.hpp"
#include <list>
#include <float.h>
#include <iostream>
#include <algorithm>

using namespace std;

Model::Model(Feature** features, int dim)
{
  separating = true;
  for (int i = 0; i < features[0]->get_misclassified_size(); i++)
  {
    bit_container check = features[0]->get_misclassified(0)[i];
    for (int j = 1; j < dim; j++)
    {
      check &= features[j]->get_misclassified(0)[i];
    }
    if (check)
    {
      separating = false;
      return;
    }
  }

  this->dim           = dim;
  this->neg_sample_no = features[0]->get_data_size();
  this->thresholds    = new double[dim];
  this->margins       = new double[dim];
  this->sorted_margins = new double[dim];
  this->feature_ids   = new int[dim];
  this->features      = (Feature**) malloc(dim*sizeof(Feature*));
  for (int i = 0; i < dim; i++)
  {
    this->features[i]    = features[i];
    this->feature_ids[i] = features[i]->id;
  }

}

Model::~Model()
{
  if (separating)
  {
    delete[] this->feature_ids;
    delete[] thresholds;
    delete[] margins;
    free(features);
  }

}

void Model::train()
{
  if (!separating)
  {
    return;
  }
  // Solve 1 dimensional problem immediately
  if(dim == 1)
  {
    margins[0]      = (features[0]->get_distances(0))[features[0]->get_index(0)[0]];
    sorted_margins[0] = margins[0];
    thresholds[0]   = features[0]->get_bound(0) - 0.5*(margins[0]);
    return;
  }

  // Initialize list of dimensions
  // Initialize data
  // Initialize index
  // Initialize max_index
  list<int> dim_list;
  int* index[dim];
  double* distances[dim];
  int closest_index[dim];
  for (int i = 0; i < dim; i++)
  {
    dim_list.push_back(i);
    distances[i] = features[i]->get_distances(0);
    index[i] = features[i]->get_index(0);
    closest_index[i]   = 0;
  }

  // Find maximum distance dimension for each point
  int furthest_dim[neg_sample_no];
  for (int i = 0; i < neg_sample_no; i++)
  {
    furthest_dim[i] = 0;
    double max_dist = distances[0][i];
    for (int j = 1; j < dim; j++)
    {
      double comp_dist = distances[j][i];

      if (comp_dist > max_dist)
      {
        furthest_dim[i] = j;
        max_dist = comp_dist;
      }
    }
  }


  // For each dimension find the closest point in the set of points with
  // maximum distance in that dimension.
  // Find the closest of these points to the positive boundary and set that as a
  // negative boundary then trim every point it classifies and remove that dimension.
  // Do this until all dimensions have been removed.
  while (dim_list.size() > 0)
  {
    int closest_dimension = -1;
    double min_distance  = DBL_MAX;
    list<int>::iterator closest_iter;
    for (list<int>::iterator dimension = dim_list.begin(); dimension != dim_list.end();)
    {
      bool broke = false;
      for (int i = closest_index[*dimension]; i < neg_sample_no; i++)
      {
        if (furthest_dim[index[*dimension][i]] == *dimension)
        {
          closest_index[*dimension] = i;
          if (distances[*dimension][index[*dimension][i]] < min_distance)
          {

            closest_iter      = dimension;
            closest_dimension = *dimension;
            min_distance  = distances[*dimension][index[*dimension][i]];
          }
          dimension++;
          broke = true;
          break;
        }
      }
      // If a dimension has no remaining points then remove it
      if(!broke)
      {
        closest_index[*dimension] = -1;
        dimension = dim_list.erase(dimension);
      }
    }

    if (dim_list.size() == 0)
    {
      break;
    }

    // Remove the dimension containing the highest of the high points
    // Remove every point below the highest of the high points
    dim_list.erase(closest_iter);
    for (int i = closest_index[closest_dimension]; i < neg_sample_no; i++)
    {
      furthest_dim[index[closest_dimension][i]] = -1;
    }
  }


  for (int i = 0; i < dim; i++)
  {
    if(closest_index[i] >= 0)
    {
      margins[i] = distances[i][index[i][closest_index[i]]];
      sorted_margins[i] = margins[i];
      thresholds[i] = features[i]->get_bound(0) - 0.5*margins[i];
    }
    else
    {
      thresholds[i] = -DBL_MAX;
      margins[i] = DBL_MAX;
      sorted_margins[i] = DBL_MAX;
    }
  }

  sort(sorted_margins, sorted_margins + dim);
  return;
}

int Model::test(double* datapoint)
{
  for (int i = 0; i < dim; i++)
  {
    if(datapoint[i] < thresholds[i])
    {
      return 0;
    }
  }
  return 1;
}

double* Model::get_thresholds() const
{
  return this->thresholds;
}

double* Model::get_margins() const
{
  return this->margins;
}

double* Model::get_sorted_margins() const
{
  return this->sorted_margins;
}

int* Model::get_feature_ids() const
{
  return this->feature_ids;
}

int Model::get_dim() const
{
  return this->dim;
}

bool model_comparator_asc (const Model* p1, const Model* p2)
{
  int dim = p1->get_dim();
  int dim2 = p2->get_dim();
  if(dim2 < dim)
  {
    dim = dim2;
  }

  double* s_margins1 = p1->get_sorted_margins();
  double* s_margins2 = p2->get_sorted_margins();

  for (int i = 0; i < dim; i++)
  {
    if(s_margins1[i] > s_margins2[i])
    {
      return true;
    }
    else if (s_margins1[i] < s_margins2[i])
    {
      return false;
    }
  }

  return dim != dim2;
}
