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

#ifndef MAKE_PROTOTYPES_H
#define MAKE_PROTOTYPES_H

#include <queue>
#include "func_util.h"

#include "scoring.h"

namespace alignment {

  //overall orchestrator function that takes in vtrees and builds ptrees
  template<typename It,typename Out>
  void make_prototypes(It from,It to,float noise,int density,Out out);

  //prototype tree creation - trees are aligned via greedy agglomeration
  class prototype {
  public:
    typedef Scorer::pre_it pre_it;

    template<typename It>
    prototype(It from,It to,float noise,score_map& scores);
    
  private:
    struct Datum {
      Datum(ptree& tr) : tr_ptr(&tr),initial_count(count(tr.begin())),
			 current_count(initial_count),current_clust(tr.begin())
      { }
						   
      ptree* tr_ptr;
      int initial_count;
      int current_count;
      pre_it current_clust;
    };
    class ClustMap : public map<pre_it,Datum> {
    public:
      typedef map<pre_it,Datum> super;
      iterator find(const pre_it& it) {
	iterator it1=super::find(it);
	while (it1->first!=it1->second.current_clust) {
	  iterator tmp=super::find(it1->second.current_clust);
	  it1->second.current_clust=tmp->second.current_clust;
	  it1->second.current_count=tmp->second.current_count;
	  it1=tmp;
	}
	return it1;
      }
    };
  };
    
  template<typename It>
  prototype::prototype(It from,It to,float noise,score_map& scores) {
    //map trees to cluster info
    ClustMap trees2Clusters;
    for (It it=from;it!=to;++it)
      trees2Clusters.insert(make_pair(it->begin(),Datum(*it)));
    
    //compute normalized scores
    priority_queue<pair<score_t,match_pair> > q;
    for (score_map::const_iterator it=scores.begin();it!=scores.end();++it) {
      if (it->second>0) {
	score_t sc=it->second/(score_t)
	  (trees2Clusters.find(it->first.first)->second.current_count+
	   trees2Clusters.find(it->first.second)->second.current_count);
	q.push(pair<score_t,match_pair>(sc,it->first));
      }
    }
    scores.clear(); //don't need it anymore

    while (!q.empty()) {
      Datum& src=trees2Clusters.find(q.top().second.first)->second;
      Datum& dst=trees2Clusters.find(q.top().second.second)->second;

      Scorer sc(noise);
      if (src.current_clust!=dst.current_clust &&
	  (src.current_clust->vlocs.size()==1 ||
	   dst.current_clust->vlocs.size()==1 ||
	   true)) {
	   
	   /*3*ceil(q.top().first*(src.initial_count+dst.initial_count))>=
	     src.initial_count+dst.initial_count)) {*/

	   /*(min(src.current_count,dst.current_count)-
	    ceil(q.top().first*(src.initial_count+dst.initial_count))<=2))) {
	   */
	cout << "considering " << ptree(src.current_clust) 
	     << " | "   << ptree(dst.current_clust) << endl;
	cout << sc(src.current_clust,dst.current_clust) << endl;
	cout << src.current_count << " " << dst.current_count << endl;

	merge_trees(*trees2Clusters.find(src.current_clust)->second.tr_ptr,
		    *trees2Clusters.find(dst.current_clust)->second.tr_ptr,sc);
	src.current_clust=dst.current_clust;
	dst.current_count=count(dst.current_clust); //recompute count
	dst.current_count=dst.current_count;
      }/* else {
       cout << "rejecting " <<  ptree(src.current_clust) 
       << " | "   << ptree(dst.current_clust) << endl;
	}*/
      q.pop();
      cout << "popped..." << q.size() << endl;
    }
    cout << "doon" << endl;
  }

  template<typename It,typename Out>
  void make_prototypes(It from,It to,float noise,int density,Out out) {
    hash_map<vtree,list<int> > uniq;
    for (int i=0;from!=to;++from,++i) {
      uniq[*from].push_back(i);
      //uniq[*from].clear();
    }
    vector<ptree> trees(uniq.size());
    for_each(uniq.begin(),uniq.end(),trees.begin(),constructor<vtree2ptree>());
    //we now have a ptree for every unique vtree

    score_map scores;
    sample_scores(trees.begin(),trees.end(),noise,density,scores);
    prototype(trees.begin(),trees.end(),noise,scores);

    for (vector<ptree>::iterator it=trees.begin();it!=trees.end();++it)
      if (!it->empty()) {
	reduce_vlocs(*it);
	*out++=*it;
      }
  }

} //~namespace alignment

#endif
