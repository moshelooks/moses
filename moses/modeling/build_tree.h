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

#ifndef BUILD_TREE_H
#define BUILD_TREE_H

#include "ProtoVertex.h"
#include "modeling.h"

namespace modeling {
  using alignment::ptree;

  class build_tree {
  public:
    typedef ptree::pre_order_iterator ppre_it;
    typedef ptree::sibling_iterator psib_it;
    typedef vtree::sibling_iterator vpre_it;
    typedef Assignment::const_iterator ass_it;

    build_tree(const ptree& proto,const Assignment& a,vtree& d) : dst_tr(d) {
      dst_tr.clear();
      dst_tr.set_head(Vertex());
      ass_it ass=a.begin();
      rec_build(proto.begin(),proto.begin(),ass,dst_tr.begin());
    }
  private:
    vtree& dst_tr;

    void rec_build(ppre_it src,ppre_it sup,ass_it& ass,vpre_it dst);
    void skip_subvariables(ppre_it src,ppre_it sup,ass_it& ass);
  };

} //~namespace modeling;

#endif
