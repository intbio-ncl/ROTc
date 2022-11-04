#include "Util.hpp"


bool index_comparator_asc (const sort_index& p1, const sort_index& p2)
{
  return p1.first < p2.first;
}

bool index_comparator_dec (const sort_index& p1, const sort_index& p2)
{
  return p1.first > p2.first;
}
