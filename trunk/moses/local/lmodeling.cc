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

#include "lmodeling.h"
#include <boost/iterator/reverse_iterator.hpp>

namespace local {

  problem_type hackproblem=JOIN;
  int joinarity=0;

  void make_join_base(vtree& t,vtree::iterator it,int depth) {
    if (depth==0) {
      *it=create_argument<Vertex>(0);
    } else {
      *it=create_progn<Vertex>();
      t.append_child(it);
      t.append_child(it);
      make_join_base(t,it.begin(),depth-1);
      make_join_base(t,++it.begin(),depth-1);
    }
  }

  KnobBase& get_knob(Knob& knob) { 
    return *apply_visitor(KnobBaseGetter(),knob); 
  }
  const KnobBase& get_knob(const Knob& knob) { 
    return *apply_visitor(ConstKnobBaseGetter(),knob);
  }

  void random_assignment(const ktree& ktr,float p,Assignment& ass) {
    Assignment::iterator a=ass.begin();
    for (ksib_it ks=ktr.begin().begin();ks!=ktr.begin().end();++ks) {
      if (get_knob(ks->front()).arity()==2) {
	if (randfloat()<p)
	  *a++=true;
	else
	  *a++=false;
      } else {
	if (randfloat()<p)
	  *a++=disc_t(get_knob(ks->front()).random_nonzero());
	else
	  *a++=disc_t(0);
      }
    }
    assert(a==ass.end());
  }

  assigner::assigner(const ktree& ktr) : at(1),nleft(0),sel(0) {
    for (ksib_it ks=ktr.begin().begin();ks!=ktr.begin().end();++ks) {
      int arity=get_knob(ks->front()).arity();
      for (int i=1;i<arity;++i)
	_map.push_back(make_pair(i,ksibs.size()));
      ksibs.push_back(ks);
    }
    nleft=_map.size();
    sel=Selector(_map.size());
  }

  void assigner::do_ass(int x,Assignment& dst) const {
    pair<int,int> p=_map[x];
    if (get_knob(ksibs[p.second]->front()).arity()==2) //bool
      dst[p.second]=true;
    else
      dst[p.second]=disc_t(p.first);
  }

  void assigner::operator()(Assignment& dst) const {
    //cout << "XX" << endl;

    //clear the ass
    vector<ksib_it>::const_iterator ks=ksibs.begin();

    /***
    for (Assignment::iterator it=dst.begin();it!=dst.end();++it,++ks)
      if (get_knob((*ks)->front()).arity()==2) {
	if (randfloat()<0.05)
	  *it=true;
	else
	  *it=false;
      } else {
	if (randfloat()<0.05)
	  *it=disc_t(get_knob((*ks)->front()).random_nonzero());
	else
	  *it=disc_t(0);
      }
    return;
    **/

    for (Assignment::iterator it=dst.begin();it!=dst.end();++it,++ks)
      if (get_knob((*ks)->front()).arity()==2)
	*it=false;
      else
	*it=disc_t(0);

    if (at==1) { //we're on one
      do_ass(sel(),dst);
      if (sel.empty()) {
	++at;
      }
    } else {
      if (at!=-1 && nleft--==0) {
	++at;
	nleft=_map.size();
      }
      if (at>dst.size()/2)
	at=-1;
      if (at==-1) {
	ks=ksibs.begin();
	for (Assignment::iterator it=dst.begin();it!=dst.end();++it,++ks)
	  if (get_knob((*ks)->front()).arity()==2) {
	    if (randfloat()<0.5)
	      *it=true;
	    else
	      *it=false;
	  } else {
	    if (randfloat()<0.5)
	      *it=disc_t(get_knob((*ks)->front()).random_nonzero());
	    else
	      *it=disc_t(0);
	  }
      } else {
	set<int> done;
	for (int i=0;i<at;++i) {
	  int x;
	  do {
	    x=randint(_map.size());
	  } while (done.find(_map[x].second)!=done.end());
	  done.insert(_map[x].second);
	  do_ass(x,dst);
	}
      }
    }
    //cout << "generated " << dst << endl;
  }


  void uniform_scaled_assignment(const ktree& ktr,Assignment& ass) {
    float p=float(1+randint(ass.size()))/float(ass.size()*2);
    Assignment::iterator a=ass.begin();
    for (ksib_it ks=ktr.begin().begin();ks!=ktr.begin().end();++ks) {
      if (get_knob(ks->front()).arity()==2) {
	if (randfloat()<p)
	  *a++=true;
	else
	  *a++=false;
      } else {
	if (randfloat()<p)
	  *a++=disc_t(get_knob(ks->front()).random_nonzero());
	else
	  *a++=disc_t(0);
      }
    }
    assert(a==ass.end());
  }

  void instantiate(const Assignment& ass,const ktree& ktr,const vtree& src,
		   vtree& dst) {
    vtree original=src;
    vtree& tmp=(*const_cast<vtree*>(&src));
    assert(&tmp!=&dst);
    Assignment::const_iterator a=ass.begin();
    for (ksib_it ks=ktr.begin().begin();ks!=ktr.begin().end();++ks,++a) {
      const bool* b=boost::get<bool>(&*a);
      if (b) {
	if (*b) {
	  //cout << "turning " << (ks->front()) << endl;	
	  get_knob(ks->front()).turn(tmp,1);
	  //cout << " -> " << tmp << endl;
	}
      } else {
	const disc_t* d=boost::get<disc_t>(&*a);
	assert(d);
	if (*d>0)
	  get_knob(ks->front()).turn(tmp,*d);
      }
    }
    dst=tmp;
    for (boost::reverse_iterator<ksib_it> ks=
	   make_reverse_iterator(ktr.begin().end());
	 ks!=make_reverse_iterator(ktr.begin().begin());++ks)
      get_knob(ks->front()).turn(tmp,0);
    assert(original==src);
  }
   
  /*  void make_empty_model::make_empty_model(const ktree& ktr,Graph& g) : _g(g) {
    assert(g.empty());

    //generate the initial 

    for_each(ktr.begin().begin(),ktr.begin().end(),bind(rec_make,_1,NULL));
  }
  void rec_make(ksib_it loc,Graph::iterator* sup_dtree) {
    if 
  }
  




      //generate assignments
      Assignment ass(_knobs.size());
      for (tree<Knob>::sibling_iterator it=_knobs.begin();it!=_knobs.end();++it)
      apply_visitor(Knobber(NULL),*it);

    }
  void create(Graph::iterator* sup,tree<Knob>::iterator it) {
    if (Removal* r=boost::get<Removal>(&*it))
      
      
    

  */

} //~namespace local

std::ostream& operator<<(std::ostream& out,const local::Knob& k) {
  return (out << typeid(get_knob(k)).name());
}


/******

  void instantiate(const Assignment& ass,const ktree& ktr,const vtree& src,
		   vtree& dst) {
    vtree original=src;
    vtree& tmp=(*const_cast<vtree*>(&src));
    assert(&tmp!=&dst);
    Assignment::const_iterator a=ass.begin();
    for (kpre_it ks=ktr.begin();ks!=ktr.end();++ks,++a) {
      const bool* b=boost::get<bool>(&*a);
      if (b && *b) {
	cout << "turning " << (ks->front()) << endl;	
	get_knob(ks->front()).turn(tmp);
	cout << " -> " << tmp << endl;
      }
      const int* i=boost::get<int>(&*a);
      if (i && *i) {
	cout << "turning " << (*ks)[*i] << endl;		
	get_knob((*ks)[*i]).turn(tmp);
	cout << " -> " << tmp << endl;
      }
    }
    dst=tmp;
    for (boost::reverse_iterator<kpre_it> ks=make_reverse_iterator(ktr.end());
	 ks!=make_reverse_iterator(ktr.begin());++ks) {
      const bool* b=boost::get<bool>(&*--a);
      if (b && *b) {
	cout << "unturning " << (ks->front()) << endl;
	get_knob(ks->front()).unturn(tmp);
	cout << " -> " << tmp << endl;
      }
      const int* i=boost::get<int>(&*a);
      if (i && *i) {
	cout << "unturning " << (*ks)[*i] << endl;
	get_knob((*ks)[*i]).unturn(tmp);
	cout << " -> " << tmp << endl;
      }
    }
    assert(original==src);
  }
*********/
