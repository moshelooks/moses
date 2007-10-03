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

#include "enf.h"
#include <boost/iterator/transform_iterator.hpp>

namespace rewrite {
  namespace enf {
    using namespace std;
    using namespace boost;
    using namespace id;

    bool consistent(const Set& s) {
      Set::const_iterator pos=find_if(s.begin(),s.end(),
				      bind(greater<Constraint>(),_1,0));
      return has_empty_intersection
	(make_transform_iterator(Set::reverse_iterator(pos),
				 negate<Constraint>()),
	 make_transform_iterator(s.rend(),negate<Constraint>()),pos,s.end());
    }

    bool reduce_to_elegance::and_cut(sib_it child) {
      bool adopted=false;
      for (sib_it gchild=child.begin();gchild!=child.end();)
	if (gchild.number_of_children()==1 && guard_set(*gchild).empty()) {
	  _tr.erase(_tr.flatten(gchild.begin()));
	  if (!adopted) //is child adopting a terminal 1-constrant AND, x?
	    for (sib_it x=gchild.begin();x!=gchild.end();++x)
	      if (x.begin()==x.end() && guard_set(*x).size()==1) {
		adopted=true;
		break;
	      }
	  gchild=_tr.erase(_tr.flatten(gchild));
	} else {
	  ++gchild;
	}
      return adopted;
    }
    void reduce_to_elegance::or_cut(sib_it current) {
      for (sib_it child=current.begin();child!=current.end();)
	if (child.number_of_children()==1) {
	  union2(guard_set(*current),guard_set(*child.begin()));
	  _tr.erase(_tr.flatten(child.begin()));
	  child=_tr.erase(_tr.flatten(child));
	} else {
	  ++child;
	}
    }

    Result reduce_to_elegance::reduce(pre_it current,
				      const Set& dominant,const Set& command) {
      if (is_and(*current)) {
	difference2(guard_set(*current),dominant);

	set_difference
	  (guard_set(*current),
	   make_transform_iterator(command.rbegin(),negate<Constraint>()),
	   make_transform_iterator(command.rend(),negate<Constraint>()));
	
	if (current.begin()==current.end() && guard_set(*current).empty())
	  return Disconnect; //0subsume
	
	if (!has_empty_intersection(guard_set(*current).begin(),
				    guard_set(*current).end(),
				    command.begin(),command.end()))
	  return Delete;     //1subsume
	
	Set prev_guard_set;
	do {
	  prev_guard_set=guard_set(*current);

	  Set handle_set;
	  set_union(dominant.begin(),dominant.end(),
		    guard_set(*current).begin(),guard_set(*current).end(),
		    inserter(handle_set,handle_set.begin()));
	  
	  if (!consistent(handle_set))
	    return Delete;

	  for (sib_it child=current.begin();child!=current.end();) {
	    switch(reduce(child,handle_set,command)) {
	    case Delete:
	      return Delete; //since current is in all selections with child
	    case Disconnect:
	      child=_tr.erase(child);
	      break;
	    case Keep:
	      assert(child.begin()!=child.end()); //not sure if this is ok..
	      //make result_set the union of child's children's guard sets
	      Set result_set(guard_set(*child.begin()));
	      for_each
		(++child.begin(),child.end(),
		 bind(intersection2<Set>,ref(result_set),bind(guard_set,_1)));
	      if (!result_set.empty()) {
		union2(guard_set(*current),result_set);
		for_each
		  (child.begin(),child.end(),
		   bind(difference2<Set>,bind(guard_set,_1),result_set));
		//try to apply and-cut to child's children
		and_cut(child.begin());
		//guard_set has been enlarged, so we need to reprocess all kids
		child=current.begin();
	      } else {
		if (!and_cut(child)) //if child is adopting a terminal 
		  ++child;           //1-constrant AND, need to reprocess it
	      }
	    }
	  }
	  //try to apply or-cut to current's children
	  or_cut(current);
	} while (prev_guard_set!=guard_set(*current));
	if (current.begin()==current.end() && guard_set(*current).empty())
	  return Disconnect; //0-subsumption
      } else {
	assert(is_or(*current));
	
	for (sib_it child=current.begin();child!=current.end();) {
	  Set child_command(command);
	  for (sib_it sib=current.begin();sib!=current.end();++sib)
	    if (sib!=child && sib.begin()==sib.end() && 
		guard_set(*sib).size()==1)
	      child_command.insert(*guard_set(*sib).begin());
	  switch(reduce(child,dominant,child_command)) {
	  case Delete:
	    if (current.number_of_children()>1)
	      child=_tr.erase(child);
	    else
	      return Delete;
	    break;
	  case Disconnect:
	    return Disconnect;
	  case Keep:
	    ++child;
	  }
	}
      }
      return Keep;
    }
  } //~namespace enf
} //~namespace rewrite

std::ostream& operator<<(std::ostream& out,const rewrite::enf::Node& x) {
  if (x.first) {
    out << "[ ";
    for (rewrite::enf::Set::const_iterator it=x.second.begin();
	 it!=x.second.end();++it)
      out << *it << " ";
    out << "]";
  } else {
    out << "o";
  }
  return out;
}

