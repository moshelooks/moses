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

#include "scoring.h"
#include "dtree_modeling.h"

#include <vector>
#include <map>
#include <boost/iterator/counting_iterator.hpp>

namespace alignment {
  static const score_t EPSILON=0.001;

  score_t Scorer::score(const match_pair& p) const {
    score_map::const_iterator it=_map.find(p);
    return (it==_map.end() ? _map.insert
	    (make_pair(p,compute_score(p.first,p.second)-
		       _noise*randfloat())).first : 
	    it)->second;
  }
  score_t Scorer::score(const range_pair& p) const {
    if (p.first.first==p.first.second || p.second.first==p.second.second)
      return 0;
    range_score_map::const_iterator it=_rangeMap.find(p);
    return (it==_rangeMap.end() ? _rangeMap.insert
	    (make_pair(p,ordered_match(p)-_noise*randfloat())).first : 
	    it)->second;
  }

  score_t Scorer::compute_score(sib_it l,sib_it r) const {
    static const score_t descent_penalty=1;

    //is_empty case taken out for now...
    score_t s=exact_match(l,r)+descent_penalty;
    if (go_down(*l))
      for (sib_it lc=l.begin();lc!=l.end();++lc)
	s=max(s,score(lc,r));
    if (go_down(*r))
      for (sib_it rc=r.begin();rc!=r.end();++rc)
	s=max(s,score(l,rc));
    return s-descent_penalty;
  }

  score_t Scorer::exact_match(sib_it l,sib_it r) const {
    return (matches(*l,*r) ? 
	    ((l.begin()==l.end() && r.begin()==r.end()) ?
	     1 :
	     (is_commutative(*l) ?
	      unordered_match(l.begin(),l.end(),r.begin(),r.end()) :
	      (is_associative(*l) ?
	       score(l.begin(),l.end(),r.begin(),r.end()) :
	       exact_match(l.begin(),l.end(),r.begin(),r.end())))) :
	    0);
  }

  score_t Scorer::exact_match(sib_it l1,sib_it l2,
			      sib_it r1,sib_it r2) const {
    score_t res=0;
    for (;l1!=l2;++l1,++r1)
      res+=score(l1,r1);
    return res;
  }

  score_t Scorer::ordered_match(range_pair p) const {
    score_t s=score(p.first.first++,p.second.first++);
    s+=score(p);
    --p.first.first;
    s=max(s,score(p));
    ++p.first.first;
    --p.second.first;
    return max(s,score(p));
  }

  score_t Scorer::unordered_match(sib_it l1,sib_it l2,
				  sib_it r1,sib_it r2) const {
    std::vector<sib_it> 
      lmatched(make_counting_iterator(l1),
	       make_counting_iterator(l2)),
      rmatched(make_counting_iterator(r1),
	       make_counting_iterator(r2));
    typedef std::multimap<score_t,pair<int,int>,std::greater<score_t> > 
      ScoreMap;
    ScoreMap scores;
    for (int i=0;l1!=l2;++l1,++i) {
      sib_it r=r1;
      for (int j=0;r!=r2;++r,++j)
	scores.insert(make_pair(score(l1,r),make_pair(i,j)));
    }
        
    score_t s=0;
    int n=0,m=min(lmatched.size(),rmatched.size());
    for (ScoreMap::const_iterator it=scores.begin();
	 it!=scores.end();++it) {
      if (lmatched[it->second.first].node!=NULL &&
	  rmatched[it->second.second].node!=NULL) { //a valid match
	lmatched[it->second.first].node=NULL;
	rmatched[it->second.second].node=NULL;
	s+=it->first;
	if (++n==m)
	  return s;
      }
    }
    return s;
  }

  void merge_trees::unordered_merge(sib_it l1,sib_it l2,sib_it r1,sib_it r2) {
    std::vector<sib_it> 
      lmatched(make_counting_iterator(l1),
	       make_counting_iterator(l2)),
      rmatched(make_counting_iterator(r1),
	       make_counting_iterator(r2));
    typedef std::multimap<score_t,pair<int,int>,std::greater<score_t> > 
      ScoreMap;
    ScoreMap scores;
    sib_it l=l1;
    for (int i=0;l!=l2;++l,++i) {
      sib_it r=r1;
      for (int j=0;r!=r2;++r,++j)
	scores.insert(make_pair(_sc.score(l,r),make_pair(i,j)));
    }
        
    int n=0,m=min(lmatched.size(),rmatched.size());
    for (ScoreMap::const_iterator it=scores.begin();
	 it!=scores.end();++it) {
      if (it->first<=0)
	break;
      if (lmatched[it->second.first].node!=NULL &&
	  rmatched[it->second.second].node!=NULL) { //a valid match

	merge(lmatched[it->second.first],rmatched[it->second.second]);
	lmatched[it->second.first].node=NULL;
	rmatched[it->second.second].node=NULL;

	if (++n==m)
	  break;
      }
    }

    for (std::vector<sib_it>::const_iterator lsib=lmatched.begin();
	 lsib!=lmatched.end();++lsib)
      if (lsib->node!=NULL) //unmatched - add to end of right
	_dst.insert_subtree(r1,*lsib);
  }
  void merge_trees::ordered_merge(sib_it l,sib_it r) {
      score_t best=_sc.score(l,r)-1; //-1 for the alignment of *l with *r;

      sib_it lat=l.begin(),lnxt=++l.begin();
      sib_it rat=r.begin(),rnxt=++r.begin();
      while (lat!=l.end() && rat!=r.end()) {
	score_t s_im=_sc.score(lat,rat);
	score_t s0=s_im+_sc.score(lnxt,l.end(),rnxt,r.end());
	score_t s1=_sc.score(lat,l.end(),rnxt,r.end());
	score_t s2=_sc.score(lnxt,l.end(),rat,r.end());
	if (s_im>0 && s0>=s1 && s0>=s2) {
	  assert(s0>=best-EPSILON);
	  best-=s_im;
	  merge(lat,rat);
	  rat=rnxt++;
	  lat=lnxt++;
	} else if (s1>=s2) {
	  assert(ceil(s1)>=ceil(best)-EPSILON);
	  rat=rnxt++;
	} else {
	  assert(ceil(s2)>=ceil(best)-EPSILON);
	  _dst.move_before(rat,lat);
	  lat=lnxt++;
	}
      }
      _dst.append_children(r,lat,l.end());
  }

  void merge_trees::merge(sib_it l,sib_it r) { 
    r->vlocs.splice(r->vlocs.begin(),l->vlocs);
    if (is_turn(*l)) { 
      assert(is_turn(*r));
      r->v.insert(l->v.begin(),l->v.end()); //jumps too once we have jumps
      /*} else if (is_if_food(*l) || is_identity(*l) || is_near(*l) ||
	       is_pickup(*l) || is_holding(*l) || is_walktowards(*l)) {
      merge(l.begin(),r.begin());
      merge(++l.begin(),++r.begin());*/
    } else if (is_not(*l)) {
      assert(is_not(*r));
      merge(l.begin(),r.begin());
    } else if (is_or(*l) || is_and(*l)) {
      assert(*l==*r);
      unordered_merge(l.begin(),l.end(),r.begin(),r.end());
    } else if (is_progn(*l)) {
      assert(is_progn(*r));
      ordered_merge(l,r);
    } else {
      //if (*l!=*r
      //cout << *l << " " << *r << endl;
      if (*l!=*r)
	r->v.insert(l->v.begin(),l->v.end());
      //assert(*l==*r);
      cout << *l <<" " << *r << endl;
      assert(l.number_of_children()==r.number_of_children());
      for (sib_it sib1=l.begin(),sib2=r.begin();sib1!=l.end();++sib1,++sib2)
	merge(sib1,sib2);
    }
  }

} //~namespace alignment
