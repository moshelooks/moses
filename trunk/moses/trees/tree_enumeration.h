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

#ifndef TREE_ENUMERATION_H
#define TREE_ENUMERATION_H

#include "tree.h"
#include "selection.h"
#include <vector>
#include <ext/numeric>
#include <ext/hash_set>
#include <ext/algorithm>

namespace trees {

  //how many binary trees with a given size and contents?
  template<typename T>
  unsigned long int number_of_binary_trees(int n,const NodeSelector<T>& s) {
    using namespace __gnu_cxx;
    return catalan(n)*
      power<unsigned long int>(s.count_with_arity(2),n)*
      power<unsigned long int>(s.count_with_arity(1)>0 ? 
			       2*s.count_with_arity(0) :
			       s.count_with_arity(0),n+1);
  }

  //all tree shapes with n internal nodes
  //using copies of an element t as tree contents
  template<typename T,typename Out>
  Out enumerate_all_binary_tree_shapes(int n,T t,Out out) {
    using namespace std;
    typedef tree<T> Tree;
    if (n==0) { //just a leaf
      out->clear();
      out->set_head(t);
      ++out;
    } else {
      //paste together smaller tree shapes
      vector<vector<Tree> > sub(n);
      for (int i=0;i<n;++i) {
	sub[i].resize(catalan(i));
	enumerate_all_binary_tree_shapes(i,t,sub[i].begin());
      }

      typename vector<vector<Tree> >::const_iterator c1=sub.begin();
      typename vector<vector<Tree> >::const_reverse_iterator c2=sub.rbegin();
      for (;c1!=sub.end();++c1,++c2) {
	for (typename vector<Tree>::const_iterator tr1=c1->begin();
	     tr1!=c1->end();++tr1) {
	  for (typename vector<Tree>::const_iterator tr2=c2->begin();
	       tr2!=c2->end();++tr2) {
	    out->clear();
	    out->set_head(t);
	    out->append_child(out->begin(),tr1->begin());
	    out->append_child(out->begin(),tr2->begin());
	    ++out;
	  }
	}
      }
    }
    return out;
  }
    
  //all tree contents for a given shape
  template<typename T,typename Out>
  Out enumerate_all_tree_contents(tree<T>& src,
				  typename tree<T>::pre_order_iterator at,
				  const NodeSelector<T>& s,Out out) {
    typedef typename NodeSelector<T>::node_iterator ns_iter;

    //cout << "got " << src << endl;
    assert(at.number_of_children()!=1);

    if (at==src.end()) {
      //cout << "creating " << src << endl;
      *(out++)=src;
      //cout << "created " << endl;
    } else {
      int a=at.number_of_children();
      typename tree<T>::pre_order_iterator nxt=at;
      ++nxt;
      for (ns_iter it=s.begin_with_arity(a);it!=s.end_with_arity(a);++it) {
	*at=*it;
	out=enumerate_all_tree_contents(src,nxt,s,out);
      }

      if (a==0) {
	for (ns_iter tp=s.begin_with_arity(1);tp!=s.end_with_arity(1);++tp) {
	  typename tree<T>::pre_order_iterator tmp=src.insert_above(at,*tp);
	  for (ns_iter it=s.begin_with_arity(0);it!=s.end_with_arity(a);++it) {
	    *at=*it;
	    out=enumerate_all_tree_contents(src,nxt,s,out);
	  }
	  at=src.erase(src.flatten(tmp));
	  //cout << "now " << src << endl;
	}
      }
    }
    return out;
  }

  template<typename T,typename Out>
  Out enumerate_all_tree_contents(tree<T>& src,const NodeSelector<T>& s,
				  Out out) {
    return enumerate_all_tree_contents(src,src.begin(),s,out);
  }

  //all actual trees
  template<typename T,typename Out>
  Out enumerate_all_binary_trees(int sz,const NodeSelector<T>& s,Out out) {
    using namespace std;
    typedef tree<T> Tree;
    //start with all shapes
    vector<Tree> all_shapes(catalan(sz));
    enumerate_all_binary_tree_shapes(sz,T(),all_shapes.begin());
    //and output all possible trees involving them
    for (typename vector<Tree>::iterator shape=all_shapes.begin();
	 shape!=all_shapes.end();++shape)
      out=enumerate_all_tree_contents(*shape,s,out);
    return out;
  }

} //~namespace trees







//enumerate_small_trees(minSize,maxSize);


/******
  class enumerate_trees {
  public:
  template<typename T,typename Simplify>
  void enumerate_trees(const NodeSelector<T>& s,const Simplify& simplify,
		       int n,int cutoff,Out out) {
    hash_set<tree<T> > trees;
  }


    void enumerate(int n,iter it) {
      if (n==1) {
	

    

    

  template<typename T>
  class tree_shape_enumerator {
  public:
    tree_shape_enumerator(const NodeSelector<T>& s,int n) : _s(s),_n(n) { }

    template<typename iter>    
    void operator()(tree<T>& tr,iter dst) {
      ++_c;
      if (_n==0) {
	tr.erase_children(dst);
      } else {
	for (TSESeq::iterator it=children.begin();it!=children.end();++it)
	  if (it->

    bool empty()

  template<typename T>
  class tree_enumerator {
  public:
    tree_enumerator(const NodeSelector<T>& s,int n) : _s(s),_n(n),_tse(s,n) { }

    bool empty() const { return (_tse.empty() && _tce.empty()); }

    template<typename iter>
    void operator()(tree<T>& tr,iter it) {
      if (_tce.empty()) {
	_tce.reset();
	_tse(_tce.shape,tce.shape.begin());
      _tce(tr,it);
    }
    

    void enumerate_small_trees(int minSize,int maxSize) {
      for (;minSize<maxSize;++minSize) {
	if (_sample[minSize].size()>=_cutoff) //already full
	  continue;

	//enumerate the trees
	tree_enumerator<T> te(_s,minSize);
	TreeSet trees;
	while (!te.empty()) {
	  tree<T> tr(T());
	  te(tr,tr.begin());
	  _simplify(tr);
	  if (complexity(tr)==minSize) {
	    trees.insert(tr);
	    if (trees.size()>_cutoff) //gotta bail!
	      return;
	  }
	}

	//update the sample
	_sample[minSize].resize(trees.size());
	copy(trees.begin(),trees.end(),_sample[minSize].begin());
      }
    }
	    _sample[minSize-1].clear()
	for (int i=0;i<_cutoff && !te.empty();
	  tree<T> tmp=*it++;
***/

#endif
