#include <stdio.h>
#include <stdlib.h>
#include <R.h>
#include <Rinternals.h>
#include <Rdefines.h>
#include <Rmath.h>
#include <algorithm>
#include "Feature.hpp"
#include "Feature_Selection.hpp"
#include "Model.hpp"

extern "C" SEXP extend_model()
{
  return R_NilValue;
}

extern "C" SEXP cross_validate()
{
  return R_NilValue;
}

extern "C" SEXP search(SEXP data_Matrix, SEXP labels_Vector,
                       SEXP depth_INT, SEXP thresholds_Vector,
                       SEXP order_INT)
{
  SEXP Rdim_INT;
  PROTECT(Rdim_INT = getAttrib(data_Matrix, R_DimSymbol));
  int rows = INTEGER(Rdim_INT)[0];
  int columns = INTEGER(Rdim_INT)[1];
  printf("%i, %i\n", rows, columns);

  UNPROTECT(1);

  int length = length(thresholds_Vector);
  printf("%i\n", length);

  int depth = INTEGER(depth_INT)[0];

  data_Matrix = coerceVector(data_Matrix, REALSXP);

  double** call_Matrix = (double**) malloc(rows*sizeof(double*));
  for (int i = 0; i < rows; i++)
  {
    call_Matrix[i] = (double*) malloc(columns*sizeof(double));
  }

  for (int i = 0; i < rows; i++)
  {
    for (int j = 0; j < columns; j++)
    {
      call_Matrix[i][j] = REAL(data_Matrix)[i + j*rows];
    }
  }

  list<Model*>* result = exhaustive_search(call_Matrix, INTEGER(labels_Vector),
                                           rows, columns, depth,
                                           REAL(thresholds_Vector), length);


  Model** model_array = new Model*[result->size()];

  int i = 0;
  for (list<Model*>::iterator it = result->begin(); it != result->end(); it++)
  {
    model_array[i++] = *it;
  }


  if(INTEGER(order_INT)[0])
  {
    sort(model_array, model_array + result->size(), model_comparator_asc);
  }

  SEXP return_Matrix;

  PROTECT(return_Matrix = allocMatrix(REALSXP, result->size(), (3*depth)));
  int k = 0;
  for (int j = 0; j < depth; j++)
  {
    for (int i = 0; i < (int) result->size(); i++)
    {
      REAL(return_Matrix)[k++] = model_array[i]->get_feature_ids()[j];
    }
  }
  for (int j = 0; j < depth; j++)
  {
    for (int i = 0; i < (int) result->size(); i++)
    {
      REAL(return_Matrix)[k++] = model_array[i]->get_margins()[j];
    }
  }
  for (int j = 0; j < depth; j++)
  {
    for (int i = 0; i < (int) result->size(); i++)
    {
      REAL(return_Matrix)[k++] = model_array[i]->get_thresholds()[j];
    }
  }

  for (int i = 0; i < (int) result->size(); i++)
  {
    delete model_array[i];
  }

  free(result);
  free(model_array);
  for (int i = 0; i < rows; i++)
  {
    free(call_Matrix[i]);
  }
  free(call_Matrix);


  UNPROTECT(1);
  return return_Matrix;
}
