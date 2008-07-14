/****
   Copyright 2005-2007, Chen Shuo

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
/*** written by ChenShuo ***/

#ifndef _BB_H
#define _BB_H
#include <algorithm>
#include <vector>
#include "modeling.h"

using namespace std;

namespace modeling
{

typedef pair<double,Assignment> fpair;

//class BBConfig;

class BuildingBlock
{
 public:
  BuildingBlock(int chroLength)
      : _inblock(chroLength), _length(chroLength), _clustered(false) {};
  BuildingBlock(const BuildingBlock& bb);
  BuildingBlock operator + (BuildingBlock &bb)
  { BuildingBlock tmp(length()+ bb.length());
    std::copy(_inblock.begin(), _inblock.end(), tmp._inblock.begin());
    std::copy(bb._inblock.begin(), bb._inblock.end(), tmp._inblock.begin()+bb.length()); }
  template <class randOpt>
  Assignment mutate(fpair a, randOpt rand)
  { for(Assignment::iterator i=a.second.begin(); i!=a.second.end(); ++i)
      if (_inblock.at(i)) a.second.at(i)=rand(); } // this variable is in the current BB
  bool configEqualTo(Assignment a1, Assignment a2)
  { assert(a1.size()==length() && a2.size()==length());
    for (int i=0; i<length(); ++i) 
      if (_inblock[i] && !(a1[i]==a2[i]) ) return false;
    return true; }
  bool bijective(vector<fpair> &pop, BuildingBlock bb)
  { vector<fpair>::iterator i,j;
    BuildingBlock tmpBB= this->operator+(bb);
    for (i=pop.begin(); i!=pop.end(); ++i) 
      for (j=i+1; j!=pop.end(); ++j)
        if ( this->configEqualTo(pop.at(i).second, pop.at(j).second) ) return false;
    return true; }
  bool clustered() const {return _clustered; }
  void setClustered(bool b) {_clustered=b; }
  int length() const {return _length; };
 private:
  vector<bool> _inblock;
  //vector<BBConfig> _configs;
  int _length;
  bool _clustered;
};

  
class BBs
{
 public:
  BBs(int chroLength) : _bbs(chroLength, BuildingBlock(1)) {};
  /***
   *  do hill climbing on BBs 
   **/
  template <class Scoring, class randOpt>	
  void hillClimb(vector<fpair> &pop, Scoring scoring, randOpt rand)
  { Assignment tmp; int score;
    for (vector<fpair>::iterator pi=pop.begin(); pi!=pop.end(); ++pi) 
      for (vector<BuildingBlock>::iterator bi=_bbs.begin(); bi!=_bbs.end(); ++bi)
      { tmp=bi->mutate(pi->second, rand);  //macro mutation
        score=scoring(tmp);					
        if (score> pi->first)               
        { pi->first=score;
          pi->second=tmp;
        }
      }
  }
  /***
   * 
   **/
  void updateBBs(vector<fpair> &pop)
  { vector<BuildingBlock>::iterator bi,bi1;
    for(bi=_bbs.begin(); bi!=_bbs.end(); ++bi) bi->setClustered(false);
    for(bi=_bbs.begin(); bi!=_bbs.end(); ++bi ) 
      for(bi1=bi+1; bi1!=_bbs.end(); ++bi1 )
        if (!bi->clustered() && !bi1->clustered())  // hasn't been clustered
          if (bi->bijective(pop, *bi1)) 	    // cluster the 2 BBs
          {
            *bi=*bi+*bi1;
            bi->setClustered(true);
            bi1->setClustered(true);
          }
  }
 private:
  vector<BuildingBlock> _bbs;
};
  
//  template<typename Opt>
//  void hillClimb(Opt score, Assignment &a, BuildingBlock b);  
  
  
/*
  template<typename Opt>
  class BBHC 
  {
  public:
  BBHC(Opt score,int length,int pop_size,int n_gen,double maxscore);
  void hillClimb(vector<Assignment> pop);
  void updateBB();
  };
*/
};
	
#endif

