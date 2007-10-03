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

#include "dtree.h"

namespace modeling { int hack_arity=3; }

std::ostream& operator<<(std::ostream& out,const modeling::Node& n) {
  if (const modeling::Leaf* l=boost::get<modeling::Leaf>(&n)) {
    out << "{ ";
    for (modeling::perm_iterator it=l->first;it!=l->second;++it)
      out << *it << " ";
    return (out << "}");
  } else {
    return (out << "[]");
  }
}
