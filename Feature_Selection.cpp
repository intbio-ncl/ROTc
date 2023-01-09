#include <iostream>
#include <float.h>
#include <list>
#include "Feature.hpp"
#include "Feature_Selection.hpp"
#include "Model.hpp"

using namespace std;

list<Model*>* exhaustive_search(double** data, int* labels, int feature_no,
                                int sample_no, int depth, double* thresholds,
                                int threshold_size)
{
  Feature* features[feature_no];

  for (int i = 0; i < feature_no; i++)
  {
    features[i] = new Feature(data[i], labels, sample_no, i + 1,"");
  }

  int row = 0;
  Feature* train[depth];
  int combination[depth];
  list<Model*>* separators = new list<Model*>();
  for (int i = 0; i < depth; i++)
  {
    combination[(depth - i) - 1] = i;
  }

  while (combination[0] < feature_no)
  {

    if (combination[depth - 1] == row)
    {
      std::cout << "Row: " << row++ << '\n';
      std::cout << separators->size() << '\n';
    }

    for (int i = 0; i < depth; i++)
    {
      train[i] = features[combination[i]];
      //std::cout << combination[i] << '\n';
    }


    Model* m = new Model(train, depth);

    if (m->separating)
    {
      m->train();
      for (int i = 0; i < threshold_size; i++)
      {
        // this may be a useful place to implement a lower limit for thresholds
        // if threshold < 1, delete the model (very small thresholds aren't useful as biomarkers)
        // ## DAVID edit suggestion ##
        if (m->get_sorted_margins()[i] < 1)  // recommend using a variable instead of 1
        {
          delete m;
          break;
        } 
        // also include a catch in the case of the margin equalling DBL_MAX
        else if (m->get_sorted_margins()[i] == DBL_MAX)
        {
          delete m;
          break;
        }
        // end of edit
        else if (m->get_sorted_margins()[i] == thresholds[i])
        {
          continue;
        }
        else if (m->get_sorted_margins()[i] > thresholds[i])
        {
          separators->push_back(m);
          break;
        }
        else if (m->get_sorted_margins()[i] < thresholds[i])
        {
          delete m;
          break;
        }
        
      }
    }
    else
    {
      delete m;
    }



    increment_counter(combination, depth, feature_no);

  }

  return separators;
}

void increment_counter(int* counter, int dim, int max)
{
  for (int i = 0; i < dim; i++)
  {
    counter[i]++;
    if (counter[i] < max - i)
    {
      for (int j = i - 1; j >= 0; j--)
      {
        counter[j] = counter[j + 1] + 1;
      }
      break;
    }
  }
}
