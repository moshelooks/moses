// Copyright 2008 Google Inc. All Rights Reserved.
//
// Licensed under the Apache License, Version 2.0 (the "License")
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an AS IS BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
//
// Author: master___chen@163.com (Chen Shuo)

// Main file for building-block hillclimber (BBHC)

#include <fstream>
#include <iostream>
#include <algorithm>
#include <cmath>
#include <string>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "../utils/io_util.h"
#include "../utils/bool_util.h"

#include "../modeling/BDe.h"
#include "../alignment/scoring.h"
#include "../modeling/dtree_learn.h"

#include "../modeling/BB.h"

using namespace boost;
using namespace std;
using namespace modeling;

typedef pair<double,Assignment> fpair;

struct opt_base
{
	opt_base(): _evals(0) {};
	//virtual double operator()(Assigenment& a);
	void inline addEvals() { ++_evals; }
	int evals() const { return _evals; }
	int _evals;
};

struct testBool : public opt_base
{
	testBool(): opt_base() {};
	double operator()(Assignment& a)
	  { opt_base::addEvals();
		return count(a.begin(), a.end(), Value(true)); }
};

struct testInt : public opt_base
{
	testInt(): opt_base() {}; 
	double operator()(Assignment& a)
	  { opt_base::addEvals();
	    int sum=0;
	    for(Assignment::iterator i=a.begin(); i!=a.end(); ++i) sum+=todisc(*i);
	    return sum; }
    string name() { return "test int"; }
};

struct hiff : public opt_base
{
	hiff(int hLevel): opt_base(), _hLevel(hLevel), _length(exp2(hLevel)){};
	int _hLevel, _length;
	
	double operator()(Assignment& a)
	  { assert(a.size()==_length); 
	    opt_base::addEvals();
	    return H(a.begin(), a.end()); }
	double H(Assignment::iterator first, Assignment::iterator last) const
	  { if ( first+1==last ) return h(first,last);
	    Assignment::iterator mid=first+(last-first)/2;
		return H(first, mid) + H(mid, last) // Hiff(L)+Hiff(R)
		       +h(first, last)*(last-first); }// hiff(x) * length
	double h(Assignment::iterator first, Assignment::iterator last) const
	  { if ( first+1==last ) return 1;
	    Assignment::iterator mid=first+(last-first)/2;
		if ( std::mismatch(first, mid, mid).first==mid 
				&& h(first, mid)==1 && h(mid, last)==1) return 1;
		return 0; }
	bool isExp2(int length) const
	  { while (length>1 && length%2==0) length=length/2; 
	    if (length==1) return true; else return false; }
	int exp2(int t) const
	  { int n=1; for (int i=0; i<t; ++i) n*=2; 
	    return n; }
	string name() { return "HIFF"; }
};

struct hxor : public opt_base
{
	hxor(int hLevel): opt_base(), _hLevel(hLevel), _length(exp2(hLevel)){};
	  int _hLevel, _length;
		
	double operator()(Assignment& a)
	  { assert(a.size()==_length); 
	    opt_base::addEvals();
	    return H(a.begin(), a.end()); }
	double H(Assignment::iterator first, Assignment::iterator last) const
	  { if ( first+1==last ) return h(first,last);
	    Assignment::iterator mid=first+(last-first)/2;
		return H(first, mid) + H(mid, last) // Hiff(L)+Hiff(R)
		       +h(first, last)*(last-first); }// hiff(x) * length
	template <typename valueType>
	struct _xor      
	{ bool operator()(valueType a, valueType b) const 
		{return (boost::get<bool>(a)==!boost::get<bool>(b));} };
	double h(Assignment::iterator first, Assignment::iterator last) const
	  { if ( first+1==last ) return 1;
	    Assignment::iterator mid=first+(last-first)/2;
		if ( std::mismatch(first, mid, mid, _xor<Value>()).first==mid 
		    	&& h(first, mid)==1 && h(mid, last)==1) return 1;
		return 0;
	  }
	int exp2(int t) const
	  { int n=1; for (int i=0; i<t; ++i) n*=2; 
	    return n; }
	string name() { return "HXOR"; }
};

struct htrap3 : public opt_base
{
	typedef pair<int, int> node_type; //
	typedef vector<node_type> level_type; //
	htrap3(int hLevel)
	  : _hLevel(hLevel), _length(exp3(hLevel)), _structure(0)
	  { for(int p=0; p<=_hLevel; ++p) _structure.push_back(level_type(exp3(_hLevel-p)) ); }
	int _hLevel, _length;
	vector<level_type > _structure;
	
	double operator()(Assignment& a)
	  { opt_base::addEvals();
		makeStruct(a); 
		return HT();
	  }
	void makeStruct(Assignment& a)
	  { assert(a.size()==_length);
		for (int i=0; i<_length; ++i) _structure[0][i].first=int(tobool(a[i]));   
        for (int p=1; p<=_hLevel; ++p) for (int j=0; j<_structure[p].size(); ++j)
          _structure[p][j]=h(_structure[p-1].begin()+j*3);	
	  }
	double HT()
	  { double res=0;
		for (int p=1; p<=_hLevel; ++p) 
	      { int len = exp3(p);
		    for (level_type::iterator it=_structure[p].begin(); it!=_structure[p].end(); ++it)
			  res += len * trap(it->second, 1.0, p==_hLevel?0.9:1.0);
	      }
		return res;
	  }
	struct value_equal
	  { int _value;
	    value_equal(int value): _value(value) {}
		bool operator()(node_type n){return n.first==_value;} };
	node_type h(level_type::iterator first1)
	  { if (first1->first==-1 || (first1+1)->first==-1 || (first1+2)->first==-1)
		  return make_pair(-1, -1);
		if (first1->first==(first1+1)->first && first1->first==(first1+2)->first) 
		  return make_pair(first1->first, count_if(first1, first1+3, value_equal(1)) );
	    else return make_pair(-1, count_if(first1, first1+3, value_equal(1)) ); }
	double trap(int u, double fHigh, double fLow)
	  { assert(u<4);
	    if (u<0) return 0;
		if (u==3) return fHigh;
	    if (u>=0) return fLow*(3-1-u)/(3-1); }
	int exp3(int t) 
	  { int res=1; for(int i=0; i<t; ++i) res*=3; return res; }
	string name() { return "HTRAP"; }
};

template <class eval_type>
struct noise : eval_type
{
	noise(const eval_type& ev, double nr)
	  :eval_type(ev), _nrate(nr) {}	
	double _nrate;
	
	bool byChance(double rate) const { return randdouble()<rate;}
	Assignment makeNoise(Assignment a) const
	  { for (Assignment::iterator i=a.begin(); i!=a.end(); ++i) 
		  if (byChance(_nrate)) *i=!tobool(*i); 
		return a; }
	double operator() (Assignment& a)
	  { Assignment tmp=makeNoise(a);
		return eval_type::operator()(tmp); }
};

//typedef pair<double,Assignment> fpair;
    
const Assignment* t_selection(const vector<fpair>& v) {
  return &(tournament_select(v.begin(),v.end(),2,less<fpair>())->second);
}

vector<fpair>::iterator rtr_select(const Assignment& a,double score,
                                   vector<fpair>& pop,int tsize) {
  assert(tsize>0);
  vector<fpair>::iterator closest;
  if (tsize==1) { //simply do size-2 tournament selection
    closest=pop.begin()+randint(pop.size());
  } else {
    Selector select(pop.size());

    int closestSim=NEG_INFINITY;
    for (int i=0;i<tsize;++i) {
      vector<fpair>::iterator tmp=pop.begin()+select();
      int sim=(-hamming_distance(a.begin(),a.end(),tmp->second.begin()));
      if (sim>closestSim) {
        sim=closestSim;
        closest=tmp;
      }
    }
  }
  return (closest->first<score) ? closest : pop.end();
}

/***************hBOA***************/

typedef vector<fpair>::iterator popIter;

popIter rtr_select(const Assignment& a,double score,
		           popIter first, popIter last, int tsize) {
  assert(tsize>0);
  int pop_size=last-first;
  vector<fpair>::iterator closest;
  if (tsize==1) { //simply do size-2 tournament selection
    closest=first+randint(pop_size);
  } else {
    Selector select(pop_size);

    int closestSim=NEG_INFINITY;
    for (int i=0;i<tsize;++i) {
      vector<fpair>::iterator tmp=first+select();
      int sim=(-hamming_distance(a.begin(), a.end(), tmp->second.begin()));
      if (sim>closestSim) {
        sim=closestSim;
        closest=tmp;
      }
    }
  }
  return (closest->first<score) ? closest : last;
}

template<typename Opt>
void hBOA_once(popIter first, popIter last, Opt& scoring, int length)
{ int pop_size=last-first;
  Graph g(pop_size);  
  g.reserve(length); 
  for (int i=0;i<length;++i) {
	DiscMarginal& d=g.create_disc_marginal();
    assert(d.size()==pop_size);
    DiscMarginal::iterator j=d.begin();
    /**/
    for(popIter a=first; a!=last; ++a)
      { *j=todisc((a->second)[i]);
    	++j;
      } // get a Marginal distribution
    g.push_back(DiscDTree(d.begin(),d.end())); // 
  }
  
  //update the gain offset
  BDe::gainOffset=(log2(gain_t(distance(first,last)))/2.0);
  
  //learn dependencies
  dtree_learn(g); // 
  
  //sample new instances and RTR them into the population
  assert(g.ordering().size()==length);
  for (int i=0;i<pop_size;++i) {  
    Assignment a(length,0);
    generate_assignment(g.ordering().begin(),g.ordering().end(),g,a);
    int rtr_tsize=min(length,int(0.05*double(pop_size)+1));
    double s=scoring(a);
    vector<fpair>::iterator j=rtr_select(a,s,first,last,rtr_tsize);
    if (j!=last) {
      j->first=s;
      j->second=a;
    }
  } 
}

/***************HHBC***************/

template<typename Opt>
struct disc_bool_opt_wrapper
{ disc_bool_opt_wrapper(Opt* bool_opt, BBs bbs)
    : _opt(bool_opt), _bbs(bbs) {}
  Opt* _opt; BBs _bbs;  	
  
  double operator()(Assignment& a)
    { Assignment tmp=toBoolType(a);
      return _opt->operator()(tmp); }
  Assignment toBoolType(Assignment& a)
    { return _bbs.toBoolType(a); }
};

template<typename Opt>
void BBHC(Opt& scoring,int length,int pop_size,int n_gen,double maxscore)
{ BBs bbs(length);			     // model of building blocks
  vector<fpair> to_model(0); // promosing solutions
  vector<fpair> disc_model(0); // transformed to discrete type
  double best=0;
  fpair newState(0.0,Assignment(length,false));

  cout << "BBHC, " << scoring.name() << "~"
       << "  chro length: " << length
       << "  population size: " << pop_size
       << "  n generations: " << n_gen << endl;
  
  for (int gen=1;gen<n_gen;++gen) { 
    
	to_model.clear();
	disc_model.clear();
	
	while (to_model.size()<pop_size ) //&& best<maxscore)  // not full && destination not reached
	  { 
		newState.second=bbs.getRandomState();
		bbs.hillClimb(newState.second, scoring);
		newState.first=scoring(newState.second);
		to_model.push_back(newState);
		best=newState.first;
	  }	

#define _DISABLE_HBOA	
#ifndef _DISABLE_HBOA
	// hBOA 
	
	// transform all to disc type
	for (int i=0; i<pop_size; ++i)
	  disc_model.push_back(make_pair(to_model[i].first, 
			                           bbs.toDiscType(to_model[i].second)) );
	
	disc_bool_opt_wrapper<Opt> disc_scoring(&scoring, bbs);
	hBOA_once(disc_model.begin(), disc_model.end(), 
			  disc_scoring, bbs.blocks());
	
	// tranform back 
	for(int i=0; i<cSize; ++i) 
	  { to_model[i].second=bbs.toBoolType(disc_model[i].second);
	    to_model[i].first=scoring(to_model[i].second);
	    if (to_model[i].first>best) best=to_model[i].first;
	  }	
#endif	
	
	// update BBHC model 		
	//bbs.updateBBsWithConf(to_model.begin(), to_model.end(), 0.9);
	bbs.updateBBs(to_model.begin(), to_model.end()); // to_model.begin()+cSize);
	bbs.updateBBConfigs(to_model.begin(), to_model.end()); // to_model.begin()+cSize);
	
	
	cout << "genration # " << gen 
	     << ", # evals " << scoring.evals()
         << ", best fitness " << max_element(to_model.begin(),to_model.end())->first
         << ", BBs " << bbs.blocks()
         << ", configs " << bbs.bbconfigs()
         << endl ;

	best = max_element(to_model.begin(),to_model.end())->first;
    if (best>=maxscore) break;	   
  }  
  
  cout << endl
       << "max score: "
       << best << endl
       << "best solution: "        
       << max_element(to_model.begin(),to_model.end())->second << endl;  
      
}

/******************END OF HHBC****************/


int main(int argc,char** argv) {
  assert(argc==5);
  srand(lexical_cast<int>(argv[1]));
  int level=lexical_cast<int>(argv[2]);
  int pop_size=lexical_cast<int>(argv[3]);
  int n_gen=lexical_cast<int>(argv[4]);
  

  hiff h(level);
  hxor hx(level);
  htrap3 ht3(level);
  
  /* 16-80; 32-192 ; 64-448 ; 128-1024; 256-2304; 512- 5120; 1024-11264*/
  
  // HIFF 
  BBHC(h, h.exp2(level), pop_size, n_gen, h.exp2(level)*(level+1));
  
  // HXOR
  //BBHC(hx, hx.exp2(level), pop_size, n_gen, hx.exp2(level)*(level+1));
  
  // HTRAP
  //BBHC(ht3, ht3.exp3(level), pop_size, n_gen, ht3.exp3(level)*level);  
}
