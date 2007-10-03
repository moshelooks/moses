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

#ifndef DTREE_MODELING_H
#define DTREE_MODELING_H

#include "dtree.h"
#include "Dependance.h"
#include <boost/iterator/permutation_iterator.hpp>
#include <list>

//#include "ProtoVertex.h"

namespace modeling {
  //using alignment::ptree;

  typedef vector<contin_t>::const_iterator ContinIt;
  typedef vector<disc_t>::const_iterator DiscIt;
  typedef vector<bool>::const_iterator BoolIt;
  typedef DTree<ContinIt> ContinDTree;
  typedef DTree<DiscIt> DiscDTree;
  typedef DTree<BoolIt> BoolDTree;
  typedef variant<ContinDTree,DiscDTree,BoolDTree> Element;

  struct Graph : public vector<Element> {
    template<typename It>
    explicit Graph(int n1,It from,It to) : vector<Element>(from,to),n(n1) { }
    explicit Graph(int n1=0) : n(n1) { }
    template<typename Out>
    void order(Out out) { 
      iota(out,out+size(),0);
      _dep.destructiveOrder(out,out+size());
    }
    void order() {
      _ordering.resize(size());
      order(_ordering.begin());
    }
    const vector<int>& ordering() const { return _ordering; }
    bool operator()(int a,int b) const { return _dep(a,b); }
    void dependance(int a,int b) { _dep.addDep(a,b); }
    void structure(int a,int b) { 
      dependance(a,b);
      _structure[a].insert(b); 
    }
    Dependance<int>& dep() { return _dep; }

    BoolMarginal& create_bool_marginal() { 
      _bms.push_back(BoolMarginal(n));
      return _bms.back();
    }
    DiscMarginal& create_disc_marginal() {
      _dms.push_back(DiscMarginal(n));
      return _dms.back();
    }

    int n; //length of marginals
  private:
    Dependance<int> _dep;
    list<BoolMarginal> _bms; //list so that things don't get realloc'ed
    list<DiscMarginal> _dms; //list so that things don't get realloc'ed
    hash_map<int,hash_set<int> > _structure;
    vector<int> _ordering;
  };

  template<typename It>
  void generate_assignment(It from,It to,const Graph& graph,Assignment& ass) {
    transform(make_permutation_iterator(graph.begin(),from),
	      make_permutation_iterator(graph.begin(),to),
	      make_permutation_iterator(ass.begin(),from),
	      bind(sample<Element>,ref(ass),_1));
  }
      
} //~namespace modeling

#endif
