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

#include "dtree_learn.h"
#include "BDe.h"
#include "func_util.h"

namespace modeling {
  static const double EPSILON(1e-5);

  struct Potential {
    Potential(gain_t gain1,Split s,sib_iterator leaf1,int tgt1,int src1)
      : gain(gain1),splt(s),leaf(leaf1),tgt(tgt1),src(src1) { }
    
    sib_iterator execute(Graph& g) const { 
      g.dependance(src,tgt);
     
      //return split(splt,boost::get<BoolDTree>(g[src]).src(),leaf,g[tgt]);
      return split_on_dtree(splt,g[src],leaf,g[tgt]);
      //return leaf;
    }

    gain_t gain;
    Split splt;
    sib_iterator leaf;
    int tgt,src;
  };

  bool operator<(const Potential& a,const Potential& b) {
    return a.gain<b.gain;
  }
  typedef list<int> VSeq;
  typedef priority_queue<Potential> PQ;

  bool operator<(const PQ& a,const PQ& b) {
    return (a.empty() ? true : (b.empty() ? false : a.top()<b.top()));
  }
  typedef priority_queue<PQ> PQQ;

  class ScoreDirector : public static_visitor<pair<gain_t,SplitV> > {
  public:
    typedef pair<gain_t,SplitV> Result;
    ScoreDirector(sib_iterator tgt_) : tgt(tgt_) { }
    sib_iterator tgt;

    Result operator()(const BoolDTree& tgt_dt,const BoolDTree& src_dt) {
      return Result(BDe::binary_binary(perm1(tgt_dt),perm2(tgt_dt),
				       perm1(src_dt)),true);
    }
    Result operator()(const BoolDTree& tgt_dt,const DiscDTree& src_dt) {
      gain_t g1=BDe::disc_disc_nway(perm1(tgt_dt),perm2(tgt_dt),2,
				     perm1(src_dt),cardinality(src_dt));

      IntSet is(cardinality(src_dt),false);
      gain_t g2=-1;
      /*gain_t g2=BDe::disc_disc_2way
	(perm1(tgt_dt),perm2(tgt_dt),2,
	perm1(src_dt),cardinality(src_dt),is);
	//perm1(src_dt),cardinality(src_dt),inserter(is));*/

      return g2>g1 ? Result(g2,is) : Result(g1,false);
    }
    Result operator()(const BoolDTree& tgt_dt,const ContinDTree& src_dt) {
      return BDe::disc_contin_2way
        (perm1(tgt_dt),perm2(tgt_dt),2,perm1(src_dt));
    }

    Result operator()(const DiscDTree& tgt_dt,const BoolDTree& src_dt) {
      return Result
	(BDe::disc_disc_nway(perm1(tgt_dt),perm2(tgt_dt),cardinality(tgt_dt),
			     perm1(src_dt),2),true);
    }
    Result operator()(const DiscDTree& tgt_dt,const DiscDTree& src_dt) {
      gain_t g1=BDe::disc_disc_nway
	(perm1(tgt_dt),perm2(tgt_dt),cardinality(tgt_dt),
	 perm1(src_dt),cardinality(src_dt));

      IntSet is(cardinality(src_dt),false);
      gain_t g2=-1;
      /*gain_t g2=BDe::disc_disc_2way
	(perm1(tgt_dt),perm2(tgt_dt),cardinality(tgt_dt),
	perm1(src_dt),cardinality(src_dt),is);*/
      //perm1(src_dt),cardinality(src_dt),inserter(is));
      //cout << "score1 " << g1 << " score2 " << g2 << endl;
      return g2>g1 ? Result(g2,is) : Result(g1,false);
    }
    Result operator()(const DiscDTree& tgt_dt,const ContinDTree& src_dt) {
      return BDe::disc_contin_2way
	(perm1(tgt_dt),perm2(tgt_dt),cardinality(tgt_dt),perm1(src_dt));
    }

    Result operator()(const ContinDTree& tgt_dt,const BoolDTree& src_dt) {
      return Result(BDe::contin_disc_nway(perm1(tgt_dt),perm2(tgt_dt),
					  perm1(src_dt),2),true);
    }
    Result operator()(const ContinDTree& tgt_dt,const DiscDTree& src_dt) {
      //gain_t g1=-1;
      assert(false);
      return Result(BDe::contin_disc_nway(perm1(tgt_dt),perm2(tgt_dt),
					  perm1(src_dt),cardinality(src_dt)),
		    false);
      /*IntSet is(0);
      gain_t g2=BDe::contin_disc_2way
	(perm1(tgt_dt),perm2(tgt_dt),
	 perm1(src_dt),cardinality(src_dt),is);
      //perm1(src_dt),cardinality(src_dt),inserter(is));
      return g2>g1 ? Result(g2,is) : Result(g1,false);*/
    }
    Result operator()(const ContinDTree& tgt_dt,const ContinDTree& src_dt) {
      return BDe::contin_contin_2way
	(perm1(tgt_dt),perm2(tgt_dt),perm1(src_dt));
    }
  private:
    int cardinality(const DiscDTree& dt) {
      return modeling::hack_arity;
      //return (*max_element(dt.src(),dt.src()+dt.perm().size())+1);
      /**
      if (c==3)
	cout << "neeble" << endl;
      else
	cout << "poop " << c << endl;
	return c;**/
      //return (*max_element(dt.src(),dt.src()+dt.perm().size())+1);
    }
    
    template<typename T>
    permutation_iterator<T,perm_iterator>
    perm1(const DTree<T>& t) { 
      return make_permutation_iterator(t.src(),boost::get<Leaf>(*tgt).first);
    }
    template<typename T>
    permutation_iterator<T,perm_iterator>
    perm2(const DTree<T>& t) { 
      return make_permutation_iterator(t.src(),boost::get<Leaf>(*tgt).second);
    }
    
  };

  PQ score_leaf(const Graph& g,int tgt,sib_iterator tgt_leaf) {
    PQ q;
    if (distance(boost::get<Leaf>(*tgt_leaf).first,
		 boost::get<Leaf>(*tgt_leaf).second)<=1)
      return q;
    ScoreDirector sd(tgt_leaf);

    for (int src=0;src<(int)g.size();++src) {
      if (src!=tgt && !g(tgt,src)) {
	ScoreDirector::Result res=apply_visitor(sd,g[tgt],g[src]);
	if (res.first>EPSILON) {
	  q.push(Potential(res.first,Split(res.second,src),tgt_leaf,tgt,src));
	}
      }
    }
    return q;
  }

  template<typename Out>
  void score_tree(const Graph& g,int tgt,Out out) {
    const Tree& tr=get_tree(g[tgt]);
    for (Tree::sibling_iterator it=tr.begin();it!=tr.end();++it)
      if (it.begin()==it.end()) //leaf
	*out++=score_leaf(g,tgt,it);
  }

  bool sweep(PQQ& pqq) {
    if (pqq.empty())
      return false;
    while (pqq.top().empty()) {
      pqq.pop();
      if (pqq.empty())
	return false;
    }
    return true;
  }

  bool ready(PQQ& pqq,const Graph& g) {
    while (sweep(pqq)) {
      if (!g(pqq.top().top().tgt,pqq.top().top().src))
	return true;
      PQ tmp=pqq.top();
      pqq.pop();
      tmp.pop();
      if (!tmp.empty())
	pqq.push(tmp);
    }
    return false;
  }
  
  void dtree_learn(Graph& g) {
    /***/
    //fill the pqq with entries for each vertex
    PQQ pqq;

    for (Graph::const_iterator v=g.begin();v!=g.end();++v)
      score_tree(g,v-g.begin(),inserter(pqq));

    while (ready(pqq,g)) {
      const Potential p=pqq.top().top();
      //clear off the whole q
      pqq.pop();
      //execute the split
      //cout << "split " << p.tgt << " on " << p.src << " " << p.gain << endl;
      sib_iterator result=p.execute(g);
      //create new qs for the children
      for (sib_iterator sib=result.begin();sib!=result.end();++sib)
	pqq.push(score_leaf(g,p.tgt,sib));
    }
    /***/
    
    //order by dependency
    g.order();
  }

} //~namespace modeling
