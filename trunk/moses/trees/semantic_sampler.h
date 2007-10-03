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

#ifndef SEMANTIC_SAMPLER_H
#define SEMANTIC_SAMPLER_H

#include <iterator>
#include <ext/hash_set>
#include <vector>
#include <boost/bind.hpp>
#include <boost/iterator/indirect_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>

#include "hash_util.h"
#include "io_util.h"
#include "tree.h"
#include "selection.h"
#include <boost/iterator/counting_iterator.hpp>

/*#include "tree_enumeration.h"*/ 
#include "uniform_tree_sampling.h"
#include "tree_util.h"

#include "TreeComp.h"
#include "id.h"
#include "Vertex.h"
#include <set>

namespace trees {
  using namespace std;
  using namespace __gnu_cxx;
  using namespace boost;
  
  template<typename T>
  struct root_shape {
    root_shape(const T& t) : root(t) { }
    root_shape(const T& t,int n0,int n1) : root(t),sizes(2) { 
      (*const_cast<std::vector<int>* >(&sizes))[0]=n0;
      (*const_cast<std::vector<int>* >(&sizes))[1]=n1;
    }
    template<typename It>
    root_shape(const T& t,It from,It to) : root(t),sizes(from,to) { }

    int arity() const { return sizes.size(); }
    
    T root;
    std::vector<int> sizes;
  };
}

template<typename T>
std::ostream& operator<<(std::ostream& out,const trees::root_shape<T>& rs) {
  return (out << rs.root << "(" << rs.sizes << ")");
}

namespace trees {

  template<typename T>
  root_shape<T> select_shape(const NodeSelector<T>& s,int n) {
    if (n==0)
      return root_shape<T>(s.select_with_arity(0));
    int split=select_by_catalan(n-1);
    return root_shape<T>(s.select_with_arity(2),split,n-1-split);
  }

  struct treehashhack {
    size_t operator()(const vtree& tr) const { 
      return boost::hash_range(tr.begin(),tr.end());
    }
  };

  template<typename T,typename Simplify>
  class semantic_sampler {
  public:
    typedef tree<T> Tree;
    typedef typename Tree::sibling_iterator sib_it;
    typedef hash_set<Tree,treehashhack> TreeSet;
    typedef vector<Tree> TreeSeq;
    struct Repository : public vector<TreeSeq> {
      typedef typename vector<TreeSeq>::iterator iterator;
      typedef typename vector<TreeSeq>::const_iterator const_iterator;

      Repository(int sz=0) : vector<TreeSeq>(sz) { }

      //could be made faster by using move_ontop - same with copying uniq (?)
      void splice(Repository& r) {
	assert(r.size()==vector<TreeSeq>::size());
	for (iterator src=r.begin(),dst=vector<TreeSeq>::begin();
	     dst!=vector<TreeSeq>::end();++src,++dst) {
	  dst->insert(dst->end(),src->begin(),src->end());

	  //dst->insert(dst->end(),src->size(),Tree());
	  //copy(src->begin(),src->end(),dst->end()-src->size());
	  /*for_each(src->begin(),src->end(),dst->end()-src->size(),
		   bind(&Tree::move_ontop,
		   bind(&Tree::begin,_2),bind(&Tree::begin,_1)));*/
	  //cout << distance(begin(),dst) << " " << dst->size() << endl;
	}
      }
      void splice(int n,sib_it it) {
	TreeSeq& v((*this)[n]);
	v.push_back(Tree(*it));
	v.back().move_ontop(sib_it(v.back().begin()),it);
      }
    };
    
    semantic_sampler(const NodeSelector<T>& s,
		     const Simplify& simplify=Simplify(),
		     int c=10000)
      : _s(s),_simplify(simplify),_cutoff(c) { }
    
    void uniform_sample(unsigned int maxSize,unsigned int m) {
      assert(m<_cutoff);
      if (maxSize+1>_full.size()) {
	_full.resize(maxSize+1);
	_repository.resize(maxSize+1);
      }
      /*for_each(_repository.begin(),_repository.end(),
	bind(&Repository::reserve,_1,m));*/
      unsigned int minSize=enumerate_small_trees(maxSize);
      for (int i=0;i<minSize;++i)
	random_shuffle(_repository[i].begin(),_repository[i].end());
      for (;maxSize>=minSize;--maxSize) { //sample trees until we have enough
	while (_repository[maxSize].size()<m) {
	  generate_tree(maxSize);
	}
      }
    }

    const TreeSeq& trees(int i) const { return _repository[i]; }
    TreeSeq& trees(int i) { return _repository[i]; }

    int cutoff() const { return _cutoff; }

    bool isFull(int i) const { return _full[i]; }

    void generate_tree(int n) {
      Repository unused(_repository.size());
      
      while (true) {
	root_shape<T> shape(select_shape(_s,n));
	tree<T> tr(shape.root);
	for (vector<int>::const_iterator sz=shape.sizes.begin();
	     sz!=shape.sizes.end();++sz)
	  draw(*sz,tr,tr.append_child(tr.begin()));

	if (minimal(n,tr)) {
	  _repository.splice(unused);
	  _repository.splice(n,tr.begin());
	  return;
	}
	for (vector<int>::const_iterator sz=shape.sizes.begin();
	     sz!=shape.sizes.end();++sz)
	  if (!_full[*sz]) //splice out and for later
	    unused.splice(*sz,tr.begin().begin());
	  else
	    tr.erase(tr.begin().begin());
	assert(tr.begin().number_of_children()==0);
      }
    }
  private:
    const NodeSelector<T>& _s;
    Simplify _simplify;
    const unsigned int _cutoff;
    vector<bool> _full;
    Repository _repository;

    int enumerate_small_trees(unsigned int maxSize) {
      unsigned int minSize=0;
      while (_full[minSize])
	++minSize;
      for (;minSize<=min(maxSize,catalan_bigness_bound);++minSize) {
	unsigned long int nWithSize=number_of_binary_trees(minSize,_s);
	if (nWithSize>10*_cutoff)
	  break;
	
	TreeSeq all(nWithSize);
	enumerate_all_binary_trees(minSize,_s,all.begin());
	//simplify and remove all duplicate trees
	for_each(all.begin(),all.end(),_simplify);

	TreeSet uniq;
	for (typename TreeSeq::const_iterator it=all.begin();
	     it!=all.end();++it)
	  if (complexity(it->begin())==minSize)
	    uniq.insert(*it);

	/*
	for (TreeSet::const_iterator it=uniq.begin();it!=uniq.end();++it)
	  if (complexity(it->begin())>minSize) {
	    cout << *it << endl;
	    assert(false);
	    }*/
	
	if (uniq.size()<=_cutoff) {
	  _repository[minSize].resize(uniq.size());

	  copy(uniq.begin(),uniq.end(),_repository[minSize].begin());
	  _full[minSize]=true;
	  //cout << "created " << _sample[minSize].size() << endl;
	} else {
	  break;
	}
      }
      //cout << "pop" << endl;
      return min(minSize,maxSize);
      /*for (;minSize<maxSize;++minSize) {
	if (_sample[minSize].size()>=_cutoff || _full[minSize]) //already full
	  continue;
	
	//fix based on how enumerate_trees is implemented
	TreeSet tmp;
	tmp.reserve(_cutoff);
	if (!enumerate_trees(_s,_simplify,minSize,_cutoff,back_inserter(tmp)))
	  break;
	
	//update the sample
	swap(tmp,_sample[minSize]);
	_full[minSize]=true;
	}*/
    }

    /*
    place randtree(int n) {
      place tmp=_sample[n].begin();
      advance(tmp,randint(_sample[n].size()));
      return tmp;
    }
    place sample(int n) {
      while (!_full[n] && _sample[n].size()<_m)
	generate_tree(n);
      //cout << "blarg" << endl;
      return randtree(n);
    }
    void release(int n,place p) { 
      if (!_full[n]) 
	_sample[n].erase(p); 
    }
    */

    //splice a tree out of the repository (if available) else create one
    //dst gets invalidated
    void draw(int n,Tree& tr,sib_it dst) { 
      if (_full[n]) {
	assert(_repository[n].size()>0);
	int idx=randint(_repository[n].size());
	tr.replace(dst,_repository[n][idx].begin());
      } else {
	if (_repository[n].empty())
	  generate_tree(n);
	tr.move_ontop(dst,sib_it(_repository[n].back().begin()));
	_repository[n].pop_back();
      }
    }

    bool minimal(int n,tree<T>& tr) {
      tree<T> test(tr);
      _simplify(test);
      int k=complexity(test.begin());
      assert(k<=n);
      if (k==n) {
	swap(test,tr);
	return true;
      }
      return false;
    }
      
#if 0
	
	vector<tree<T> > subtrees(shape.arity());
	for (unsigned int i=0;i<subtrees.size();++i) {
	subtrees[i]=*sample(shape.sizes[i]);
      /*transform(shape.sizes.begin(),shape.sizes.end(),subtrees.begin(),
      bind(&semantic_sampler<T,Simplify>::sample,this,_1));*/
      
      //cout << "puke" << endl;
      for (typename vector<tree<T> >::const_iterator it=subtrees.begin();
	   it!=subtrees.end();++it)
	tmp.append_child(tmp.begin(),it->begin());
      int maxSize=(n==0 ? -1 :
		   *max_element(shape.sizes.begin(),shape.sizes.end()));
      //cout << "mook" << tmp << endl;
      _simplify(tmp);

      int k=complexity(tmp.begin());

      if (k>n) {
	//cout << "XXX " << k << "!" << n << " " << tmp << endl;
	exit(84);
      }

      if (k>maxSize) { //worth keeping
	//cout << "book" << k << endl;
	if (_sample[k].insert(tmp).second) {
	  if (k==n) { //done!
	    cout << "success " << n << endl;
	    return;
	  } else {
	    cout << "Xccess " << k << endl;
	  }
	}
      }
      
      for (unsigned int i=0;i<subtrees.size();++i)
	_sample[shape.sizes[i]].insert(subtrees[i]); //put em back
		
      //cout << "failure " << n << " - " << k << endl;
      generate_tree(n); //try again
    }
#endif
  };

  template<typename T,typename Simplify,typename Out>
  Out enumerate_minimal_trees(int k,const NodeSelector<T>& sel,
			      const Simplify& simplify,Out out) {
    typedef typename semantic_sampler<T,Simplify>::TreeSeq TreeSeq;

    set<tree<T>,trees::SubtreeOrder<T> > tmp;
    semantic_sampler<T,Simplify> ss(sel,simplify);
    ss.uniform_sample(k,ss.cutoff()-1);
    for (int i=0;i<k;++i) {
      for (typename TreeSeq::const_iterator it=ss.trees(i).begin();
	   it!=ss.trees(i).end();++it) {
	tree<T> neg(id::create_not<T>());
	neg.append_child(neg.begin(),it->begin());
	simplify(neg);
	if (tmp.find(neg)==tmp.end())
	  tmp.insert(*it);
      }
    }
    return copy(tmp.begin(),tmp.end(),out);
  }
} //~namespace trees
      
#endif
