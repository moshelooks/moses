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

#ifndef LMODELING_H
#define LMODELING_H

#include "knobs.h"
#include "dtree_modeling.h"
#include "semantic_sampler.h"
#include <boost/utility.hpp>

namespace local {
  using namespace trees;
  using namespace modeling;

  enum problem_type { JOIN,ANT,BINARY_FUNCTIONS,BOOLEAN };
  extern problem_type hackproblem;
  extern int joinarity;

  void make_join_base(vtree& t,vtree::iterator it,int depth);

  //create set of knobs for a given prototype, based on a simplifier and 
  //a range of trees to use for the base neighborhood
  template<typename Simplify,typename It>
  void make_ktree(const Simplify&,It,It,const vtree&,ktree&);

  template<typename Simplify>
  struct neighborhood_params {
    neighborhood_params(int k,const vector<vtree>& perms,
			const NodeSelector<Vertex>& sel,
			const Simplify& s=Simplify())
      : _k(k),_perms(&perms),_sel(sel),_simplify(s) { }

    const Simplify& simplify() const { return _simplify; }
    const NodeSelector<Vertex>& selector() const { return _sel; }
  protected:
    int _k;
    const vector<vtree>* _perms;
    const NodeSelector<Vertex> _sel;
    Simplify _simplify;
  };

  template<typename Simplify>
  struct neighborhood  : public neighborhood_params<Simplify>,noncopyable {
    neighborhood(int k,const vector<vtree>& perms,
		 const NodeSelector<Vertex>& sel,
		 const Simplify& s=Simplify())
      : neighborhood_params<Simplify>(k,perms,sel,s),
	_vtr(create_or<Vertex>()) { }
    explicit neighborhood(const neighborhood_params<Simplify>& params)
      : neighborhood_params<Simplify>(params),_vtr(create_or<Vertex>()) { }
    
    void clear_knobs() { _ktr.clear(); }
    void make_knobs(semantic_sampler<Vertex,Simplify>& ss) {
      clear_knobs();
      make_ktree(neighborhood_params<Simplify>::_simplify,ss,
		 neighborhood_params<Simplify>::_perms->begin(),
		 neighborhood_params<Simplify>::_perms->end(),_vtr,_ktr,
		 _ugly);
      cout << "prototype: " << _vtr << endl;
    }
    void exemplar(const vtree& x) {
      _vtr=x;

      if (hackproblem==JOIN) {
	if (_vtr.size()<=3) {
	  _vtr.clear();
	  _vtr.set_head(create_argument<Vertex>(0));
	  int depth=int(log2(log2(joinarity)+1)+0.5);
	  //int depth=2;
	  make_join_base(_vtr,_vtr.begin(),depth);
	}
	  /*if (!is_progn(*_vtr.begin()))
	  _vtr.insert_above(_vtr.begin(),create_progn<Vertex>());
	  if (_vtr.begin().number_of_children()<2)
	  _vtr.append_children(_vtr.begin(),create_argument<Vertex>(0),
	  2-_vtr.begin().number_of_children());*/
      } else if (hackproblem==ANT) {
	assert(is_progn(*_vtr.begin()));
      } else {
	assert(hackproblem=BOOLEAN);
	if (!(is_and(*_vtr.begin()) && _vtr.begin().number_of_children()==1 &&
	      is_or(*_vtr.begin().begin())))
	  _vtr.insert_above(_vtr.begin(),create_or<Vertex>());
	
	for (vtree::post_order_iterator it=_vtr.begin_post();
	     it!=_vtr.end_post();++it) {
	  if (it==_vtr.begin() || it.number_of_children()>1) {
	    if (is_and(*it))
	      _vtr.append_child(it,create_or<Vertex>());
	    else
	    _vtr.append_child(it,create_and<Vertex>());
	  }
	}
	/**/
	for (vtree::pre_order_iterator it=_vtr.begin();it!=_vtr.end();++it) {
	  if (is_argument(*it)) {
	    if (is_and(*_vtr.parent(it)) && 
		_vtr.parent(it).number_of_children()>1)
	      _vtr.insert_above(it,create_or<Vertex>());
	  } else if (is_not(*it)) {
	    if (is_and(*_vtr.parent(it)) && 
		_vtr.parent(it).number_of_children()>1)
	      _vtr.insert_above(it,create_or<Vertex>());
	  }
	  }/**/
      }
    }
    const vtree& exemplar() const { return _vtr; }
    const ktree& knobs() const { return _ktr; }

    int length() const { return _ktr.size()-1; }

    float nbits() const {
      static const float log_3=log2(3.0f);
      float res=0.0f;
      
      if (hackproblem==BOOLEAN) {
	for (ktree::sibling_iterator it=_ktr.begin().begin();
	     it!=_ktr.begin().end();++it) {
	  const KnobBase& kb=get_knob(it->front());
	  if (kb.arity()==2 || kb.bad1 || kb.bad2)
	    res+=1;
	  else
	    res+=log_3;
	}
      } else if (hackproblem==ANT) {
	for (ktree::sibling_iterator it=_ktr.begin().begin();
	     it!=_ktr.begin().end();++it) {
	  const KnobBase& kb=get_knob(it->front());
	  res+=log2(kb.arity());
	}
      } else {
	assert(hackproblem==JOIN);
	res+=
	  float(distance(_ktr.begin().begin(),_ktr.begin().end()))*
	  1.0f;
	  //float(2*joinarity+1);
	  //log2(float(2*joinarity+1));
      }
      return res;
    }

    const neighborhood_params<Simplify>& params() const { return *this; }
  private:
    vtree _vtr;
    ktree _ktr;
    vector<vtree> _ugly;
  };

  void random_assignment(const ktree& ktr,float p,Assignment& dst);

  void uniform_scaled_assignment(const ktree& ktr,Assignment& dst);

  struct assigner {
    assigner(const ktree& ktr);
    void operator()(Assignment& dst) const;
    mutable int at;
    mutable int nleft;
    mutable Selector sel;
    vector<ksib_it> ksibs;
  protected:
    void do_ass(int x,Assignment& dst) const;
    vector<pair<int,int > > _map;
  };

  template<typename Out>
  void sample_assignments(const ktree& ktr,int n,Out out) {
    Assignment ass(ktr.size()-1);
    while (n-->0)
      random_assignment(ktr,0.5,*out++);
  }

  //instantiate an assignment over a set of knobs, based on a given exemplar
  void instantiate(const Assignment&,const ktree&,const vtree&,vtree&);
    
  template<typename Simplify,typename Generator,typename Out>
  Out sample_assignments(neighborhood<Simplify>& ni,const Generator& gen,
			 int n,Out out) {
    Assignment ass(ni.length());
    hash_set<vtree,trees::treehashhack> trees;
    int panic=min(n*100,max(n*2,10000));
    while (trees.size()<n && --panic>0) {
      gen(ni.knobs(),ass);

      vtree tr;
      instantiate(ass,ni.knobs(),ni.exemplar(),tr);
      ni.simplify()(tr);
      if (trees.insert(tr).second)
	*out++=ass;
    }
    return out;
  }

  template<typename Simplify,typename FF>
  float on_tree(const Assignment& ass,const neighborhood<Simplify>& ni,
		const FF& ff) {
    vtree tr;
    instantiate(ass,ni.knobs(),ni.exemplar(),tr);
    ni.simplify()(tr);
    return ff(tr);
  }

  //create an empty model for a given set of knobs and assignments
  template<typename It>
  void make_empty_model(const ktree& ktr,It from,It to,Graph& g) {
    g=Graph(distance(from,to));
    int model_size=ktr.size()-1;
    //cout << "model size: " << model_size << endl;
    g.reserve(model_size);
    ksib_it ks=ktr.begin().begin();
    for (int i=0;i<model_size;++i,++ks) {
      int arity=get_knob(ks->front()).arity();

      if (arity==2) {
	BoolMarginal& m=g.create_bool_marginal();
	for (It it=from;it!=to;++it)
	  m[distance(from,it)]=tobool((*it)[i]);
	/*transform(from,to,m.begin(),
	  bind(tobool,bind(&Assignment::operator[],_1,i)));*/

	bool t=(find(m.begin(),m.end(),true)!=m.end());
	bool f=(find(m.begin(),m.end(),false)!=m.end());

	if (t && f) 
	  g.push_back(BoolDTree(m.begin(),m.end()));
	else
	  g.push_back(BoolDTree(m.begin(),m.begin()+1));
      } else {
	DiscMarginal& m=g.create_disc_marginal();
	for (It it=from;it!=to;++it)
	  m[distance(from,it)]=todisc((*it)[i]);
	/*transform(from,to,m.begin(),
	  bind(todisc,bind(&Assignment::operator[],_1,i)));*/
	
	DiscMarginal::const_iterator it=m.begin();
	for (;it!=m.end();++it)
	  if (*it!=*m.begin())
	    break;
	if (it==m.end()) { //all the same
	  g.push_back(DiscDTree(m.begin(),m.begin()+1));
	} else { // different
	  g.push_back(DiscDTree(m.begin(),m.end()));
	}
	/**bool x0=(find(m.begin(),m.end(),0)!=m.end());
	   bool x1=(find(m.begin(),m.end(),1)!=m.end());
	bool x2=(find(m.begin(),m.end(),2)!=m.end());

	if ((!x0 && !x1) || 
	    (!x1 && !x2) ||
	    (!x0 && !x2)) {
	  g.push_back(DiscDTree(m.begin(),m.begin()+1));
	} else {
	  g.push_back(DiscDTree(m.begin(),m.end()));
	  }
	**/
      }
    }
  }

  /*** implementation of make_ktree ***/

  template<typename Simplify,typename It>
  class ktree_maker {
  public:
    ktree_maker(const Simplify& s,semantic_sampler<Vertex,Simplify>& ss,
		It from,It to,const vtree& tr,ktree& out,vector<vtree>& ugly)
      : _simplify(s),_ss(ss),_from(from),_to(to),_tr(*const_cast<vtree*>(&tr)),
	_out(out),_k(complexity(tr.begin())),_ugly(ugly) {

      _out.clear();
      _out.set_head(kseq()); //head is an empty kseq

      vtree tmp(tr);
      _simplify(tmp);
      if (!is_false(*tmp.begin())) {
	_visited.insert(tmp);
	_visited.insert(vtree(create_true<Vertex>()));
      }
      _visited.insert(vtree(create_false<Vertex>()));

      if (hackproblem==JOIN) {
	assert(is_progn(*_tr.begin()));
	join_add(_tr.begin());	
      } else if (hackproblem==ANT) {
	assert(is_progn(*_tr.begin()));
	ant_add(_tr.begin());
      } else if (hackproblem==BINARY_FUNCTIONS) {
	
      } else {
	assert(hackproblem==BOOLEAN);

	//a hack
	int n=int(sqrtf(float(distance(_from,_to))))+1;
	_ugly.reserve(n*n*_tr.size()*2+1);
	_ss.uniform_sample(n,n);
	//_ugly.reserve(distance(_from,_to)*_tr.size()+1);

	add_knobs(_tr.begin());//,_out.begin());
      }
      cleanup(_out.begin());
    }

    template<typename KnobT>
    void add_knob(KnobT knob) {
      _out.append_child(_out.begin(),kseq(1,knob));
   }

    template<typename KnobT>
    bool probe(KnobT knob) {
      for (int i=1;i<knob.arity();++i) {
	if (!knob.valid(i)) {
	  knob.disallow(i);
	  continue;
	}

	knob.turn(_tr,i);
	vtree test(_tr);
	_simplify(test);

	if (!(complexity(test)>=_k+knob.bound(i))) {
	  /**   _visited.insert(test).second)) {**/
	  knob.disallow(i);
	  /**cout << typeid(knob).name() << " failed -> " << _tr << endl
	    << " -> " << test << endl;
	  } else {
	  cout << typeid(knob).name() << " creating -> " << _tr
	  << " -> " << test << endl;**/
	}
      }
      knob.turn(_tr,0);
      if (knob.turnable()) {
	//cout << "moo" << endl;
	//cout << "src is " << vtree(knob.src) << endl;
	/*
	if (knob.src.number_of_children()==1) {
	  if (is_argument(*(knob.src.begin())))
	    cout << "PDQ " << get_argument_idx(*(knob.src.begin())) << " " << _out.begin().number_of_children() << endl;
	  else if (is_not(*(knob.src.begin())) && is_argument(*(knob.src.begin().begin())))
	    cout << "PDQ " << get_argument_idx(*(knob.src.begin().begin())) << " " << _out.begin().number_of_children() << endl;
	}
	*/
	add_knob(knob);
	//cout << "made " << typeid(knob).name() << endl;
	return true;
      }
      //cout << "Xed " << typeid(knob).name() << endl;
      return false;
    }

    bool is_literal(pre_it node) const {
      return (node.begin()==node.end() || is_not(*node) ||
	      (is_and(*node) && 
	       ++node.begin()==node.end() && 
	       is_literal(node.begin())));
    }

    void ant_add(pre_it node) {
      assert(is_progn(*node));
      for (sib_it it=node.begin();it!=node.end();++it) {
        if (is_if_food(*it)) {
          add_knob(AntMover(_tr.insert(it,create_progn<Vertex>())));
          add_knob(AntRotater(_tr.insert_after(it,create_progn<Vertex>())));
          ant_add(it.begin());
          ant_add(++it.begin());

          ++it;
        } else if (is_move(*it)) {
          pre_it myif=_tr.insert_after(it,create_if_food<Vertex>());

          pre_it first=_tr.append_child(myif,create_progn<Vertex>());
          add_knob(AntMover(_tr.append_child(first,create_progn<Vertex>())));
          add_knob(AntRotater(_tr.append_child(first,create_progn<Vertex>())));

          pre_it second=_tr.append_child(myif,create_progn<Vertex>());
          add_knob(AntMover(_tr.append_child(second,create_progn<Vertex>())));
          add_knob(AntRotater(_tr.append_child(second,create_progn<Vertex>())));

          add_knob(AntMover(it));
          add_knob(AntMover(_tr.insert(it,create_progn<Vertex>()))); //2nd
          //add_knob(AntRotater(_tr.insert(myif,create_progn<Vertex>())));
          add_knob(AntRotater(_tr.insert_after(myif,create_progn<Vertex>())));
          
          ++it;
	  ++it;
        } else {
          if (!(is_left(*it) || is_right(*it) || is_reversal(*it))) {
            cerr << "oops " << _tr << " | " << vtree(it) << endl;
          }
          assert(is_left(*it) || is_right(*it) || is_reversal(*it));

          add_knob(AntRotater(it));

          pre_it myif=_tr.insert(it,create_if_food<Vertex>());

          pre_it first=_tr.append_child(myif,create_progn<Vertex>());
          add_knob(AntMover(_tr.append_child(first,create_progn<Vertex>())));
          add_knob(AntRotater(_tr.append_child(first,create_progn<Vertex>())));

          pre_it second=_tr.append_child(myif,create_progn<Vertex>());
          add_knob(AntMover(_tr.append_child(second,create_progn<Vertex>())));
          add_knob(AntRotater(_tr.append_child(second,create_progn<Vertex>())));

          add_knob(AntMover(_tr.insert(myif,create_progn<Vertex>())));
	  
          //++it;
        }
      }
      add_knob(AntMover(_tr.append_child(node,create_progn<Vertex>())));
      
      pre_it myif=_tr.append_child(node,create_if_food<Vertex>());
      pre_it first=_tr.append_child(myif,create_progn<Vertex>());
      add_knob(AntMover(_tr.append_child(first,create_progn<Vertex>())));
      add_knob(AntRotater(_tr.append_child(first,create_progn<Vertex>())));
      pre_it second=_tr.append_child(myif,create_progn<Vertex>());
      add_knob(AntMover(_tr.append_child(second,create_progn<Vertex>())));
      add_knob(AntRotater(_tr.append_child(second,create_progn<Vertex>())));

      add_knob(AntRotater(_tr.insert_after(myif,create_progn<Vertex>())));
    }


#if 0
    void ant_add(pre_it node) {
      assert(is_progn(*node));
      for (sib_it it=node.begin();it!=node.end();++it) {
	if (is_if_food(*it)) {
	  add_knob(AntRotater(_tr.insert(it,create_progn<Vertex>())));
	  add_knob(AntMover(_tr.insert_after(it,create_progn<Vertex>())));
	  ant_add(it.begin());
	  ant_add(++it.begin());

	  ++it;
	} else if (is_move(*it)) {
	  pre_it myif=_tr.insert(it,create_if_food<Vertex>());

	  pre_it first=_tr.append_child(myif,create_progn<Vertex>());
	  add_knob(AntRotater(_tr.append_child(first,create_progn<Vertex>())));
	  add_knob(AntMover(_tr.append_child(first,create_progn<Vertex>())));

	  pre_it second=_tr.append_child(myif,create_progn<Vertex>());
	  add_knob(AntRotater(_tr.append_child(second,create_progn<Vertex>())));
	  add_knob(AntMover(_tr.append_child(second,create_progn<Vertex>())));

	  add_knob(AntMover(it));
	  add_knob(AntMover(_tr.insert_after(it,create_progn<Vertex>()))); //2nd
	  add_knob(AntRotater(_tr.insert(myif,create_progn<Vertex>())));
	  
	  ++it;
	} else {
	  if (!(is_left(*it) || is_right(*it) || is_reversal(*it))) {
	    cerr << "oops " << _tr << " | " << vtree(it) << endl;
	  }
	  assert(is_left(*it) || is_right(*it) || is_reversal(*it));

	  add_knob(AntRotater(it));

	  pre_it myif=_tr.insert_after(it,create_if_food<Vertex>());

	  pre_it first=_tr.append_child(myif,create_progn<Vertex>());
	  add_knob(AntRotater(_tr.append_child(first,create_progn<Vertex>())));
	  add_knob(AntMover(_tr.append_child(first,create_progn<Vertex>())));

	  pre_it second=_tr.append_child(myif,create_progn<Vertex>());
	  add_knob(AntRotater(_tr.append_child(second,create_progn<Vertex>())));
	  add_knob(AntMover(_tr.append_child(second,create_progn<Vertex>())));

	  add_knob(AntMover(_tr.insert_after(myif,create_progn<Vertex>())));

	  ++it;
	  ++it;
	}
      }

      add_knob(AntRotater(_tr.append_child(node,create_progn<Vertex>())));
      
      pre_it myif=_tr.append_child(node,create_if_food<Vertex>());
      pre_it first=_tr.append_child(myif,create_progn<Vertex>());
      add_knob(AntRotater(_tr.append_child(first,create_progn<Vertex>())));
      add_knob(AntMover(_tr.append_child(first,create_progn<Vertex>())));
      pre_it second=_tr.append_child(myif,create_progn<Vertex>());
      add_knob(AntRotater(_tr.append_child(second,create_progn<Vertex>())));
      add_knob(AntMover(_tr.append_child(second,create_progn<Vertex>())));

      add_knob(AntMover(_tr.insert_after(myif,create_progn<Vertex>())));
    }
#endif

    void rand_join(int n,pre_it node) {
      if (n==0) {
	add_knob(JoinKnob(node,joinarity));
      } else {
	int split=select_by_catalan(n-1);
	*node=create_progn<Vertex>();
	_tr.append_children(node,create_argument<Vertex>(0),2);
	rand_join(split,node.begin());
	rand_join(n-1-split,++node.begin());
      }
    }
    void rand_join(int n,pre_it node,pre_it zoop) {
      if (n==0) {
	_tr.move_ontop(node,zoop);
      } else {
	int split=select_by_catalan(n-1);
	*node=create_progn<Vertex>();
	_tr.append_children(node,create_argument<Vertex>(0),2);
	bool goleft=(randint(n+1)<split+1);
	if (goleft) {
	  rand_join(split,node.begin(),zoop);
	  rand_join(n-1-split,++node.begin());
	} else {
	  rand_join(split,node.begin());
	  rand_join(n-1-split,++node.begin(),zoop);
	}
      }
    }	

    //a knob (with dummy removal possibility) for every leaf
    //the insertion scheme creates a new leaf for every node except for root
    //if both sibs get removed, then parent gets removed too (conceptually)
    void join_add(pre_it node) {
      assert(node.number_of_children()==2);

      //recurse
      if (node.begin().number_of_children()>0)
	join_add(node.begin());
      else
	add_knob(JoinKnob(node.begin(),joinarity));
      if ((++node.begin()).number_of_children()>0)
	join_add(++node.begin());
      else
	add_knob(JoinKnob(++node.begin(),joinarity));
      
      sib_it first=_tr.insert_above(node.begin(),create_progn<Vertex>());
      sib_it second=_tr.insert_above(++node.begin(),create_progn<Vertex>());

      rand_join(int(log2(joinarity)+0.5)-1,_tr.append_child(first),first.begin());
      assert(first.number_of_children()==1);
      _tr.erase(_tr.flatten(first));

      rand_join(int(log2(joinarity)+0.5)-1,_tr.append_child(second),second.begin());
      assert(second.number_of_children()==1);
      _tr.erase(_tr.flatten(second));

#if 0
      //create new nodes
      for (int i=0;i<int(log2(joinarity)+0.5);++i) {

	sib_it first=_tr.insert_above(node.begin(),create_progn<Vertex>());
	sib_it second=_tr.insert_above(++node.begin(),create_progn<Vertex>());

	//first new node
	_tr.insert(first.begin(),create_argument<Vertex>(0));
	add_knob(JoinKnob(first.begin(),joinarity));

	//second new node
	_tr.append_child(second,create_argument<Vertex>(0));
	add_knob(JoinKnob(++second.begin(),joinarity));
      }
#endif
    }

    // a knob for every internal node
    // with one or more leaves as children
    //
    // the insertion scheme creates a new node (with an additional removal 
    // setting) with a leaf child
    // for every internal node except for the root based on parentage;
    // a left child gets moved to the right, a right child to the left
    void binary_functions_add(pre_it node) {
#if 0
      assert(node.number_of_children()==2);

      //recurse first
      if (node.begin().number_of_children()>0)
	binary_functions_add(node.begin());
      if ((++node.begin()).number_of_children()>0)
	binary_functions_add(++node.begin());

      sib_it first=_tr.insert_above(node.begin(),
				    create_argument<Vertex>(0));
      sib_it second=_tr.insert_above(++node.begin(),
				     create_argument<Vertex>(0));

      add_knob(BFKnob(first,
		      _tr.insert(first.begin(),create_argument<Vertex>(0)),
		      bfarity));
      add_knob(BFKKnob(second,
		       _tr.append_child(second,create_argument<Vertex>(0)),
		       bfarity));
      
      add_knob(BFKnob(node,bfarity));
#endif
    }

    void add_knobs(pre_it node) {
      bool lit=is_literal(node);
      if (lit && !is_and(*node) && !is_or(*node))
	return;

      /**if (!lit)
	 probe(Flip(node));**/
      
      for (It sub=_from;sub!=_to;++sub)
	probe(Child(node,sub->begin()));

      /****
      int n=distance(_from,_to);
      int panix=n;
      _ss.uniform_sample(1,n);

      assert(_ss.trees(1).size()>0);
      assert(_ss.isFull(1));
      random_shuffle(_ss.trees(1).begin(),_ss.trees(1).end());
      for (vector<vtree>::const_iterator it=_ss.trees(1).begin();
	   it!=_ss.trees(1).end() && n>0 && panix>0;++it) {
	_ugly.push_back(*it);
	if (probe(Child(node,_ugly.back().begin()))) {
	  --n;
	} else {
	  _ugly.pop_back();
	  --panix;
	}
      }
      ****/

      /********/
      {
      int n=int(sqrtf(float(distance(_from,_to))))+1;
      int panix=n;
      for (int i=2;i<=n;++i) {
	for (int j=0;j<n;++j) {
	  if (_ss.trees(i).size()==0) {
	    //cout << "conting " << i << endl;
	    continue;
	  }
	  if (_ss.isFull(i)) {
	    _ugly.push_back(_ss.trees(i)[randint(_ss.trees(i).size())]);
	  } else {
	    assert(_ugly.capacity()>_ugly.size()+1);
	    _ugly.push_back(_ss.trees(i).back());
	    _ss.trees(i).pop_back(); 
	  }
	  if (!probe(Child(node,_ugly.back().begin()))) {
	    _ugly.pop_back();
	    if (--panix>=0)
	      --j;
	  }
	  if (_ss.trees(i).empty())
	    _ss.generate_tree(i);
	}
      }

      }
      /****/

      int n=distance(_from,_to);
      int panix=n;
      for (int i=1;i<=1;++i) {
	for (int j=0;j<n;++j) {
	  if (_ss.trees(i).size()==0) {
	    cout << "conting " << i << endl;
	    continue;
	  }
	  if (_ss.isFull(i)) {
	    _ugly.push_back(_ss.trees(i)[randint(_ss.trees(i).size())]);
	  } else {
	    assert(_ugly.capacity()>_ugly.size()+1);
	    _ugly.push_back(_ss.trees(i).back());
	    _ss.trees(i).pop_back(); 
	  }
	  if (!probe(Child(node,_ugly.back().begin()))) {
	    _ugly.pop_back();
	    if (--panix>=0)
	      --j;
	  }
	  if (_ss.trees(i).empty())
	    _ss.generate_tree(i);
	}
      }

      for (sib_it sib=node.begin();sib!=node.end();) {
	sib_it nxt=sib;
	++nxt;
	if (is_literal(sib) && !is_and(*sib) && !is_or(*sib))
	  probe(Removal(node,sib));
	sib=nxt;
      }

      for_each(make_counting_iterator(node.begin()),
	       make_counting_iterator(node.end()),
	       bind(&ktree_maker::add_knobs,this,_1));

    }

    void cleanup(ksib_it loc) {
      if (loc->empty()) {
	if (loc.begin()==loc.end())
	  _out.erase(loc);
	else if (++loc.begin()==loc.end())
	  _out.erase(_out.flatten(loc));
      }
      for_each(make_counting_iterator(loc.begin()),
	       make_counting_iterator(loc.end()),
	       bind(&ktree_maker::cleanup,this,_1));
    }
  private:
    const Simplify& _simplify;
    semantic_sampler<Vertex,Simplify>& _ss;
    It _from,_to;
    vtree& _tr;
    ktree& _out;

    const int _k;
    hash_set<vtree,trees::treehashhack> _visited;

    vector<vtree>& _ugly;
  };

  template<typename Simplify,typename It>
  void make_ktree(const Simplify& s,
		  semantic_sampler<Vertex,Simplify>& ss,
		  It from,It to,const vtree& tr,ktree& out,
		  vector<vtree>& ugly) {
    ktree_maker<Simplify,It>(s,ss,from,to,tr,out,ugly);
  }

} //~namespace local

#endif

