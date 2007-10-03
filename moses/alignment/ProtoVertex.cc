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

#include "ProtoVertex.h"
#include <boost/iterator/counting_iterator.hpp>
#include <boost/bind.hpp>
#include "func_util.h"
#include <ext/hash_map>

namespace alignment {
  using namespace __gnu_cxx;

  void reduce_vlocs(ptree& tr) { 
    hash_map<int,int> indices;
    for (ptree::pre_order_iterator pv=tr.begin();pv!=tr.end();++pv) {
      for (VlocSeq::iterator vl=pv->vlocs.begin();vl!=pv->vlocs.end();++vl) {
	hash_map<int,int>::iterator idx=indices.find(vl->first);
	if (idx==indices.end())
	  idx=indices.insert(make_pair(vl->first,indices.size())).first;
	vl->first=idx->second;
      }
    }
  }

  const list<int> vtree2ptree::emptylist=list<int>();

  void vtree2ptree::rec_make(vtree::sibling_iterator src,
			     ptree::sibling_iterator dst) {
    *dst=ProtoVertex(*src,indices);
    tr.append_children(dst,src.number_of_children());
    for_each(make_counting_iterator(src.begin()),
	     make_counting_iterator(src.end()),
	     make_counting_iterator(dst.begin()),
	     bind(&vtree2ptree::rec_make,this,_1,_2));
  }

} //~namespace alignment
