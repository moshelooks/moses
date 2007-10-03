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

#ifndef _COMBO_USING_H
#define _COMBO_USING_H

#include <boost/variant.hpp>
#include <boost/bind.hpp>
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/assign/list_of.hpp>
#include <boost/multi_array.hpp>

#include <functional>
#include <algorithm>

/// anything that gets imported into the combo namespace with a using
/// directive should go here
namespace combo {
  using boost::variant;
  using boost::static_visitor;
  using boost::bind;
  using boost::make_counting_iterator;
  using boost::make_indirect_iterator;
  using boost::make_transform_iterator;
  using boost::apply_visitor;
  using boost::assign::list_of;
  using boost::multi_array;
  using std::find_if;
  using std::accumulate;
}

#endif
