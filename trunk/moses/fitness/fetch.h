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

#ifndef FETCH_H
#define FETCH_H

namespace fitness {

  template<typename T>
  class EasyFetch : public std::unary_function<tree<T>,float> {
  public:    
    typedef typename tree<T>::sibling_iterator sib_it;
    float operator()(const tree<T>& tr) const {
      cout << "Ef" << tr << endl;
      holdingball=false;
      saiddone=false;
      balldist=randint(4)+1;
      teachdist=randint(4)+1;
      for (int i=0;i<30 && !saiddone;++i)
	doit(tr,tr.begin());
      float score=0;

      if (saiddone)
	score+=0.5;
      if (holdingball)
	score+=1;
      if (balldist==0)
	score+=0.1;
      if (teachdist==0 && holdingball)
	score+=1;
      cout << "Ef" << endl;      
      return score;
    }
    
    void doit(const tree<T>& tr,sib_it it) const {
      if (saiddone)
	return;
      if (is_progn(*it)) {
	for (sib_it sib=it.begin();sib!=it.end();++sib)
	  doit(tr,sib);
      } else if (is_pickup(*it)) {
	if (is_ball(*it.begin()) && balldist==0)
	  holdingball=true;
      } else if (is_walktowards(*it)) {
	if (is_ball(*it.begin())) {
	  if (balldist>0)
	    --balldist;
	  ++teachdist;
	} else if (is_teacher(*it.begin())) {
	  if (teachdist>0)
	    --teachdist;
	  --balldist;
	}
      } else if (is_done(*it)) {
	saiddone=true;
      } else {
	assert(is_ifelse(*it));
	if (eval_pred(it.begin()))
	  doit(tr,++it.begin());
	else
	  doit(tr,++++it.begin());
      }
    }

    bool eval_pred(sib_it it) const {
      if (is_holding(*it)) {
	if (is_teacher(*it.begin()))
	  return false;
	return holdingball;
      } else {
	assert(is_near(*it));
	if (is_teacher(*it.begin()))
	  return teachdist==0;
	return balldist==0;
      }
    }

    mutable bool holdingball;
    mutable bool saiddone;
    mutable int balldist;
    mutable int teachdist;
  };

} //~namespace fitness
      
#endif
