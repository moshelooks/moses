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

#ifndef _EDA_USING_H
#define _EDA_USING_H

#include <boost/bind.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/iterator/permutation_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/utility/result_of.hpp>
#include <boost/next_prior.hpp>
#include <boost/type_traits.hpp>
#include "util/functional.h"
#include "util/numeric.h"
#include <vector>
#include <algorithm>
#include <utility>

/// anything that gets imported into the eda namespace with a using
/// directive should go here
namespace eda {
  using boost::bind;
  using boost::ref;
  using boost::make_counting_iterator;
  using boost::make_transform_iterator;
  using boost::make_permutation_iterator;
  using boost::make_indirect_iterator;
  using boost::result_of;
  using boost::next;
  using boost::prior;

  using util::begin_generator;
  using util::end_generator;
  using util::make_transform_output_iterator;

  using util::integer_log2;

  using util::nullary_function;  
  using std::unary_function;
  using std::binary_function;

  using std::vector;

  using std::distance;
  using std::copy;
  using std::transform;
  using std::nth_element;
  using std::accumulate;
  using std::adjacent_find;

  using std::pair;
  using std::make_pair;

  using util::select1st;
  using util::select2nd;
}

#endif
