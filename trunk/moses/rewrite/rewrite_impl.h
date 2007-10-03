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

#include <functional>
#include <vector>
#include <map>
#include <ext/hash_map>
#include "math_util.h"
#include "TreeComp.h"
#include <boost/bind.hpp>

namespace rewrite {

  //RecRule
  template<typename T>
  void RecRule<T>::operator()(tree<T>& t) const {
    for (sib_it sib=t.begin();sib!=t.end();++sib) {
      operator()(t,sib);
      sib.skip_children();
    }
  }
  template<typename T>
  void RecRule<T>::operator()(tree<T>& t,sib_it it) const {
    for (sib_it sib=t.begin(it);sib!=t.end(it);++sib)
      operator()(t,sib);
    
    //t.validate(); //DEBUG
    //cout << "before " << typeid(*this).name() << " " << t 
    //<< " | " << tree<T>(it) << endl;
    visit(t,it);
    //cout << "after: " << t << endl;
    //t.validate();
  }

  //Level
  template<typename T>
  void Level<T>::visit(tree<T>& t,sib_it it) const {
    if (is_associative(*it)) {
      for (sib_it sib=t.begin(it);sib!=t.end(it);)
	if (*sib==*it) {
	  t.flatten(sib);
	  sib=t.erase(sib);
	} else
	  ++sib;
    }
  }

  //MergeConstants
  template<typename T>
  void MergeConstants<T>::visit(tree<T>& t,sib_it it) const {
    if (it.number_of_children()==0)
      return;
    sib_it to;
    if (!is_commutative(*it)) {
      for (sib_it sib=t.begin(it);sib!=t.end(it);++sib)
	if (!is_constant(*sib))
	  return;
      to=t.end(it);
    } else {
      //needs to be deep, so can't call std::partition
      to=t.partition(t.begin(it),t.end(it),is_constant<T>);
      if (distance(t.begin(it),to)<2)
	return;
    }
    /**
    T res=apply(*it,t.begin(it),to);
    if (to==t.end(it)) { //evaled whole subtree
      t.replace(it,res);
      t.erase_children(it);
    } else {
      t.begin(it)=t.replace(t.begin(it),res);
      for (sib_it sib=++t.begin(it);sib!=to;)
	sib=t.erase(sib);
    }  
    **/
  }    
  
  //Exapand
  template<typename T>
  void Expand<T>::visit(tree<T>& t,sib_it it) const {
    if (is_commut(*it)) {
      for (sib_it sib=t.begin(it);sib!=t.end(it);++sib) {
	if (is_distrib(*it,*sib)) {
	  sib=t.move_before(t.begin(it),sib);
	  sib_it second=++t.begin(it);
	  for (sib_it sib1=t.begin(sib);sib1!=t.end(sib);++sib1) {
	    sib1=t.insert_above(sib1,*it);
	    t.append_children(sib1,second,t.end(it));
	  }
	  for (sib_it op=second;op!=t.end(it);op=t.erase(op))
	    ;
	  sib=t.flatten(sib);
	  it=t.replace(it,*sib);
	  t.erase(sib);
	  visit(t,it);
	  return;
	}
      }
    }
  }

  //CanonicalOrder
  template<typename T>
  void CanonicalOrder<T>::visit(tree<T>& t,sib_it it) const {
    if (is_commutative(*it))
      t.sort_tree_nodes(t.begin(it),t.end(it),trees::TreeComp<T>(t));
  }

} //~namespace rewrite
