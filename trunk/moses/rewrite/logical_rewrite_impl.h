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

namespace rewrite {
  
  //EvalLogicalIdentities
  template<typename T>
  void EvalLogicalIdentities<T>::visit(tree<T>& t,sib_it it) const {
    if (is_not(*it)) {
      if (is_true(*t.begin(it))) {
	t.erase_children(it);
	*it=create_false<T>();
      } else if (is_false(*t.begin(it))) {
	t.erase_children(it);
	*it=create_true<T>();
      }
    } else if (is_and(*it)) {
      for (sib_it sib=t.begin(it);sib!=t.end(it);) {
	if (is_false(*sib)) {
	  t.erase_children(it);
	  *it=create_false<T>();
	  return;
	} else if (is_true(*sib)) {
	  sib=t.erase(sib);
	} else {
	  ++sib;
	}
      }
      if (it.number_of_children()==0)
	*it=create_true<T>();
      else if (it.number_of_children()==1) {
	*it=*it.begin();
	t.erase(t.flatten(it.begin()));
      }	
    } else if (is_or(*it)) {
      for (sib_it sib=t.begin(it);sib!=t.end(it);) {
	if (is_true(*sib)) {
	  t.erase_children(it);
	  *it=create_true<T>();
	  return;
	} else if (is_false(*sib)) {
	  sib=t.erase(sib);
	} else {
	  ++sib;
	}
      }
      if (it.number_of_children()==0)
	*it=create_false<T>();
      else if (it.number_of_children()==1) {
	*it=*it.begin();
	t.erase(t.flatten(it.begin()));
      }	
    }
  }

  //ReduceNots
  template<typename T>
  void ReduceNots<T>::visit(tree<T>& t,sib_it it) const {
    if (is_not(*it)) {
      if (is_not(*t.begin(it))) {
	//!!a->!a
	t.flatten(t.begin(it));
	t.erase(t.begin(it));
	//!a->a
	t.replace(it,*t.begin(it));
	t.reparent(it,t.begin(it));
	t.erase(t.begin(it));
      } else if (is_and(*t.begin(it)) || is_or(*t.begin(it))) { 
	//!(a&&b) -> (!a||!b), and !(a||b) -> (!a&&!b)
	//first transform to comp_op(a,b)
	t.flatten(t.begin(it));
	*it=(is_and(*t.begin(it)) ? create_or<T>() : create_and<T>());
	t.erase(t.begin(it));
	//now to comp_op(!a,!b)
	for (sib_it sib=t.begin(it);sib!=t.end(it);++sib)
	  sib=t.insert_above(sib,create_not<T>());	
      } else if (is_bool_if(*it.begin())) {
	*it=*it.begin();
	t.erase(t.flatten(it.begin()));
	t.insert_above(++it.begin(),create_not<T>());
	t.insert_above(++(++it.begin()),create_not<T>());
      }
    }
  }

  //ReduceOrs
  template<typename T>
  void ReduceOrs<T>::visit(tree<T>& t,sib_it it) const {
    if (!is_or(*it))
      return;
    for (sib_it sib=it.begin();sib!=it.end();)
      if (is_and(*sib) && sib.begin()==sib.end())
	sib=t.erase(sib);
      else
	++sib;

    //first construct a mapping between items and ints,
    //and convert all conjunctions to this format
    NFMapper<T> mapper;
    NF f(mapper.addDNF(t.begin(it),t.end(it)));
    int sz=number_of_literals(f);
    //before this method will work, need to eliminate A&&!A clauses
    f.remove_if(bind(tautology,_1));
    //remove clauses which are subsets of others
    pairwise_erase_if(f,bind(subset_eq,_1,_2));
    
    //create implications (and remove subsets)
    for (NF::iterator c1=f.begin();c1!=f.end();++c1) {
      for (NF::iterator c2=f.begin();c2!=c1;++c2) {
	NF impls;
	implications(*c1,*c2,std::back_inserter(impls));
	for (NF::const_iterator impl=impls.begin();impl!=impls.end();++impl) {
	  for (NF::iterator c=f.begin();c!=f.end();)
	    if (c!=c1 && c!=c2 && subset_eq(*impl,*c))
	      c=f.erase(c);
	    else
	      ++c;
	  if (subset(*impl,*c1))
	    *c1=*impl;
	  if (subset(*impl,*c2))
	    *c2=*impl;
	}
      }
    }

    if (sz!=number_of_literals(f))
      mapper.extractDNF(f.begin(),f.end(),t,it);
  }

  //ReduceAnds
  template<typename T>
  void ReduceAnds<T>::visit(tree<T>& t,sib_it it) const {
    if (!is_and(*it))
      return;
    for (sib_it sib=it.begin();sib!=it.end();)
      if (is_or(*sib) && sib.begin()==sib.end())
	sib=t.erase(sib);
      else
	++sib;
    
    //first construct a mapping between items and ints,
    //and convert all conjunctions to this format
    NFMapper<T> mapper;
    NF f(mapper.addCNF(t.begin(it),t.end(it)));
    int sz=number_of_literals(f);

    //negate
    for (NF::iterator c=f.begin();c!=f.end();++c) {
      Clause tmp;
      std::transform(c->begin(),c->end(),std::inserter(tmp,tmp.begin()),
		     std::negate<int>());
      *c=tmp;
    }

    //before this method will work, need to eliminate A&&!A clauses
    f.remove_if(bind(tautology,_1));

    if (f.empty()) { //tautological expression
      t.erase_children(it);
      *it=create_true<T>();
      return;
    }

    //remove clauses which are supersets of others
    pairwise_erase_if(f,bind(subset_eq,_1,_2));

    //create implications (and remove subsets)
    for (NF::iterator c1=f.begin();c1!=f.end();++c1) {
      for (NF::iterator c2=f.begin();c2!=c1;++c2) {
	NF impls;
	implications(*c1,*c2,std::back_inserter(impls));
	for (NF::const_iterator impl=impls.begin();impl!=impls.end();++impl) {
	  for (NF::iterator c=f.begin();c!=f.end();)
	    if (c!=c1 && c!=c2 && subset_eq(*impl,*c))
	      c=f.erase(c);
	    else
	      ++c;
	  if (subset(*impl,*c1))
	    *c1=*impl;
	  if (subset(*impl,*c2))
	    *c2=*impl;
	}
      }
    }

    Clause intersect;
    if (f.size()>1 && f.front().size()>1) {
      intersect=f.front();
      for (NF::const_iterator c=++f.begin();c!=f.end() && !intersect.empty();
	   ++c) {
	if (c->size()==1) {
	  intersect.clear();
	  break;
	}
	Clause tmp;
	std::set_intersection(intersect.begin(),intersect.end(),
			      c->begin(),c->end(),inserter(tmp,tmp.begin()),
			      AdjacentNegativesComp());
	intersect=tmp;
      }
    }

    if (sz!=number_of_literals(f) || !intersect.empty()) {
      /*cout << "Anow " << t << " " << sz << " " << number_of_literals(f) 
	<< "XX " << intersect.size() << endl;*/
      for (NF::iterator c=f.begin();c!=f.end();++c) {
	for (Clause::const_iterator it=intersect.begin();
	     it!=intersect.end();++it)
	  c->erase(*it);

	Clause tmp;
	std::transform(c->begin(),c->end(),std::inserter(tmp,tmp.begin()),
		     std::negate<int>());
	*c=tmp;
      }

      Clause tmp;
      std::transform(intersect.begin(),intersect.end(),
		     std::inserter(tmp,tmp.begin()),std::negate<int>());
      intersect=tmp;
      
      mapper.extractCNF(f.begin(),f.end(),t,it);
    }

    if (!intersect.empty()) {
      *it=create_or<T>();
      t.insert(it.begin(),create_and<T>());
      t.reparent(it.begin(),++it.begin(),it.end());
      //cout << "ook" << t << " " << intersect.size() << " " << *intersect.begin() << endl;
      mapper.extractConjunction(intersect,t,t.append_child(it));
      //cout << "poo" << t << endl;
    }
    //cout << "Enow " << t << endl;
  } 

} //~namespace rewrite
