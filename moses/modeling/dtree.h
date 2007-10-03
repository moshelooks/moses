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

#ifndef DTREE_H
#define DTREE_H

#include "modeling.h"

#include <boost/bind.hpp>
#include <utility>
#include <ext/functional>
#include <ext/numeric>
#include <ext/hash_set>
#include "math_util.h"
#include <vector>

#include <iostream>
using namespace std; //!!

namespace modeling {
  using namespace __gnu_cxx;

  extern int hack_arity;


  typedef int Address;
  typedef vector<int> Permutation;
  typedef Permutation::iterator perm_iterator;
  //typedef hash_set<int> IntSet;
  /*struct IntSet {
    explicit IntSet(unsigned int v_) : v(v_) { }
    unsigned int v;
    };*/
  typedef vector<bool> IntSet;
  //typedef int IntSet;
  typedef variant<contin_t,IntSet,bool> SplitV;
  //typedef variant<contin_t,bool> SplitV;
  typedef pair<SplitV,Address> Split;
  typedef pair<perm_iterator,perm_iterator> Leaf;
  typedef variant<Split,Leaf> Node;
  typedef tree<Node> Tree;
  typedef Tree::sibling_iterator sib_iterator;
} //~namespace modeling


std::ostream& operator<<(std::ostream& out,const modeling::Node& n);

namespace modeling{
  template<typename Src>
  class DTree {
  public:
    DTree() { }
    DTree(Src from,Src to);
    DTree(const DTree& dtree);

    DTree& operator=(const DTree& dtree) {
      if (&dtree!=this) {
	_perm=dtree._perm;
	_src=dtree._src;
	_tree=dtree._tree;
	for (Tree::pre_order_iterator it=_tree.begin();it!=_tree.end();++it) {
	  if (it.number_of_children()==0) {
	    const Leaf& l=boost::get<Leaf>(*it);
	    *it=Leaf(_perm.begin()+(l.first-dtree._perm.begin()),
		     _perm.begin()+(l.second-dtree._perm.begin()));
	  }
	}
      }
      return *this;
    }

    friend class ConstTreeExtractor;
    friend class TreeExtractor;
    friend class SrcExtractor;
    template<typename T>
    friend const Tree& get_tree(const DTree<T>&);
    template<typename T>
    friend Tree& get_tree(DTree<T>&);

    Src src() const { return _src; }

    const Permutation& perm() const { return _perm; }
  private:

    Permutation _perm;
    Src _src;
    Tree _tree;
  };
  template<typename Src>
  DTree<Src> make_dtree(Src from,Src to) { return DTree<Src>(from,to); }
  
  template<typename Src>
  DTree<Src>::DTree(Src from,Src to) : _perm(distance(from,to)),_src(from),
				       _tree(Leaf(_perm.begin(),_perm.end())) {
    iota(_perm.begin(),_perm.end(),0);
  }

  template<typename Src>
  DTree<Src>::DTree(const DTree& dtree) : _perm(dtree._perm),_src(dtree._src),
					  _tree(dtree._tree) {
    for (Tree::pre_order_iterator it=_tree.begin();it!=_tree.end();++it) {
      if (it.number_of_children()==0) {
	const Leaf& l=boost::get<Leaf>(*it);
	*it=Leaf(_perm.begin()+(l.first-dtree._perm.begin()),
		 _perm.begin()+(l.second-dtree._perm.begin()));
      }
    }
  }

  struct ConstTreeExtractor : public static_visitor<const Tree&> {
    template<typename DTreeT>
    const Tree& operator()(const DTreeT& dtree) const { return dtree._tree; }
  };
  struct TreeExtractor : public static_visitor<Tree&> {
    template<typename DTreeT>
    Tree& operator()(DTreeT& dtree) const { return dtree._tree; }
  };
  struct SrcExtractor : static_visitor<Value> {
    SrcExtractor(int n) : _n(n) { }
    template<typename DTreeT>
    Value operator()(const DTreeT& dtree) const { 
      //cout << "fluga" << _n << endl;
      //cout << dtree._perm[1] << endl;
      return *(dtree._src+_n); 
    }
    const int _n;
  };

  template<typename T>
  const Tree& get_tree(const T& dtree)  { 
    return apply_visitor(ConstTreeExtractor(),dtree);
  };
  template<typename T>
  Tree& get_tree(T& dtree)  { 
    return apply_visitor(TreeExtractor(),dtree);
  };
  template<typename T>
  const Tree& get_tree(const DTree<T>& dtree)  { 
    return dtree._tree;
  };
  template<typename T>
  Tree& get_tree(DTree<T>& dtree)  { 
    return dtree._tree;
  };
  template<typename T>
  Value get_src(const T& dtree,int n)  { 
    return apply_visitor(SrcExtractor(n),dtree);
  };

  class Sampler : public static_visitor<int> {
  public:
    Sampler(const Value& v) : _v(v) { }
      
    int operator()(contin_t c) const { 
      return c<boost::get<contin_t>(_v); 
    }
    int operator()(const IntSet& h) const {
      return h[boost::get<disc_t>(_v)];
      //return (h.v&(1<<boost::get<disc_t>(_v))!=0);
      //return (h.find(boost::get<disc_t>(_v))!=h.end());
    }
    int operator()(bool isBool) const {
      return isBool ? boost::get<bool>( _v)
	: boost::get<disc_t>(_v);
    }
  private:
    const Value& _v;
  };

  //sample from the tree
  template<typename T>
  Value sample(const Assignment& ass,const T& dtree) {
    //cout << "AA" << endl;
    const Tree& tr=get_tree(dtree);

    sib_iterator it=tr.begin();
    //cout << "ti" << endl;
    while (it.number_of_children()!=0) {
      Split& split=boost::get<Split>(*it);

      /*cout << "XX" << it.number_of_children() << " P " << split.second << " "
	   << ", " << apply_visitor(Sampler(ass[split.second]),split.first) 
	   << endl;*/

      int goesTo=apply_visitor(Sampler(ass[split.second]),split.first);
      if (goesTo>=(int)it.number_of_children())
	goesTo=randint(it.number_of_children());

      sib_iterator ch=tr.child(it,goesTo);
      if (Leaf* l=boost::get<Leaf>(&(*ch))) {
	if (l->first==l->second) {
	  ch=tr.child(it,randint(it.number_of_children()));
	  while (Leaf* l=boost::get<Leaf>(&(*ch))) {
	    if (l->first!=l->second)
	      break;
	    if (++ch==it.end())
	      ch=it.begin();
	  }
	}
      }
      it=ch;
    }
    //cout << "mm" << endl;
    Leaf& leaf=boost::get<Leaf>(*it);
    //cout << "eee" << endl;

    //cout << distance(leaf.first,leaf.second) << endl;
    int x=randint(distance(leaf.first,leaf.second));
    /*cout << x << endl;
    cout << "poo" << *(leaf.first+x) << endl;
    cout << get_src(dtree,*(leaf.first+x)) << endl;*/

    return get_src(dtree,*(leaf.first+x));
  }

  template<typename T>
  class Splitter {
  public:
    typedef pair<T,int> Item;
    typedef vector<Item> Seq;

    template<typename It>
    Splitter(sib_iterator tgt,It from,Tree& tree)
      : _tgt(tgt),_ord(distance(leaf().first,leaf().second)),_tree(tree)
    {
      //create a composite containing the split source and idx
      typename Seq::iterator dest=_ord.begin();
      for (perm_iterator src=leaf().first;src!=leaf().second;++src)
	*dest++=make_pair(*(from+(*src)),*src);
    }

  protected:
    const sib_iterator _tgt;
    Seq _ord;
    Tree& _tree;

    Leaf& leaf() { return boost::get<Leaf>(*_tgt); }

    void nWaySplit();
    template<typename Comp>
    void twoWaySplit(const Comp& comp);
  };

  template<typename T>
  template<typename Comp>
  void Splitter<T>::twoWaySplit(const Comp& comp) {
    //partition
    perm_iterator pivot=leaf().first
      +distance(_ord.begin(),partition(_ord.begin(),_ord.end(),comp));
    /*cout << "partitioned into " << distance(leaf().first,pivot) << " and " 
      << distance(pivot,leaf().second) << endl;*/

    //read back the new permutation
    transform(_ord.begin(),_ord.end(),leaf().first,select2nd<Item>());

    _tree.append_child(_tgt,Leaf(pivot,leaf().second));
    _tree.append_child(_tgt,Leaf(leaf().first,pivot));
  }

  template<typename T>
  void Splitter<T>::nWaySplit() {
    sort(_ord.begin(),_ord.end());

    //read back the new permutation
    transform(_ord.begin(),_ord.end(),leaf().first,select2nd<Item>());
    _tree.append_children(_tgt,Leaf(),_ord.back().first+1);

    typename Seq::const_iterator it=_ord.begin(),prev=_ord.begin();
    sib_iterator sib=_tgt.begin();
    for (int i=0;i<it->first;++i)
      ++sib;
    for (;it!=_ord.end();++it) {
      if (it->first!=prev->first) {
	*sib=Leaf(leaf().first+(prev-_ord.begin()),
		      leaf().first+(it-_ord.begin()));
	for (int i=0;i<it->first-prev->first;++i)
	  ++sib;
	//cout << "yawn " << it-prev << endl;
	//cout << " Z " << boost
	prev=it;

      }
    }
    //cout << "yawn " << it-prev << endl;
    *_tree.last_child(_tgt)=Leaf(leaf().first+(prev-_ord.begin()),
				 leaf().second);

    //cout << "tree is " << _tree << endl;
  }

  class ContinSplitter : public Splitter<contin_t> {
  public:
    template<typename It>
    ContinSplitter(sib_iterator tgt,It from,Tree& tree)
      : Splitter<contin_t>(tgt,from,tree) { }

    sib_iterator operator()(contin_t c,Address addr) {
      twoWaySplit(bind(less<contin_t>(),c,
		       bind(select1st<pair<contin_t,int> >(),_1)));
      *_tgt=Split(c,addr);
      return _tgt;
    }
  };

  struct intset_noteq {
    intset_noteq(const IntSet& is_) : is(is_) { }
    const IntSet& is;
    bool operator()(const pair<int,int>& p) const {
      //cout << "aar " << is.v << " , " << p.first << " , " << (is.v&(1<<p.first)) << "PP" << ((is.v&(1<<p.first))!=0) << endl;
      //return ((is.v&(1<<p.first))!=0);
      return is[p.first];
    }
  };

  class DiscSplitter : public Splitter<int> {
  public:
    template<typename It>
    DiscSplitter(sib_iterator tgt,It from,Tree& tree)
      : Splitter<int>(tgt,from,tree) { }

    //an n-way split
    sib_iterator operator()(Address addr) {
      nWaySplit();
      *_tgt=Split(false,addr);
      //cout << _tgt.number_of_children() << "-way" << endl;
      return _tgt;
    }
    //a two-way split
    sib_iterator operator()(const IntSet& h,Address addr) {
      //cout << "2way" << endl;
      //twoWaySplit(bind(not_equal_to<IntSet::const_iterator>(),
      //bind(&IntSet::find,h,
      //bind(select1st<pair<int,int> >(),_1)),h.end()));
      twoWaySplit(intset_noteq(h));

      *_tgt=Split(h,addr);
      return _tgt;
      }
  };
  class BoolSplitter : public Splitter<bool> {
  public:
    template<typename It>
    BoolSplitter(sib_iterator tgt,It from,Tree& tree)
      : Splitter<bool>(tgt,from,tree) { }

    //a two-way split
    sib_iterator operator()(Address addr) {
      //cout << "bool" << endl;
      twoWaySplit(select1st<pair<bool, int> >());
      if (addr<0) {
	*_tgt=(*_tree.last_child(_tgt));
	_tree.erase_children(_tgt);
      } else {
	*_tgt=Split(true,addr);
      }
      return _tgt;
    }
  };

  template<typename DTreeT>
  sib_iterator split_on_marginal(const Split& s,cmarg_it from,
				 sib_iterator tgt,DTreeT& dt) {
    return ContinSplitter(tgt,from,get_tree(dt))
      (boost::get<contin_t>(s.first),s.second);
  }
  template<typename DTreeT>
  sib_iterator split_on_marginal(const Split& s,dmarg_it from,
				 sib_iterator tgt,DTreeT& dt) {
    DiscSplitter ds(tgt,from,get_tree(dt));
    if (const IntSet* is=boost::get<IntSet>(&s.first))
      return ds(*is,s.second);
    return ds(s.second);
  }
  template<typename DTreeT>
  sib_iterator split_on_marginal(const Split& s,bmarg_it from,
				 sib_iterator tgt,DTreeT& dt) {
    assert(boost::get<bool>(&s.first));
    return BoolSplitter(tgt,from,get_tree(dt))(s.second);
  }
  
  template<typename DTreeT>
  struct SrcSplitter : public static_visitor<sib_iterator> {
    SrcSplitter(const Split& s,sib_iterator tgt,DTreeT& dt) 
      : _s(s),_tgt(tgt),_dt(dt) { }
    
    template<typename T>
    sib_iterator operator()(const DTree<T>& t) {
      return split_on_marginal(_s,t.src(),_tgt,_dt);
    }
  private:
    const Split& _s;
    const sib_iterator _tgt;
    DTreeT& _dt;
  };  
  
  template<typename DTreeT1,typename DTreeT2>
  sib_iterator split_on_dtree(const Split& s,const DTreeT1& dt1,
			      sib_iterator tgt,DTreeT2& dt2) {
    SrcSplitter<DTreeT2> splitter(s,tgt,dt2);
    return apply_visitor(splitter,dt1);
  }

  template<typename T,typename DTreeT2>
  sib_iterator split_on_dtree(const Split& s,const DTree<T>& dt1,
			      sib_iterator tgt,DTreeT2& dt2) {
    SrcSplitter<DTreeT2> splitter(s,tgt,dt2);
    return splitter(dt1);
  }

} //~namespace modeling

#endif
