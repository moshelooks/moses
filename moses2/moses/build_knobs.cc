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

#include "moses/build_knobs.h"
#include "reduct/meta_rules.h"
#include "reduct/general_rules.h"
#include "util/lazy_random_selector.h"
#include "util/dorepeat.h"

using namespace std;

namespace moses {
  typedef vtree::sibling_iterator sib_it;
  typedef vtree::pre_order_iterator pre_it;
  using util::from_one;

  build_knobs::build_knobs(vtree& exemplar,const combo::tree_type& t,
			   knob_mapper& mapper,contin_t step_size,contin_t expansion,
			   eda::field_set::arity_t depth)
    : _exemplar(exemplar),_type(t),_mapper(mapper),
      _arity(t.begin().number_of_children()-1),
      _step_size(step_size),_expansion(expansion),_depth(depth)
  { 
    if (combo::output_type(_type)==combo::id::boolean) {
      logical_canonize(_exemplar.begin());
      build_logical(_exemplar.begin());
      logical_cleanup();
    } else {
      assert(combo::output_type(_type)==combo::id::contin);
      cout << "BK" << endl;
      contin_canonize(_exemplar.begin());
      cout << "BK" << endl;
      build_contin(_exemplar.begin());
    }      
  }

  void build_knobs::build_logical(pre_it it) { 
    add_logical_knobs(it);

    if (*it==id::logical_and) {
      for (vtree::sibling_iterator sib=it.begin();sib!=it.end();++sib)
	if (is_argument(*sib))
	  add_logical_knobs(_exemplar.insert_above(sib,id::logical_or),false);
	else if (*sib==id::null_vertex)
	  break;
	else
	  build_logical(sib);
      add_logical_knobs(_exemplar.append_child(it,id::logical_or));
    } else if (*it==id::logical_or) {
      for (vtree::sibling_iterator sib=it.begin();sib!=it.end();++sib)
	if (is_argument(*sib))
	  add_logical_knobs(_exemplar.insert_above(sib,id::logical_and),false);
	else if (*sib==id::null_vertex)
	  break;
	else
	  build_logical(sib);
      add_logical_knobs(_exemplar.append_child(it,id::logical_and));
    }
  }

  void build_knobs::build_contin(pre_it it) {
    pre_it end=it;
    end.skip_children();
    for (++end;it!=end;++it)
      if (is_contin(*it)) {
	contin_knob kb(_exemplar,it,_step_size,_expansion,_depth);
	_mapper.contin.insert(make_pair(kb.spec(),kb));
      }
  }

  void build_knobs::logical_canonize(pre_it it) {
    if (*it==id::logical_true || *it==id::logical_false)
      *it=id::logical_and;
    else if (*it!=id::logical_and && *it!=id::logical_or)
      it=_exemplar.insert_above(it,id::logical_and);

    if (*it==id::logical_and)
      _exemplar.insert_above(it,id::logical_or);
    else
      _exemplar.insert_above(it,id::logical_and);
  }

  void build_knobs::add_logical_knobs(pre_it it,bool add_if_in_exemplar) {
    vector<vtree> perms;
    sample_logical_perms(it,perms);
    foreach(const vtree& tr,perms)
      logical_probe(tr,it,add_if_in_exemplar);
  }

  void build_knobs::sample_logical_perms(pre_it it,vector<vtree>& perms) {
    //all n literals
    foreach(int i,from_one(_arity))
      perms.push_back(vtree(argument(i)));
      
    //and n random pairs out of the total  2 * choose(n,2) = n * (n - 1) of these
    //TODO: should bias the selection of these (and possibly choose larger subtrees)
    util::lazy_random_selector select(_arity*(_arity-1));
    dorepeat(_arity) {
      //while (!select.empty()) {
      vtree v(*it==id::logical_and ? id::logical_or : id::logical_and);
      int x=select();
      int a=x/(_arity-1);
      int b=x-a*(_arity-1);
      if (b==a)
	b=_arity-1;

      if (b<a) {
	v.append_child(v.begin(),argument(1+a));
	v.append_child(v.begin(),argument(1+b));
      } else {
	v.append_child(v.begin(),argument(1+b));
	v.append_child(v.begin(),argument(-(1+a)));
      }

      perms.push_back(v);
    }
  }

  void build_knobs::logical_probe(const vtree& tr,pre_it it,
				  bool add_if_in_exemplar) {
    logical_subtree_knob kb(_exemplar,it,tr.begin());
    if ((add_if_in_exemplar || !kb.in_exemplar()) && disc_probe(it,kb))
      _mapper.disc.insert(make_pair(kb.spec(),kb));
  }

  void build_knobs::logical_cleanup() {
    vtree::post_order_iterator it=_exemplar.begin_post();
    while (it!=_exemplar.end_post())
      if ((*it==id::logical_and || *it==id::logical_or) && it.is_childless())
	_exemplar.erase(it++);
      else
	++it;
    if (_exemplar.empty())
      _exemplar.set_head(id::logical_and);
  }

  bool build_knobs::disc_probe(pre_it parent,disc_knob_base& kb) {
    using namespace reduct;

    vector<int> to_disallow;
    foreach(int idx,from_one(kb.arity()-1)) {

      /*vtree XX(_exemplar);apply_rule(downwards(remove_null_vertices()),XX);
	cout << "exemplar " << XX << " -> ";*/
      
      //this is messed up - turn requires <3

      kb.turn(idx);

      /*XX=vtree(_exemplar);apply_rule(downwards(remove_null_vertices()),XX);
	cout << XX << endl;*/
	
      //fast way
      /**
	 note: representation-building is a bit tricky & rotten - the pairwise 
	 probing approach should be better, but wait until contin is ready to to 
	 test..
	 vtree probing_tr(vertex());
	 copy_no_nulls(parent,probing_tr.begin());
	 for (vtree::upwards_iterator up=_exemplar.parent(parent);
	 up!=_exemplar.end_upwards() && is_associative(*up);++up) {
	 probing_tr.insert_above(probing_tr.begin(),*up);
	 for (vtree::sibling_iterator sib=up.begin();sib!=up.end();++sib) {
	 if (*sib==id::null_vertex || sib.node!=parent.node)
	 continue;
	 vtree sib_tr(vertex());
	 copy_no_nulls(sib,sib_tr.begin());
	 apply_rule(sequential(remove_null_vertices(),
	 ),exemplar_no_nulls);
	 if (sib.is_childless() && sib.node!=parent.node)
	 tmp.append_child(tmp.begin(),*sib); // *sib not sib cuz only childless
	 }
	 complexity_t initial_c=complexity(tmp.begin());
      **/
      /*XX=vtree(tmp);apply_rule(downwards(remove_null_vertices()),XX);
	cout << "tmp= " << XX << endl;*/

      //slow way
      /**/
      vtree tmp(_exemplar);
      complexity_t _c=complexity(_exemplar.begin());
      complexity_t initial_c=_c;//+kb.complexity_bound();
      /**/	
      cout << "doing " << tmp << endl;
      clean_and_full_reduce(tmp);

      cout << initial_c << " vs. " << complexity(tmp.begin()) << endl;
      //note that complexity is negative, with -inf being highest, 0 lowest
      if (initial_c<complexity(tmp.begin())) {
	vtree XX(_exemplar);
	clean_reduce(XX);
	

	//apply_rule(downwards(remove_null_vertices()),XX,XX.begin());

	vtree YY(tmp);
	clean_reduce(YY);

	cout << "disallowing " << XX << " -> " << tmp << endl;
	to_disallow.push_back(idx);
      } else {
	vtree XX(_exemplar);
	clean_reduce(XX);
	
	vtree YY(tmp);
	clean_reduce(YY);

	cout << "allowing " << XX << " -> " << YY << endl;
      }
    }
    kb.turn(0);

    //if some settings aren't disallowed, make a knob
    if (int(to_disallow.size())<kb.arity()-1) { 
      foreach (int idx,to_disallow)
	kb.disallow(idx);
      return true;
    } else {
      kb.clear_exemplar();
      return false;
    }
  }

  // the canonical form we want for a term is linear-weighted fraction whose
  // numerator & denominator are either generalized polynomials or terms, i.e.:
  //
  // c1 + (c2 * p1) / p2
  //
  // generalized polys (p1 and p2) can contain:
  //
  // x, sin, log, exp, x*y
  // (where x & y are any vars)
  //
  // we assume that reduction has already taken place
  //
  // if there are multiple divisors, they will be transformed into separate terms
  void build_knobs::contin_canonize(pre_it it) {
    if (*it==id::div) {
      assert(it.number_of_children()==2);
      _exemplar.append_child(_exemplar.insert_above(it,id::plus),contin_t(0));
      
      canonize_div(it);
    } else if (*it==id::plus) {
      //move the constant child upwards
      add_constant_child(it,0);
      _exemplar.insert_above(it,id::plus);
      _exemplar.move_after(it,pre_it(it.last_child()));
      //handle any divs
      for (sib_it div=_exemplar.partition(it.begin(),it.end(),
					  bind(not_equal_to<vertex>(),_1,id::div));
	   div!=it.end();)
	canonize_div(_exemplar.move_after(it,pre_it(div++)));
      //handle the rest of the children
      _exemplar.append_child(_exemplar.insert_above(it,id::div),contin_t(1));
      canonize_div(_exemplar.parent(it));
    } else {
      _exemplar.append_child(_exemplar.insert_above(it,id::plus),contin_t(0));
      _exemplar.append_child(_exemplar.insert_above(it,id::div),contin_t(1));

      canonize_div(_exemplar.parent(it));
    }

    cout << "ok " << _exemplar << endl;
  }

  void build_knobs::canonize_div(pre_it it) {
    canonize_times(it.begin());
    linear_canonize(it.begin().begin());
    linear_canonize(it.last_child());	
  }

  void build_knobs::add_constant_child(pre_it it,contin_t v) {
    sib_it sib=find_if(it.begin(),it.end(),is_contin);
    if (sib==it.end())
      _exemplar.append_child(it,v);
    else
      _exemplar.swap(sib,it.last_child());
  }

  //make it binary * with second arg a constant
  pre_it build_knobs::canonize_times(pre_it it) {
    sib_it sib=(*it!=id::times ? it.end() : find_if(it.begin(),it.end(),is_contin));
    if (sib==it.end()) {
      _exemplar.append_child(_exemplar.insert_above(it,id::times),
			     contin_t(1));
      return _exemplar.parent(it);
    } else if (it.number_of_children()>2) {
      _exemplar.insert_above(it,id::times);
      _exemplar.move_after(it,pre_it(sib));
      return _exemplar.parent(it);
    } else {
      _exemplar.swap(sib,it.last_child());
      return it;
    }
  }

  void build_knobs::linear_canonize(pre_it it) { 
    //make it a plus
    if (*it!=id::plus)
      it=_exemplar.insert_above(it,id::plus);
    add_constant_child(it,0);

    //add the basic elements and recurse, if necessary
    rec_canonize(it);
  }

  void build_knobs::rec_canonize(pre_it it) {
    cout << "X " << _exemplar << " | " << vtree(it) << endl;
    //recurse on whatever's already present, and create a multiplicand for it
    if (*it==id::plus) {
      for (sib_it sib=it.begin();sib!=it.end();++sib) {
	if (!is_contin(*sib)) {
	  sib=canonize_times(sib);
	  rec_canonize(sib.begin());
	  assert(is_contin(*sib.last_child()));
	  rec_canonize(_exemplar.insert_above(sib.last_child(),id::plus));
	}
      }

      //add the basic elements: sin, log, exp, and any variables (#1, ..., #n)
      append_linear_combination(mult_add(it,id::sin));
      append_linear_combination(mult_add(it,id::log));
      append_linear_combination(mult_add(it,id::exp));
      append_linear_combination(it);
    } else if (*it==id::sin || *it==id::log || *it==id::exp) {
      cout << _exemplar << " | " << vtree(it) << endl;
      linear_canonize(it.begin());
    } else {
      assert(is_argument(*it));
    }
  }

  void build_knobs::append_linear_combination(pre_it it) {
    if (*it!=id::plus)
      it=_exemplar.append_child(it,id::plus);
    foreach(int idx,from_one(_arity))
      mult_add(it,argument(idx));
  }    

  pre_it build_knobs::mult_add(pre_it it,const vertex& v) {
    return --_exemplar.append_child
      (_exemplar.insert_above
       (_exemplar.append_child(it,v),id::times),contin_t(0));
  }

} //~namespace moses
