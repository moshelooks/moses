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

#include "make_empty_model.h"
#include <map>

namespace modeling {

  void make_empty_model::rec_make(pre_it at,pre_it sup,
				  Graph::iterator* sup_dtree) {
    using namespace id;
    
    //assert(at->jumps.size()==0);
    if (at->v.size()==1) {
      if (at->vlocs.size()<sup->vlocs.size()) { //add an expr dtree
	BoolMarginal& m=graph.create_bool_marginal();
	for (VlocSeq::const_iterator it=at->vlocs.begin();
	     it!=at->vlocs.end();++it)
	  m[it->first]=true;

	/*cout << "PD ";
	for (int i=0;i<m.size();++i)
	  cout << (m[i] ? "1" : "0") << " ";
	  cout << endl;*/
	
	graph.push_back(BoolDTree(m.begin(),m.end()));
	if (sup_dtree!=NULL) {
	  split_on_dtree(Split(true,distance(graph.begin(),*sup_dtree)),
			 **sup_dtree,get_tree(graph.back()).begin(),
			 graph.back());
	  graph.structure(distance(graph.begin(),*sup_dtree),graph.size()-1);
	}
	sup=at;
	sup_dtree=&--graph.end();
      }
      for (sib_it child=at.begin();child!=at.end();++child)
	rec_make(child,sup,sup_dtree);
    } else {
      assert(at.number_of_children()==0);
      if (is_turn(*at)) {
	//create two binary dtrees
	BoolMarginal& l=graph.create_bool_marginal();
	BoolMarginal& r=graph.create_bool_marginal();
	for (VlocSeq::const_iterator it=at->vlocs.begin();
	     it!=at->vlocs.end();++it) {
	  l[it->first]=(is_left(it->second) || is_reversal(it->second));
	  r[it->first]=(is_right(it->second) || is_reversal(it->second));
	}
	graph.push_back(BoolDTree(l.begin(),l.end()));
	BoolDTree& ldtree=boost::get<BoolDTree>(graph.back());
	graph.push_back(BoolDTree(r.begin(),r.end()));
	BoolDTree rdtree=boost::get<BoolDTree>(graph.back());
	if (sup_dtree) {
	  split_on_dtree(Split(true,distance(graph.begin(),*sup_dtree)),
			 **sup_dtree,get_tree(ldtree).begin(),ldtree);
	  split_on_dtree(Split(true,distance(graph.begin(),*sup_dtree)),
			 **sup_dtree,get_tree(rdtree).begin(),rdtree);
	  graph.structure(distance(graph.begin(),*sup_dtree),graph.size()-1);
	  graph.structure(distance(graph.begin(),*sup_dtree),graph.size()-2);
	}
      } else {
	//assert(is_variable(*at));
	//create a sequence of n linked boolean dtrees, one for each var
	cout << "Dd" << endl;
	typedef map<Vertex,BoolMarginal*> MMap;
	MMap margs;
	for (VertexSet::const_iterator it=at->v.begin();it!=at->v.end();++it)
	  margs.insert(make_pair(*it,
				 &graph.create_bool_marginal())).first->second;
	for (VlocSeq::const_iterator it=at->vlocs.begin();
	     it!=at->vlocs.end();++it)
	  (*margs[it->second])[it->first]=true;

	const unsigned int fromIdx=graph.size();
	for (MMap::const_iterator marg=margs.begin();
	     marg!=margs.end();++marg) {
	  graph.push_back(BoolDTree(marg->second->begin(),
				    marg->second->end()));
	  BoolDTree& current=boost::get<BoolDTree>(graph.back());

	  Tree::iterator leaf=get_tree(current).begin();

	  if (sup_dtree) {
	    cout << "Xdoing " << distance(graph.begin(),*sup_dtree)
		 << " -> " << graph.size()-1 << endl;
	    leaf=++split_on_dtree
	      (Split(true,distance(graph.begin(),*sup_dtree)),**sup_dtree,
	       get_tree(current).begin(),current).begin();
	    graph.structure(distance(graph.begin(),*sup_dtree),graph.size()-1);
	  }

	  for (unsigned int idx=fromIdx;idx<graph.size()-1;++idx) {
	    cout << "doing " << idx << " -> " << graph.size()-1 << endl;
	    leaf=split_on_dtree(Split(true,idx),graph[idx],
				leaf,current).begin();
	    graph.structure(idx,graph.size()-1);
	    cout << "uf" << endl;
	  }
	}
	cout << "Ddq" << endl;
	for (VlocSeq::const_iterator it=at->vlocs.begin();
	     it!=at->vlocs.end();++it) {
	  assert(boost::get<Argument>(&it->second));
	  /*cout << boost::get<Argument>(it->second).idx << " "
	       << it->first << endl;
	  cout << margs.size() << endl
	  << margs[boost::get<Argument>(it->second).idx-1]->size() << endl;
	  */
	  (*margs[it->second])[it->first]=true;
	}
	/*cout << "Ddc" << endl;
	for (MMap::const_iterator it=margs.begin();it!=margs.end();++it) {
	  for (int i=0;i<it->second->size();++i)
	    cout << ((*(it->second))[i] ? "1" : "0") << " ";
	  cout << endl;
	  }*/
      }
    }
  }

} //~namespace modeling
