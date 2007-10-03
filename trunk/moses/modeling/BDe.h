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

#ifndef BDE_H
#define BDE_H

#include "func_util.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/iterator/transform_iterator.hpp>
#include <boost/bind.hpp>
#include <functional>
#include <ext/functional>
#include <utility>
#include <vector>
#include <algorithm>
#include <ext/numeric>
#include "math_util.h"
#include "MedianStore.h"

/**
 * The Bayesian-Derichlet scoring Metric for decision trees.
 * First described in Heckerman, D., Geiger, D., and Chickering, D. 1994. 
 * Learning Bayesian Neworks: The Combination of Knowledge and Satistical Data.
 * Machine Learning, 20:197-243.
 * Exdended by Jiri Ocenasek, further extended by me.
 * Handles binary-variable-on-binary-variable splits with specialized code, 
 * with generalized code for all possible discrete-continuous splits (two-way 
 * as well as n-way when applicable). See paper for details.
 **/
#include <iostream>
namespace BDe {
  typedef int disc_t;
  typedef float contin_t;
  typedef float gain_t;

  using namespace std;
  using namespace __gnu_cxx;
  using namespace boost;

  class Log2Sum {
  public:
    void update(int maxSize);
    gain_t operator()(int i) { return _log2Sum[i]; }
  private:
    vector<gain_t> _log2Sum;
  };

  extern Log2Sum log2Sum;
  extern gain_t gainOffset;

  //this is specialized for binary-split-on-binary
  template<typename It1,typename It2>
  gain_t binary_binary(It1 from1,It1 to1,It2 from2);

  //equivalent to Ocanesek's CategoricalSplitCategoricalTargetBDe. For a 
  //particular disc var, using hillclimbing to find the best two-way split
  template<typename It1,typename It2,typename Out>
  gain_t disc_disc_2way(It1 from1,It1 to1,int c1,It2 from2,int c2,Out& out);

  //all new! compute the cost of the fixed split into n leaves
  //note that NWay spelled backwards is yawn
  //also note that under some circumstances this computation will compute 
  //(slightly) different gains from other methods on identical splits, due to 
  //the order that the computations are performed (different roundoffs).
  //I _really_ hate floating point numbers...
  template<typename It1,typename It2>
  gain_t disc_disc_nway(It1 from1,It1 to1,int c1,It2 from2,int c2);

  //equivalent to Ocanesek's CategoricalSplitContinuousTargetBDe within a 
  //particular group, using hillclimbing to find the best two-way split
  template<typename It1,typename It2,typename Out>
  gain_t contin_disc_2way(It1 from1,It1 to1,It2 from2,int c2,Out& out);

  //all new! compute the cost of the fixed split into n leaves
  //note that NWay spelled backwards is yawn
  //also note that under some circumstances this computation will compute 
  //(slightly) different gains from other methods on identical splits, due to 
  //the order that the computations are performed (different roundoffs).
  //I _really_ hate floating point numbers...
  template<typename It1,typename It2>
  gain_t contin_disc_nway(It1 from1,It1 to1,It2 from2,int c2);
  
  //equivalent to Ocenasek's function ContinuousSplitCategoricalTargetBDe
  template<typename It1,typename It2>
  pair<gain_t,contin_t> disc_contin_2way(It1 from1,It1 to1,int c1,It2 from2);

  //equivalent to Ocenasek's function ContinuousSplitContinuousTargetBDe
  template<typename It1,typename It2>
  pair<gain_t ,contin_t> contin_contin_2way(It1 from1,It1 to1,It2 from2);

  //for disc_disc_2way
  gain_t evaluateTransfer(int* nFrom,int* nTo,int sumFrom,int sumTo,
			 int* enCats,int* cntCats,int nCats,gain_t oldgain);
  void updateTransfer(int* nFrom,int* nTo,int& sumFrom,int& sumTo,
		      int* enCats,int* cntCats,int nCats);

  //for contin_disc_2way
  template<typename It>
  gain_t evaluateTransfer(It begin,It end,MedianStore& store,int nIn,int n);
  template<typename It>
  void updateTransfer(It begin,It end,MedianStore& store,int& nIn);

  //runs comparison tests on the metric
  void test();

  /***********
   * IMPLEMENTATION
   ***********/

  template<typename It1,typename It2>
  gain_t binary_binary(It1 from1,It1 to1,It2 from2) {
    int n=distance(from1,to1);
    log2Sum.update(n);
    
    int a=0,b=0,c=0,d=0;
    while (from1!=to1) {
      d+=(*from2 && *from1);
      c+=*from2++;
      b+=*from1++;
    }
    b-=d;
    c-=d;
    a=n-b-c-d; 

    /**cout << "binbin " << a << " " << b << " " << c << " " << d << endl
	 << +log2Sum(a+1)+log2Sum(b+1)+log2Sum(c+1)+log2Sum(d+1)
      -log2Sum(a+b+2)-log2Sum(c+d+2)-log2Sum(a+c+1)-log2Sum(b+d+1)
      +log2Sum(n+2) << " "
	 << gainOffset << " "
	 <<       +log2Sum(a+1)+log2Sum(b+1)+log2Sum(c+1)+log2Sum(d+1)
      -log2Sum(a+b+2)-log2Sum(c+d+2)-log2Sum(a+c+1)-log2Sum(b+d+1)
      +log2Sum(n+2)
      -gainOffset << " "
      << endl;**/
    
    return
      +log2Sum(a+1)+log2Sum(b+1)+log2Sum(c+1)+log2Sum(d+1)
      -log2Sum(a+b+2)-log2Sum(c+d+2)-log2Sum(a+c+1)-log2Sum(b+d+1)
      +log2Sum(n+2)
      -gainOffset;
  }

  template<typename It1,typename It2,typename Out>
  gain_t disc_disc_2way(It1 from1,It1 to1,int c1,It2 from2,int c2,Out& out) {
    typedef typename std::iterator_traits<It1>::value_type T1;
    typedef typename std::iterator_traits<It2>::value_type T2;
    typedef vector<pair<T2,T1> > Seq;
    typedef typename Seq::const_iterator const_iterator;

    int n=distance(from1,to1);
    if (n<2)
      return -1;
    log2Sum.update(n);

    //merge and sort - note that split-var (2) comes first
    Seq ord(n);
    transform(from2,from2+n,from1,ord.begin(),make_pair<T2,T1>);
    std::sort(ord.begin(),ord.end());
    
    //can't split on a single category
    if (ord.front().first==ord.back().first)
      return -1;

    int* nFrom=new int[c1];
    fill(nFrom,nFrom+c1,0);
    int*  nTo=new int[c1];
    fill(nTo,nTo+c1,0);
    int maxCatPairs=min(c1*c2,n);
    int* cntTargetCats=new int[maxCatPairs];
    fill(cntTargetCats,cntTargetCats+maxCatPairs,0);
    int* enTargetCats=new int[maxCatPairs];
    int* startNodeCats=new int[maxCatPairs+1];
    int sumFrom=n;
    int sumTo=0;
    int nNodeCats=1;
    int posNodeCats=0;
    vector<int> nodePos;
  
    int nTargetGroups=1;

    nFrom[ord.front().second]=1;

    cntTargetCats[0]=1;
    enTargetCats[0]=ord.front().second;
    startNodeCats[0]=0;

    const_iterator prev=ord.begin();
    nodePos.push_back(prev->first);
    for (const_iterator it=++ord.begin();it!=ord.end();++it) {
      bool splitVarChange=(prev->first!=it->first);
      bool treeVarChange=(prev->second!=it->second);
      if (splitVarChange || treeVarChange) {
	enTargetCats[++posNodeCats]=it->second;
	if (splitVarChange) {
	  startNodeCats[nNodeCats++]=posNodeCats;
	  nodePos.push_back(it->first);
	}
      }
      ++cntTargetCats[posNodeCats];
      if ((++nFrom[it->second])==1)
	++nTargetGroups;
      prev=it;
    }
    startNodeCats[nNodeCats]=posNodeCats+1;
    int nNodeGroups=nNodeCats;

    vector<int> sizeNodeCats(nNodeCats);
    for (int i=0;i<nNodeCats;++i)
      sizeNodeCats[i]=startNodeCats[i+1]-startNodeCats[i];

    gain_t maxGain=NEG_INFINITY;
    gain_t gain=0;

    while (true) {
      int bestCat=1;
      for (int i=1;i<nNodeCats;++i) {
	//evaluate the transfer
	gain_t newgain=evaluateTransfer(nFrom,nTo,sumFrom,sumTo,
					enTargetCats+startNodeCats[i],
					cntTargetCats+startNodeCats[i],
					sizeNodeCats[i],gain);
	if (newgain>maxGain) {
	  maxGain=newgain;
	  bestCat=i;
	}
      }
      if (maxGain<=gain)
	break;
      gain=maxGain;
    
      updateTransfer(nFrom,nTo,sumFrom,sumTo,
		     enTargetCats+startNodeCats[bestCat],
		     cntTargetCats+startNodeCats[bestCat],
		     sizeNodeCats[bestCat]);
      nNodeCats--;
      //*out++=nodePos[bestCat];
      //cout << nodePos[bestCat] << endl;
      //out.v=(out.v|(1<<(nodePos[bestCat])));
      out[nodePos[bestCat]]=true;
      startNodeCats[bestCat]=startNodeCats[nNodeCats];
      sizeNodeCats[bestCat]=sizeNodeCats[nNodeCats];
      nodePos[bestCat]=nodePos[nNodeCats];
    }

    //maxGain-=gainOffset*(nNodeGroups-1)*(nTargetGroups-1);

    /*cout << "maxGain " << maxGain << " " << nNodeGroups << " " 
      << nTargetGroups << " " << gainOffset << endl;*/
    //maxGain-=gainOffset*max(nNodeGroups+nTargetGroups-3,1);
    //maxGain-=gainOffset;
    maxGain-=gainOffset*(nNodeGroups-1);
    
    delete[] nFrom;
    delete[] nTo;
 
    delete[] cntTargetCats;
    delete[] enTargetCats;
    delete[] startNodeCats;

    return maxGain;
  }

  template<typename It1,typename It2>
  gain_t disc_disc_nway(It1 from1,It1 to1,int c1,It2 from2,int c2) {
    int n=distance(from1,to1);
    if (n<2)
      return -1;
    log2Sum.update(n);

    vector<vector<int> > nIn(c1,vector<int>(c2));
    vector<int> sumIn(c2);
    vector<bool> hasTarget(c1);
    int nNodeGroups=0,nTargetGroups=0;
    while (from1!=to1) {
      ++nIn[*from1][*from2];
      if (++sumIn[*from2]==1)
	++nNodeGroups;
      if (!hasTarget[*from1]) {
	hasTarget[*from1]=true;
	++nTargetGroups;
      }
      ++from1;
      ++from2;
    }

    gain_t gain=0;
    for (int i=0;i<c1;++i) {
      int sum=0;
      for (int j=0;j<c2;++j) {
	gain+=log2Sum(nIn[i][j]);
	sum+=nIn[i][j];
      }
      gain-=log2Sum(sum);
    }
    for (int i=0;i<c2;++i) {
      gain-=log2Sum(sumIn[i]);
    }
    //note: we are creating nGroup new leaves, and getting rid of one
    gain+=log2Sum(n)-gainOffset*(nNodeGroups-1)*(nTargetGroups-1); 

    /**cout << "discdisc "
	 << log2Sum(n) << " "
	 << nNodeGroups-1 << " "
	 << nTargetGroups-1 << " "
	 << gainOffset << " " 
	 << gainOffset*(nNodeGroups-1)*(nTargetGroups-1) 
	 << endl;**/
      
    return gain;
  }

  template<typename It1,typename It2,typename Out>
  gain_t contin_disc_2way(It1 from1,It1 to1,It2 from2,int c2,Out& out) {
    typedef typename std::iterator_traits<It1>::value_type T1;
    typedef typename std::iterator_traits<It2>::value_type T2;
    typedef pair<T1,T2> Item;
    typedef pair<Item,int> ItemIdx;
    typedef vector<ItemIdx> Seq;
    typedef typename Seq::const_iterator const_iterator;

    int n=distance(from1,to1);
    if (n<2)
      return -1;
    log2Sum.update(n);

    Seq ord(n);
    transform(from1,to1,from2,ord.begin(),
	      bind(make_pair<Item,int>,bind(make_pair<T1,T2>,_1,_2),0));
    //order by target, and store the resulting indices
    sort(ord.begin(),ord.end());
    transform(ord.begin(),ord.end(),make_counting_iterator(1),ord.begin(),
	      bind(make_pair<Item,int>,bind(select1st<ItemIdx>(),_1),_2));

    //now order by split, then by target
    sort(ord.begin(),ord.end(),
	 bind(inverse_order<T1,T2>,bind(select1st<ItemIdx>(),_1),
	      bind(select1st<ItemIdx>(),_2)));

    //can't split on one value
    if (ord.front().first.second==ord.back().first.second)
      return -1.0;
    
    vector<int> startNodeCats(n+1);
    startNodeCats[0]=0;
    int nNodeCats=1;
    const_iterator prev=ord.begin();
    vector<int> nodePos;
    nodePos.push_back(prev->first.second);
    const_iterator ordBegin=ord.begin();
    for (const_iterator it=ord.begin()+1;it!=ord.end();++it) {
      if (prev->first.second!=it->first.second) {
	startNodeCats[nNodeCats++]=distance(ordBegin,it);
	nodePos.push_back(it->first.second);
      }
      prev=it;
    }
    startNodeCats[nNodeCats]=n;
    int nNodeGroups=nNodeCats;
    
    vector<int> sizeNodeCats(nNodeCats);
    for (int i=0;i<nNodeCats;++i)
      sizeNodeCats[i]=startNodeCats[i+1]-startNodeCats[i];

    MedianStore store(n+1);

    int nIn=0;
    gain_t maxGain=NEG_INFINITY;
    while (true) {
      int bestCat=-1;
      for (int i=1;i<nNodeCats;++i) {
	gain_t newGain=evaluateTransfer
	  (make_transform_iterator(ord.begin()+startNodeCats[i],
				   select2nd<ItemIdx>()),
	   make_transform_iterator(ord.begin()+startNodeCats[i]+
				   sizeNodeCats[i],select2nd<ItemIdx>()),
	   store,nIn,n);
	if (newGain>maxGain) {
	  maxGain=newGain;
	  bestCat=i;
	}
      }
      if (bestCat==-1)
	break;
      updateTransfer
	(make_transform_iterator(ord.begin()+startNodeCats[bestCat],
				 select2nd<ItemIdx>()),
	 make_transform_iterator(ord.begin()+startNodeCats[bestCat]+
				 sizeNodeCats[bestCat],select2nd<ItemIdx>()),
	 store,nIn);
      --nNodeCats;
      //*out++=nodePos[bestCat];
      //cout << nodePos[bestCat] << endl;
      //out.v=(out.v|(1<<(nodePos[bestCat])));
      out[nodePos[bestCat]]=true;

      startNodeCats[bestCat]=startNodeCats[nNodeCats];
      sizeNodeCats[bestCat]=sizeNodeCats[nNodeCats];
      nodePos[bestCat]=nodePos[nNodeCats];
    }
    maxGain-=gainOffset*(nNodeGroups-1);
    
    return maxGain;
  }

  template<typename It>
  gain_t evaluateTransfer(It from,It to,MedianStore& store,int nIn,int n) {
    //for_each(from,to,bind(&MedianStore::addItem,store,_1));
    for (It it=from;it!=to;++it)
      store.addItem(*it);
  
    nIn+=distance(from,to);
    int median=store.getItemByCount(nIn/2);
    int yind;
    if (median<n/2)
      yind=(2*median+nIn)/2;  //heuristics
    else
      yind=(2*median-nIn)/2;  //heuristics
    int cnt=store.getCountBelowEq(yind);
    gain_t gain=
      +log2Sum(cnt)+log2Sum(nIn-cnt)+log2Sum(yind-cnt)+log2Sum(n-nIn-yind+cnt)
      -log2Sum(nIn)-log2Sum(n-nIn)-log2Sum(yind)-log2Sum(n-yind)+log2Sum(n);
    for (It it=from;it!=to;++it)
      store.removeItem(*it);
    return gain;
  }

  template<typename It>
  void updateTransfer(It from,It to,MedianStore& store,int &nIn)  {
    for (It it=from;it!=to;++it)
      store.addItem(*it);
    nIn+=distance(from,to);
  }


  template<typename It1,typename It2>
  gain_t contin_disc_nway(It1 from1,It1 to1,It2 from2,int c2) {
    typedef typename std::iterator_traits<It1>::value_type T1;
    typedef typename std::iterator_traits<It2>::value_type T2;
    typedef vector<pair<T1,T2> > Seq;
    typedef typename Seq::const_iterator const_iterator;

    int n=distance(from1,to1);
    log2Sum.update(n);

    Seq ord(n);
    transform(from1,to1,from2,ord.begin(),make_pair<T1,T2>);
    sort(ord.begin(),ord.end());

    vector<int> sumIn(c2);
    vector<int> partialSumIn(c2);
    int c=0; //cardinality
    for (const_iterator it=ord.begin();it!=ord.end();++it)
      if (++sumIn[it->second]==1)
	++c;

    if (c<2)
      return -1; //can't split on a single group
    int partialSum=0;
    const_iterator prev=ord.begin();
    gain_t maxGain=NEG_INFINITY;
    for (const_iterator it=++ord.begin();it!=ord.end();++it) {
      ++partialSumIn[it->second];
      ++partialSum;
      gain_t gain=0;
      //we are O(c2); we could be O(c), but not worth the overhead
      for (int i=0;i<c2;++i)
	gain+=log2Sum(partialSumIn[i])
	  +log2Sum(sumIn[i]-partialSumIn[i])-log2Sum(sumIn[i]);
      gain-=log2Sum(partialSum)+log2Sum(n-partialSum);

      //we don't care what i is here, just the  maxGain 
      //(see Ocanesek's thesis, bot of p.83)
      maxGain=max(gain,maxGain); 
      prev=it;
    }
    //since we are creating c new leaves and getting rid of one
    maxGain+=log2Sum(n)-gainOffset*(c-1);
    
    return maxGain;
  }

  template<typename It1,typename It2>
  pair<gain_t,contin_t> disc_contin_2way(It1 from1,It1 to1,int c1,It2 from2) {
    typedef typename std::iterator_traits<It1>::value_type T1;
    typedef typename std::iterator_traits<It2>::value_type T2;
    typedef vector<pair<T2,T1> > Seq;
    typedef typename Seq::const_iterator const_iterator;

    int n=distance(from1,to1);
    if (n<2)
      return make_pair(gain_t(-1),contin_t(0));
    log2Sum.update(n);

    //note that split comes before target
    Seq ord(n);
    transform(from2,from2+n,from1,ord.begin(),make_pair<T2,T1>);
    std::sort(ord.begin(),ord.end());

    //start out with nFrom[i] as the number of instances with cat i at _idx
    vector<int> nFrom(c1);
    int nTargetGroups=1;
    for (const_iterator it=ord.begin();it!=ord.end();++it)
      if ((++nFrom[it->second]==1))
	++nTargetGroups;
    //start out with nTo an empty vector (zeros)
    vector<int> nTo(c1);
    //start out with sumFrom as the total # of instances
    int sumFrom=n;
    
    gain_t maxGain=NEG_INFINITY;
    gain_t gain=0;
    contin_t splitAt=0;
    const_iterator second=++ord.begin(),secondToLast=--(--ord.end());
    for (const_iterator it=ord.begin();it!=secondToLast;) {
      gain-= 
	+log2Sum(nFrom[it->second])
	+log2Sum(nTo[it->second])
	-log2Sum(sumFrom)
	-log2Sum(n-sumFrom);
      --nFrom[it->second]; //we have removed one from the right
      ++nTo[it->second]; //and added it to the left
      --sumFrom;
      gain+=
	+log2Sum(nFrom[it->second])
	+log2Sum(nTo[it->second])
	-log2Sum(sumFrom)
	-log2Sum(n-sumFrom);
      //we can't split if this value is the same as the next one
      contin_t value=(it++)->first;
      if (value==it->first)
	continue;
      //check against maxgain, assuming this is a valid split
      if (gain>maxGain && it!=second) {
	maxGain=gain;
	splitAt=value;
      }
    }
    maxGain-=gainOffset*(nTargetGroups-1);

    return make_pair(maxGain,splitAt);
  }

  template<typename It1,typename It2>
  pair<gain_t ,contin_t> contin_contin_2way(It1 from1,It1 to1,It2 from2) {
    typedef typename std::iterator_traits<It1>::value_type T1;
    typedef typename std::iterator_traits<It2>::value_type T2;
    typedef pair<T1,T2> Item;
    typedef pair<Item,int> ItemIdx;
    typedef vector<ItemIdx> Seq;
    typedef typename Seq::const_iterator const_iterator;

    int n=distance(from1,to1);
    if (n<2)
      return make_pair(gain_t(-1),contin_t(0));
    log2Sum.update(n);
    
    Seq ord(n);
    transform(from1,to1,from2,ord.begin(),
	      bind(make_pair<Item,int>,bind(make_pair<T1,T2>,_1,_2),0));
    //order by target, and store the resulting indices
    sort(ord.begin(),ord.end());
    transform(ord.begin(),ord.end(),make_counting_iterator(1),ord.begin(),
	      bind(make_pair<Item,int>,bind(select1st<ItemIdx>(),_1),_2));
    //now order by split, then by target
    sort(ord.begin(),ord.end(),
	 bind(inverse_order<T1,T2>,bind(select1st<ItemIdx>(),_1),
	      bind(select1st<ItemIdx>(),_2)));

    gain_t maxGain=NEG_INFINITY;
    contin_t splitAt=0;
    MedianStore store(n+1);
    const_iterator it=ord.begin();
    T2 last=ord.back().first.second;
    for (int i=1;i<n && it->first.second!=last;++i,++it) {
      int yind;
      store.addItem(it->second);
      
      if (it->first.second==(it+1)->first.second || 
	  it->first.first==(it+1)->first.first)
	continue;
      
      int median=store.getItemByCount(i/2);
      if (median<n/2)
	yind=(2*median+i)/2;
      else
	yind=(2*median-i)/2;
      int cnt=store.getCountBelowEq(yind);
      gain_t gain=
	+log2Sum(cnt+1)+log2Sum(i-cnt+1)
	+log2Sum(yind-cnt+1)+log2Sum(n-i-yind+cnt+1)
	-log2Sum(i+2)-log2Sum(n-i+2)
	-log2Sum(yind+1)-log2Sum(n-yind+1);
      if (gain>maxGain) {
	maxGain=gain;
	splitAt=it->first.second;
      }
    }
    maxGain+=log2Sum(n)-gainOffset;
    
    return make_pair(maxGain,splitAt);
  }

} //~namespace BDe

#endif
