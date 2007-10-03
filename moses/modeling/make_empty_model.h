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

#ifndef MAKE_EMPTY_MODEL_H
#define MAKE_EMPTY_MODEL_H

#include "dtree_modeling.h"
#include "ProtoVertex.h"

namespace modeling {
  using alignment::ptree;

  //create a model with minimal dependencies based on a prototype
  class make_empty_model {
  public:
    typedef ptree::pre_order_iterator pre_it;
    typedef ptree::sibling_iterator sib_it;
    typedef alignment::VlocSeq VlocSeq;

    make_empty_model(const ptree& proto,Graph& g) : graph(g) {
      graph=Graph(proto.begin()->vlocs.size());
      graph.reserve(proto.size()*2);
      rec_make(proto.begin(),proto.begin());
      cout << "made with size " << graph.size() << endl;
    }
  private:
    Graph& graph;
    
    void rec_make(pre_it,pre_it,Graph::iterator* it=NULL);

    
  };

} //~namespace modeling

#endif
