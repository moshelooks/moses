/****
   Copyright 2005-2007, Moshe Looks and Novamente LLC

   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.
****/

#ifndef _MOSES_USING_H
#define _MOSES_USING_H

#include "combo/vertex.h"
#include <boost/logic/tribool.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/permutation_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <set>
#include <functional>
#include <vector>

namespace moses {

  using namespace combo;

  using std::min;
  using std::max;
  using std::accumulate;
  using std::set;
  using std::unary_function;
  using std::make_pair;
  using std::vector;
  using std::advance;

  using boost::logic::tribool;
  using boost::logic::indeterminate;
  using boost::bind;
  using boost::make_counting_iterator;
  using boost::make_transform_iterator;
  using boost::make_permutation_iterator;
  using boost::make_indirect_iterator;

} //~namespace moses

#endif
