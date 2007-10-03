/****
   Copyright 2005-2007, Moshe Looks

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

#ifndef MODELING_H
#define MODELING_H

#include "tree.h"
#include <boost/variant.hpp>
#include <boost/functional/hash.hpp>
#include <vector>
#include "basic_types.h"

namespace modeling {
  using namespace std;
  using namespace boost;

  typedef vector<contin_t> ContinMarginal;
  typedef vector<disc_t> DiscMarginal;
  typedef vector<bool> BoolMarginal;
  typedef variant<ContinMarginal,DiscMarginal,BoolMarginal> Marginal;
  typedef tree<Marginal> MargTree;
  typedef variant<contin_t,disc_t,bool> Value;
  typedef vector<Value> Assignment;

  inline bool tobool(const Value& v) { return boost::get<bool>(v); }
  inline disc_t todisc(const Value& v) { return boost::get<disc_t>(v); }
  inline contin_t tocontin(const Value& v) { return boost::get<contin_t>(v); }

  typedef ContinMarginal::const_iterator cmarg_it;
  typedef DiscMarginal::const_iterator   dmarg_it;
  typedef BoolMarginal::const_iterator   bmarg_it;

} //~namespace modeling

namespace boost {
  inline std::size_t hash_value(const modeling::Value& v) {
    if (const bool* b=boost::get<bool>(&v))
      return boost::hash_value(*b);
    else if (const disc_t* d=boost::get<disc_t>(&v))
      return boost::hash_value(*d);
    else
      return boost::hash_value(boost::get<contin_t>(v));
  }
}

#endif



