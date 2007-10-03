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


#include <iostream>
using namespace std;

namespace rewrite {
  using namespace id;

  /***
  //RemoveMinus
  template<typename T>
  void RemoveMinus<T>::visit(tree<T>& t,sib_it it) const {
    if (is_minus(*it)) {
      *it=create_plus();
      sib_it second=t.child(it,1),repl=t.append_child(it,create_times());
      t.move_after(t.append_child(repl,create_const(-1)),second);
    }
  } 

  //RemoveDivides
  template<typename T>
  void RemoveDivides<T>::visit(tree<T>& t,sib_it it) const {
    if (is_rdiv(*it)) {
      *it=create_times();
      sib_it second=t.child(it,1),repl=t.append_child(it,create_exp());
      t.move_before(t.append_child(repl,create_const(-1)),second);
    }
  } 

  //RemoveCos
  template<typename T>
  void RemoveCos<T>::visit(tree<T>& t,sib_it it) const {
    if (is_cos(*it)) {
      *it=create_sin();
      t.append_child(t.insert_above(t.begin(it),create_plus()),
		     create_const(PI/2));
    }
  }
  **/

  //EvalAlgebraicIdentities
  template<typename T>
  void EvalAlgebraicIdentities<T>::visit(tree<T>& t,sib_it it) const {
    if (is_plus(*it))
      visit_plus(t,it);
    else if (is_times(*it))
      visit_times(t,it);
    else if (is_exp(*it))
      visit_exp(t,it);
    else if (is_log(*it))
      visit_log(t,it);
  }
  
  template<typename T>
  void EvalAlgebraicIdentities<T>::visit_plus(tree<T>& t,sib_it it) const {
    cout << "vp " << tree<T>(it) << endl;
    if  (is_constant(*t.begin(it)) && get_const(*t.begin(it))==0) { //0+a->a
      t.erase(t.begin(it)); 
      if (it.number_of_children()==1)
	t.erase(t.flatten(it));
    }
    cout << "ed" << endl;
  }
  template<typename T>
  void EvalAlgebraicIdentities<T>::visit_times(tree<T>& t,sib_it it) const {
    cout << "vt " << tree<T>(it) << endl;
    if (is_constant(*t.begin(it)) && get_const(*t.begin(it))==0) { //0*a->0
      t.erase_children(it);
      *it=create_const<T>(0.0f);
    } else if (is_constant(*t.begin(it)) && get_const(*t.begin(it))==1) { 
      t.erase(t.begin(it)); //1*a->a
      if (it.number_of_children()==1)
	t.erase(t.flatten(it));
    }
    cout << "ed" << endl;
  }
  template<typename T>
  void EvalAlgebraicIdentities<T>::visit_exp(tree<T>& t,sib_it it) const {
    /***
    cout << "ve " << tree<T>(it) << endl;
    if (is_constant(*t.child(it,1)) && get_const(*t.child(it,1))==0) { //a^0->1
      t.erase_children(it); 
      *it=create_const(1.0f);
    } else if (is_constant(*t.child(it,1)) && get_const(*t.child(it,1))==1) {
	t.erase(t.child(it,1)); //a^1->a
	t.reparent(it,it.begin());
	*it=*it.begin();
	t.erase(it.begin());
	//t.erase(t.flatten(it));
    } else if (is_constant(*t.begin(it)) && get_const(*t.begin(it))==0) { 
      t.erase_children(it); //0^a->0
      *it=create_const(0.0f);
    } else if (is_constant(*t.begin(it)) && get_const(*t.begin(it))==1) { 
      t.erase_children(it); //1^a->1
      *it=create_const(1.0f);
    } else if (is_exp(*t.begin(it))) { //(a^x)^y->a^(x*y)
      t.move_before(t.begin(t.insert_above(t.child(it,1),create_times())),
		    t.child(t.begin(it),1));
      t.erase(t.flatten(t.begin(it)));
    }
    cout << "ed" << endl;
    ****/
  }
  template<typename T>
  void EvalAlgebraicIdentities<T>::visit_log(tree<T>& t,sib_it it) const {
    cout << "vl " << tree<T>(it) << endl;
    if (is_times(*t.begin(it))) { // log(a*b)->log(a)+log(b)
      for (sib_it sib=t.begin(t.begin(it));sib!=t.end(t.begin(it));++sib)
	t.insert_above(sib,create_log<T>());
      *t.begin(it)=create_plus<T>();
      t.erase(t.flatten(it));
    } else if (is_exp(*t.begin(it))) { // log(a^b)->b*log(a)
      /**t.move_before(t.begin(it),t.child(t.begin(it),1));
      *it=create_times();
      *t.child(it,1)=create_log();**/
    }
    cout << "ed " << tree<T>(it) << endl;
  } 
  
  //MergeCommons
  template<typename T>
  void MergeCommons<T>::visit(tree<T>& t,sib_it it) const {
    if (is_plus(*it)) {
      if (explicate_plus(t,it)) {
	merge_plus(t,it);
	for (sib_it sib=it.begin();sib!=it.end();++sib)
	  EvalAlgebraicIdentities<T>().visit(t,sib);
      }
    } else if (is_times(*it)) {
      if (explicate_times(t,it)) {
	cout << "explicated to " << t << endl;
	merge_times(t,it);
	cout << "after merge, we have " << t << endl;
	for (sib_it sib=it.begin();sib!=it.end();++sib) {
	  EvalAlgebraicIdentities<T>().visit(t,sib);
	  cout << "nu " << t << endl;
	}
      }
    }
  }

  template<typename T>
  bool MergeCommons<T>::explicate_plus(tree<T>& t,sib_it it) const {
    //x+c*y->1*x+c*y
    for (sib_it sib=t.begin(it);sib!=t.end(it);++sib) {
      if (is_times(*sib)) {
	for (sib_it sib1=t.begin(it);sib1!=t.end(it);++sib1)
	  if (!is_times(*sib1))
	    t.insert(t.insert_above(sib1,create_times<T>()).begin(),
		     create_const<T>(1.0f));
	return true;
      }
    }
    return false;
  }
  template<typename T>
  bool MergeCommons<T>::explicate_times(tree<T>& t,sib_it it) const {
    //x*(y^c)->(x^1)*(y^c)
    for (sib_it sib=t.begin(it);sib!=t.end(it);++sib) {
      if (is_exp(*sib)) {
	/**cout << "was " << tree<T>(it) << endl;
	for (sib_it sib1=t.begin(it);sib1!=t.end(it);) {
	  if (!is_exp(*sib1)) {
	    sib1=t.insert_above(sib1,create_exp());
	    t.insert_after(t.begin(sib1++),create_const(1.0f));
	  } else if (is_times(*t.begin(sib1))) {
	    //expand any multiplicands
	    for (sib_it sib2=sib1.begin().begin();
		 sib2!=sib1.begin().end();++sib2) {
	      sib2=t.insert_above(sib2,create_exp());
	      t.append_child(sib2,t.child(sib1,1));
	    }
	    t.erase(t.child(sib1,1));
	    sib1=t.erase(t.flatten(sib1));
	    sib1=t.erase(t.flatten(sib1));
	  } else {
	    ++sib1;
	  }
	  }**/
	cout << "now " << tree<T>(it) << endl;
	return true;
      }
    }
    return false;
  }

  //a*x+a*y->a*(x+y),a+a->2*a
  template<typename T>
  void MergeCommons<T>::merge_plus(tree<T>& t,sib_it it) const {
    for (sib_it sib1=++t.begin(it);sib1!=t.end(it);++sib1) {
      for (sib_it sib2=t.begin(it);sib2!=sib1;++sib2) {
	cout << "isect " << tree<T>(sib1) << " X " << tree<T>(sib2) << endl;
	sib_it in=sect(t,t.begin(sib1),t.end(sib1),t.begin(sib2),t.end(sib2));
	if (in!=t.begin(sib1)) {
	  cout << "match!" << endl;
	  sib_it ag=t.append_child(sib1,create_plus<T>());
	  cout << "a" << endl;
	  t.reparent(distance(in,ag)>1 ?
		     t.append_child(ag,create_times<T>()) : ag,in,ag);
	  cout << "a" << endl;
	  t.reparent(sib2.number_of_children()>1 ?
		     t.append_child(ag,create_times<T>()) : ag,sib2);
	  cout << "a" << endl;
	  t.erase(sib2);
	  --sib1;
	  cout << "a" << endl;
	  break;
	} else
	  cout << "no match" << endl;
      }
    }
    if (it.number_of_children()==1)
      t.erase(t.flatten(it));
    cout << "tush " << t << endl;
  }

  //(a^x)*(a^y)->a^(x+y)
  //note that we dont have the rule (x^a)*(y^a)->(x*y)^a
  //(or the more general x^(a*b)*y^a); the assumption is that exponents are
  //generally small and unique
  template<typename T>
  void MergeCommons<T>::merge_times(tree<T>& t,sib_it it) const {
    for (sib_it sib1=++t.begin(it);sib1!=t.end(it);++sib1) {
      for (sib_it sib2=t.begin(it);sib2!=sib1;++sib2) {
	if (t.equal_subtree(t.begin(sib1),t.begin(sib2))) { 
	  t.reparent(t.insert_above(t.child(sib1,1),create_plus<T>()),
		     t.child(sib2,1),t.end(sib2)); //(a^x)*(a^y)->a^(x+y)
	  t.erase(sib2); 
	  --sib2;
	  break;
	}
	//else if (t.equal_subtree(t.child(sib1,1),t.child(sib2,1))) {
	//(x^a)*(y^a)->(x*y)^a
      }
    }
    if (it.number_of_children()==1)
      t.erase(t.flatten(it));
  }

    /***
    sib_it prev=t.begin(it);
    for (sib_it sib=++t.begin(it);sib!=t.end(it);++sib) {
      if (is_exp(*prev) && t.equal_subtree(prev,sib)) {
	if (is_constant(*t.child(prev,1)))
	  t.replace(t.child(sib,1),create_const(get_const(*t.begin(prev))
						+get_const(*t.begin(sib))));
	else {
	  t.insert(t.begin(t.child(sib,1)),create_const(2.0f));
	  t.insert_above(t.child(sib,1),create_times());
	}
	prev=sib=t.erase(prev);
      }
    }
    ***/

  template<typename T>
  typename MergeCommons<T>::sib_it 
  MergeCommons<T>::sect(tree<T>& t,sib_it from1,sib_it to1,
			sib_it from2, sib_it to2) const {
    sib_it in=from1;
    SubtreeOrder<T> comp;
    while (from1!=to1 && from2!=to2) {
      int r=comp.cmp(from1,from2);
      if (r<0) //from1<from2
	++from1;
      else if (r>0) //from1>from2
	++from2;
      else {
	t.swap_siblings(in,from1);
	++in;
	++from1;
	from2=t.erase(from2);
      }
    }
    return in;
  }

} //~namespace rewrite
