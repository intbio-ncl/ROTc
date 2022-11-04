#include <utility>

#ifndef UTIL_H
#define UTIL_H

typedef std::pair<double,int> sort_index;
bool index_comparator_asc (const sort_index& p1, const sort_index& p2);
bool index_comparator_dec (const sort_index& p1, const sort_index& p2);

#endif
