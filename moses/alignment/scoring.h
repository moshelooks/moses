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

#ifndef SCORING_H
#define SCORING_H

#include <ext/hash_map>
#include <map>
#include <list>
#include <utility>
#include "tree.h"
#include "basic_types.h"
#include "math_util.h"
#include "hash_util.h"
#include "selection.h"

#include "ProtoVertex.h"

namespace alignment {
  using namespace std;
  using namespace boost;
  using namespace __gnu_cxx;
  using namespace id;

struct uglyugly {
	template<typename T>
	int operator()(const T&) const { assert(false); return 0; }
};

  class Scorer {
  public:
    typedef score_t result_type;
    typedef ptree::pre_order_iterator pre_it;
    typedef ptree::sibling_iterator sib_it;
    typedef pair<pre_it,pre_it> match_pair;
    typedef pair<pair<sib_it,sib_it>,pair<sib_it,sib_it> > range_pair;
    typedef hash_map<match_pair,score_t,
	uglyugly> score_map;
    typedef hash_map<range_pair,score_t,
		     uglyugly > range_score_map;

    static const score_t descent_penalty=1;

    //members
    Scorer(score_t noise) : _noise(noise) { }

    score_t operator()(pre_it l,pre_it r) const { 
      return score(make_pair(l,r)); 
    }
    score_t operator()(const ptree& l,const ptree& r) const {
      return score(make_pair(l.begin(),r.begin())) ;
    }
    score_t operator()(const vtree& l,const vtree& r) const {
      if (l.empty() || r.empty())
	return score_t(0);
      //this is kinda ugly...
      ptree lp,rp;
      vtree2ptree(l,lp);
      vtree2ptree(r,rp);
      score_t sc=score(make_pair(lp.begin(),rp.begin()));
      clear();
      return sc;
    }

    score_t score(pre_it l,pre_it r) const { return score(make_pair(l,r)); }
    score_t score(sib_it l1,sib_it l2,sib_it r1,sib_it r2) const { 
      return score(make_pair(make_pair(l1,l2),make_pair(r1,r2)));
    }

    score_t score(const match_pair&) const;
    score_t score(const range_pair&) const;

    void clear() const { _map.clear(); _rangeMap.clear(); }


    score_t compute_score(sib_it l,sib_it r) const;
  private:
    const score_t _noise;
    mutable score_map _map;
    mutable range_score_map _rangeMap;

    score_t exact_match(sib_it l,sib_it r) const;
    score_t exact_match(sib_it l1,sib_it l2,sib_it r1,sib_it r2) const;
    score_t ordered_match(range_pair p) const;
    score_t unordered_match(sib_it l1,sib_it l2,sib_it r1,sib_it r2) const;
  };

  typedef Scorer::match_pair match_pair;
  typedef Scorer::range_pair range_pair;
  typedef Scorer::score_map score_map;

  class merge_trees {
  public:
    typedef Scorer::sib_it sib_it;

    merge_trees(ptree& src,ptree& dst,Scorer& sc) : _dst(dst),_sc(sc) { 
      merge(src.begin(),dst.begin()); 
      src.clear();
    }
  private:
    void unordered_merge(sib_it l1,sib_it l2,sib_it r1,sib_it r2);
    void ordered_merge(sib_it l,sib_it r);

    ptree& _dst;
    Scorer& _sc;

    void merge(sib_it,sib_it);
  };

  template<typename iter>
  int count(iter it) {
    typedef typename tree<typename iter::value_type>::sibling_iterator sib_it;
    if (it.number_of_children()==0)
      return 1;
    int sz=0;
    for (sib_it child=it.begin();child!=it.end();++child)
      sz+=count(child);
    return sz;
    /**
    int sz=(is_progn(*it) && it.begin()!=it.end()) ? 0 : 1;
    for (sib_it child=it.begin();child!=it.end();++child)
      sz+=count(child);
    return sz;
    **/
  }

  //this should be replaced by a generic algorithm
  //parwise scoring a set of trees approximated via subsampling
  template<typename It>
  void sample_scores(It from,It to,float noise,int density,score_map& scores) {
#if 0
    assert(density==distance(from,to));
    Scorer sc(noise);
    for (It tr1=from;tr1!=to;++tr1) {
      for (It tr2=from;tr2!=tr1;++tr2) {
	scores.insert(make_pair(make_pair(tr1->begin(),tr2->begin()),
				sc.compute_score(tr1->begin(),tr2->begin())));
	sc.clear();
      }
    }
#endif    

    density=min(density,distance(from,to)-1);
    for (It tr=from;tr!=to;++tr) {
      Selector select(distance(from,to));
      for (int i=0;i<density && !select.empty();++i) {
	int n;
	match_pair match;
	do {
	  n=select();
	  match=make_pair(tr->begin(),(from+n)->begin());
	} while ((n==distance(from,tr) || scores.find(match)!=scores.end() ||
		  scores.find(switch_pair(match))!=scores.end()) &&
		 !select.empty());
	if (n==distance(from,tr) || scores.find(match)!=scores.end() ||
	    scores.find(switch_pair(match))!=scores.end())
	  break;
	//note that performance could be improved by caching these Scorer 
	//objects both within and across generations
	/*cout << "inserting " << ptree(match.first) << " " <<
	  ptree(match.second) << endl;*/

	//float sc=2.0*Scorer(noise).score(match)/
	//(score_t)(count(match.first)+count(match.second));
	//int c1=count(match.first),c2=count(match.second);
	float sc=Scorer(noise).score(match); //((score_t)(c1+c2));
	scores.insert(make_pair(match,sc));
      }
    }
  }

} //~namespace alignment

#endif
