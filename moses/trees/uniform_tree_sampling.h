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

#ifndef UNIFORM_TREE_SAMPLING_H
#define UNIFORM_TREE_SAMPLING_H

#include "tree.h"
#include "selection.h"
#include <vector>
#include <ext/numeric>
#include <ext/hash_set>
#include <ext/algorithm>

namespace libs {
  using namespace __gnu_cxx;
  using namespace std;
  using namespace boost;
}

namespace trees {
  //any bigger -> danger of overflow in computations
  static const unsigned int catalan_bigness_bound=11;
  //how many redundant trees to we need to generate in a row before panicing?
  static const unsigned int panic_after=20;
  //what is the absolute largest tree we want to generate
  //static int unsigned global_max_size=100;

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

  template<typename T,typename iter>
  void random_binary_tree(tree<T>& tr,const NodeSelector<T>& s,
			  int n,iter tgt) {
    if (n==0) { //a leaf
      if (s.count_with_arity(1)>0 && randbool()) { //a literal?
	*tgt=s.select_with_arity(1);
	tgt=tr.append_child(tgt);
      }
      *tgt=s.select_with_arity(0);
    } else {    //an internal node
      *tgt=s.select_with_arity(2);
      //cout << "sp" << n-1 << endl;
      int split=select_by_catalan(n-1);
      //cout << "lit" << split << endl;
      tr.append_children(tgt,2);
      random_binary_tree(tr,s,split,tgt.begin());
      random_binary_tree(tr,s,n-1-split,tr.last_child(tgt));
    }
  }
  
  template<typename T>
  void random_binary_tree(tree<T>& tr,const NodeSelector<T>& s,int n) {
    tr.clear();
    tr.set_head(s.select_with_arity(0));
    random_binary_tree(tr,s,n,tr.begin());
  }

  template<typename T>
  int internal_count(const tree<T>& tr) {
    int sz=0;
    for (typename tree<T>::pre_order_iterator it=tr.begin();it!=tr.end();++it)
      if (it.number_of_children()>1)
	sz+=it.number_of_children()-1;
    return sz;
  }

  template<typename T,typename iter,typename Simplify>
  void random_simple_binary_tree(tree<T>& tr,const NodeSelector<T>& s,
				 const Simplify& simplify,int n,iter tgt) {
    if (n==0) { //a leaf
      if (s.count_with_arity(1)>0 && randbool()) { //a literal?
	*tgt=s.select_with_arity(1);
	tgt=tr.append_child(tgt);
      }
      *tgt=s.select_with_arity(0);
    } else {    //an internal node
      tree<T> tmp(*tgt);
      do {
	tmp.erase_children(tmp.begin());
	tmp.append_children(tmp.begin(),2);

	*tmp.begin()=s.select_with_arity(2);

	int split=select_by_catalan(n-1);
	random_simple_binary_tree(tmp,s,simplify,split,tmp.begin().begin());
	random_simple_binary_tree(tmp,s,simplify,n-1-split,
				  tmp.last_child(tmp.begin()));
	simplify(tmp);
      } while (internal_count(tmp)<n);
      tr.move_ontop(tgt,iter(tmp.begin()));
    }
  }

  template<typename T,typename Simplify>
  void random_simple_binary_tree(tree<T>& tr,const NodeSelector<T>& s,
				 const Simplify& simplify,int n) {
    tr.clear();
    tr.set_head(s.select_with_arity(0));
    random_simple_binary_tree(tr,s,simplify,n,tr.begin());
  }


  //should be similar formulations to exploit symmetrical function sets

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

  /*
  template<typename It,typename T>
  void uniform_sample(It from,It to,const NodeSelector<T>& s,
		      int minSize,int maxSize) {
    using namespace __gnu_cxx;
    using namespace std;
    typedef typename iterator_traits<It>::value_type Tree;
    
    for (;minSize<=min(maxSize,catalan_bigness_bound);++minSize) {
      unsigned int n=distance(from,to);
      unsigned int expected=n/(maxSize-minSize+1);
      unsigned long int total=number_of_binary_trees(minSize,s);
      if (total<=2*expected) {
	std::vector<Tree> all(total);
	enumerate_all_binary_trees(minSize,s,all.begin());
	from=random_sample(all.begin(),all.end(),from,from+expected);
      } else
	break;
    }
    
    if (minSize==maxSize)
      while (from!=to)
	random_binary_tree(*from++,s,minSize);
    else
      while (from!=to)
	random_binary_tree(*from++,s,randint(maxSize-minSize)+minSize);
  }
  */

#if 0
  sample(int n,unused) {
    root_shape<T> shape(select_shape(s,n));
    vector<Selector> selectors;
    for (int i=0;i<n;++i)
      selectors.push_back(Selector(unused[i].size()));
    



  }

  //generate m trees of size n
  template<typename T,typename Simplify,typename Out>
  void uniform_sampleX(int m,int n,const NodeSelector<T>& s,
		      const Simplify& simplify,Out out) {
    using namespace libs;
    typedef hash_set<tree<T> > TreeSet;
    typedef vector<TreeSet> TreeSetSeq;
    TreeSetSeq unused(n-1);

    
    
    //enumerate all trees of a given intrinsic complexity until prohibitive
    for (unsigned int i=0;i<trees.size();++i) {
      if (!enumerate_up_to(i+1,s,simplify,inserter(trees[i],trees[i].begin())));
      

      
    
    transform(s.begin_with_arity(0),s.end_with_arity(0),
	      inserter(trees[0],trees[0].begin()),constructor<tree<T> >());
	      
    while (trees[n-1].size()<m) {
      
    }
  }
  
  template<typename It,typename T,typename Simplify>
  void uniform_sample(It from,It to,const NodeSelector<T>& s,
		      int minSize,int maxSize,const Simplify& simplify) {
    using namespace __gnu_cxx;
    using namespace std;
    typedef typename iterator_traits<It>::value_type Tree;

    vector<hash_set<Tree> > trees(maxSize-minSize+1);
    unsigned int total=distance(from,to),nAdded=0;

    int nSampled=0;
    int atSize=minSize;
    for (;atSize<=min(maxSize,catalan_bigness_bound);++atSize) {
      unsigned int expected=(total-nAdded)/(maxSize-atSize+1);
      unsigned long int nWithSize=number_of_binary_trees(atSize,s);
      if (nWithSize<=10*expected) {
	nSampled+=nWithSize;
	vector<Tree> all(nWithSize);
	enumerate_all_binary_trees(atSize,s,all.begin());
	//simplify and remove all duplicate trees
	for_each(all.begin(),all.end(),simplify);
	hash_set<Tree> uniq(all.begin(),all.end());
	all.resize(uniq.size());
	copy(uniq.begin(),uniq.end(),all.begin());
	//insert a random subsection of the trees
	random_shuffle(all.begin(),all.end());
	for (typename vector<Tree>::const_iterator it=all.begin();
	     it!=all.end() && trees[atSize-minSize].size()<=expected;++it) {
	  if (internal_count(*it)==atSize) {
	    trees[atSize-minSize].insert(*it);
	    ++nAdded;
	    cout << "ADDED " << *it << endl;
	  }
	}
      } else {
	break;
      }
    }

    cout << "middle.." << endl;
    
    int panic_level=0;
    int samplingLevel=maxSize;
    unsigned int expected=(total-nAdded)/(maxSize-atSize+1);
    while (nAdded<total) {
      Tree tmp;
      random_binary_tree(tmp,s,samplingLevel);
      simplify(tmp);
      int count=internal_count(tmp);
      if (count<atSize) {
	++panic_level;
      } else {
	panic_level=0;
	if (count>maxSize)
	  cout << "tooBig!" << endl;
	if (count<=maxSize && trees[count-minSize].size()<=expected &&
	    trees[count-minSize].insert(tmp).second) {
	  ++nAdded;
	  cout << "ADDED " << tmp << endl;
	  if (count==atSize && trees[atSize-minSize].size()>expected)
	    ++atSize;
	}
      }
      if (samplingLevel<global_max_size && panic_level>panic_after) {
	cout << "panic@thedisco! " << samplingLevel << " " 
	     << atSize << " " << maxSize << endl;
	++samplingLevel;
	panic_level=0;
      }
      ++nSampled;
    }
    cout << "nSampled: " << nSampled << endl;

    for (typename vector<hash_set<Tree> >::const_iterator it=trees.begin();
	 it!=trees.end();++it)
      from=copy(it->begin(),it->end(),from);
  }
#endif
} //~namespace trees

#endif
