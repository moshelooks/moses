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

#ifndef _BB_H
#define _BB_H
#include <algorithm>
#include <vector>
#include <iostream>
#include "modeling.h"

using namespace std;

struct RandomOrder
{
	RandomOrder(int size): _order(size) 
	  { for (int i=0; i<size; ++i) _order[i]=i; 
	    std::random_shuffle(_order.begin(), _order.end()); };
	int operator[](int i) { return _order[i]; }
	vector<int> _order;
};

namespace modeling
{

  class BuildingBlock
  {
  friend class BBs;
  public:
    BuildingBlock(int chroLength)
      : _inblock(chroLength, false), _configs(0), _length(chroLength), _clustered(false) {};
    BuildingBlock(const BuildingBlock& bb) //
      : _inblock(bb._inblock), _configs(bb._configs), _length(bb._length), _clustered(bb._clustered) {};
    BuildingBlock& operator= (const BuildingBlock& bb)
      { _inblock=bb._inblock; _configs=bb._configs; 
        _length=bb._length; _clustered=bb._clustered;
        return *this; }
    BuildingBlock operator + (BuildingBlock &bb)
      { assert(length() == bb.length());
    	BuildingBlock tmp(length());
    	for (int i=0; i<length(); ++i)
    	  tmp._inblock[i] = this->_inblock[i] || bb._inblock[i]; 
    	return tmp; }
    void setBlockIn(int locus)
      { _inblock[locus]=true; }
    void setBlockOut(int locus)
      { _inblock[locus]=false; }
    Assignment mutateToConfigs(const Assignment& a)  // randomly
      { int rand = randint(_configs.size());
        for (Assignment::const_iterator i=a.begin(); i!=a.end(); ++i) assert(boost::get<bool>(&(*i)));
        //if (rand == _configs.size()) return a; // no change
    	Assignment tmp=a,
    	           filter= _configs[rand]; 
        for (int i=0; i<_length; ++i) if (_inblock[i]) tmp[i]=filter[i];
        return tmp; }
    Assignment mutateToConfigs(int n, const Assignment& a)
      { assert(n<_configs.size() && n>=0);
        Assignment tmp=a,
                   filter= _configs[n]; 
        for (int i=0; i<_length; ++i) if (_inblock[i]) tmp[i]=filter[i]; 
        return tmp; }
    void mutateConfig(int n, Assignment& a)
      { assert(n<_configs.size() && n>=0);
        Assignment filter=_configs[n]; 
        for (int i=0; i<_length; ++i) if (_inblock[i]) a[i]=filter[i]; }
    void addConfig(const Assignment& a)
      { _configs.push_back(a); }
    void clearConfig()
      { _configs.clear(); }
    bool hasConfig(const Assignment& a)
      { if (_configs.size()==0) return false;
    	for (int i=0; i<_configs.size(); ++i) 
    	  if (configEqualTo(a, _configs[i])) return true; 
    	return false; }
    bool configEqualTo(const Assignment& a1, const Assignment& a2) const
      { if (a1.size()!=length() || a2.size()!=length()) 
    	  cout<<"a1: "<<a1.size()<<" ; a2: "<<a2.size()<<" ;block: "<<length()<<endl;
    	assert(a1.size()==length() && a2.size()==length());
        for (int i=0; i<length(); ++i) 
          if (_inblock[i] && !(a1[i]==a2[i]) ) return false;
        return true; }
    template<typename popIter>
    bool bijective(popIter first, popIter last, const BuildingBlock& bb) {
      popIter i,j;
      for (i=first; i!=last; ++i) 
        for (j=i+1; j!=last; ++j)
          if ( (this->configEqualTo(i->second, j->second) && 
                !bb.configEqualTo(i->second, j->second)) 
               || (!this->configEqualTo(i->second, j->second) && 
                   bb.configEqualTo(i->second, j->second)) )
            return false;
      return true; 
    }
    template<typename popIter>
    double bjConf(popIter first, popIter last, const BuildingBlock & bb)  // bijective with confidence
      { int failures=0, popNum=last-first;
        popIter i,j;
        for (i=first; i!=last; ++i) 
          for (j=i+1; j!=last; ++j)
          	if ( (this->configEqualTo(i->second, j->second) && !bb.configEqualTo(i->second, j->second)) 
          	  || (!this->configEqualTo(i->second, j->second) && bb.configEqualTo(i->second, j->second)) )
          	  failures+=1;
    	return 1-failures/(popNum*(popNum-1));
      }
    // return the position of the configs which a has in this BB
    disc_t toDiscType(Assignment& a) 
      { vector<Assignment>::iterator it;
        for (it=_configs.begin(); it!=_configs.end(); ++it) 
          if (configEqualTo(a, *it)) return it-_configs.begin(); 
        return -1; }  // -1 means error, when configs not matched
    bool clustered() const { return _clustered; }
    void setClustered(bool b) { _clustered=b; }
    int length() const { return _length; }
    int configNum() const { return _configs.size(); }
  private:
    vector<bool> _inblock;
    vector<Assignment> _configs;
    int _length;
    bool _clustered;
  };

  
  class BBs
  {
  public:
	BBs(int chroLength) : _bbs(chroLength, BuildingBlock(chroLength)) 
	  { Assignment a(chroLength, false), b(chroLength, true);
		for (int i=0; i<chroLength; ++i) 
		  { _bbs[i].setBlockIn(i);  // at initiation, each bit is a single BB
		    _bbs[i].setClustered(false); 
		    _bbs[i].addConfig(a);
		    _bbs[i].addConfig(b); }
	  }
	/***
	 *  do hill climbing on BBs 
	 **/
    template <class Scoring, class popIter>	
	void hillClimb(popIter pFirst, popIter pLast, Scoring& scoring)
	  { Assignment tmp(pFirst->second.size(), false); 
	    double score;	
		for (popIter pi=pFirst; pi!=pLast; ++pi)   // for each individual in the pop
		{ RandomOrder BBOrder(_bbs.size());    // reorder of BBs
		  for (int bi=0; bi<_bbs.size(); ++bi)  // for each BB in a random order
		  { RandomOrder configOrder(_bbs[BBOrder[bi]].configNum()); // reoder of configs 
			for (int ci=0; ci<_bbs[BBOrder[bi]].configNum(); ++ci)  // for each configs
		    { tmp=_bbs[BBOrder[bi]].mutateToConfigs(configOrder[ci], pi->second); 
			  score=scoring(tmp);					
		      if (score>=pi->first)               
		      { pi->first=score;
		        pi->second=tmp;
		      }
		    }  
		  }
		}  
	  }
	template <class Scoring>	
	void hillClimb(Assignment& state, Scoring& scoring)
	  { Assignment tmp; 
	    double score,oldScore=scoring(state);
	    RandomOrder BBOrder(_bbs.size());    // reorder of BBs
	    for (int bi=0; bi<_bbs.size(); ++bi)   //for each BB
	    { RandomOrder configOrder(_bbs[BBOrder[bi]].configNum());  // reorder of configs
		  for (int ci=0; ci<_bbs[BBOrder[bi]].configNum(); ++ci)   // for each configs
	      { tmp=_bbs[BBOrder[bi]].mutateToConfigs(configOrder[ci], state); 
	        score=scoring(tmp);					
		    if (score>=oldScore) 
		  	{ state=tmp;
		  	  oldScore=score;
		  	}
	      } 
		}
	  }
	//
    template<typename popIter>
    bool updateBBs(popIter first, popIter last) {
      vector<BuildingBlock>::iterator bi,bi1;
      bool updated=false;
      // update BB structure
      for (bi=_bbs.begin(); bi!=_bbs.end(); ++bi) {
        if (bi->clustered())
          continue;
        for(bi1=bi+1; bi1!=_bbs.end(); ++bi1) {
          if ( !bi->clustered() && !bi1->clustered()  // hasn't been clustered
               && bi->bijective(first, last, *bi1))
          { // cluster the 2 BBs
            *bi=*bi+*bi1;
            bi->clearConfig();
            bi1->setClustered(true);
            ++bi;
            updated=true;
            break;
          }
        }
      }
      // delete BBs which are clustered
      for(bi=_bbs.begin(); bi<_bbs.end(); ++bi)
        if (bi->clustered()) { _bbs.erase(bi); --bi; } 	  // can be optimized
      return updated;
    }

    template<typename popIter>
	void updateBBsWithConf(popIter first, popIter last, double confidence)
	  { vector<BuildingBlock>::iterator bi,bi1;
		// update BB structure
	    for (bi=_bbs.begin(); bi!=_bbs.end(); ++bi) 
	      for(bi1=bi+1; bi1!=_bbs.end(); ++bi1)
	        if ( !bi->clustered() && !bi1->clustered()  // hasn't been clustered
	           && bi->bjConf(first, last, *bi1)>confidence ) 	   		
	          { // cluster the 2 BBs
	            *bi=*bi+*bi1;
	            bi->clearConfig();
	            bi1->setClustered(true);
	            ++bi;
	            break;
	          }
	    // delete BBs which are clustered
        for(bi=_bbs.begin(); bi<_bbs.end(); ++bi)
          if (bi->clustered()) { _bbs.erase(bi); --bi; } 	  // can be optimized
        int sum=0; for (int i=0; i<_bbs.size(); ++i) sum+=_bbs[i].configNum();
	  }
	template<typename popIter>
	void updateBBConfigs(popIter first, popIter last)
	  { vector<BuildingBlock>::iterator bi;
	    for (bi=_bbs.begin(); bi!=_bbs.end(); ++bi) 
	      for (popIter pi=first; pi!=last; ++pi)
	    	if (!bi->hasConfig(pi->second)) bi->addConfig(pi->second);        
	  }	
	Assignment getRandomState()
	  { Assignment tmp(_bbs[0].length());
	    generate(tmp.begin(), tmp.end(), &randbool);
	    for (int i=0; i<_bbs.size(); ++i) tmp=_bbs[i].mutateToConfigs(/*randint(_bbs[i].configNum()),*/ tmp); 
	    return tmp; }
	int blocks() const { return _bbs.size(); }
	int bbconfigs() const 
	  { int sum=0; 
	    for (int i=0; i<_bbs.size(); ++i) sum+=_bbs[i].configNum();
		int counter=0;
		for (int i=0; i<_bbs.size(); ++i) counter+=_bbs[i].configNum(); 
	    return sum; }
	Assignment toBoolType(Assignment& a)
	  { Assignment tmp(_bbs[0].length(), false);  
	    for(int i=0; i<_bbs.size(); ++i)
	      _bbs[i].mutateConfig(todisc(a[i]), tmp);
	    return tmp;
	  }
	Assignment toDiscType(Assignment& a)
      { Assignment tmp(_bbs.size(), 0);
        for(int i=0; i<_bbs.size(); ++i) 
          { tmp[i]=_bbs[i].toDiscType(a); 
            assert(todisc(tmp[i])>=0);  }
        return tmp; }
  private:
	vector<BuildingBlock> _bbs;
  };
  
};
	
#endif
