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

#include "build_tree.h"

namespace modeling {

  void build_tree::rec_build(ppre_it src,ppre_it sup,ass_it& ass,vpre_it dst) {
    using namespace id;
    if (src->v.size()==1) {
      if (src->vlocs.size()<sup->vlocs.size()) {
	if (!boost::get<bool>(*ass)) {
	  dst_tr.erase(dst);
	  skip_subvariables(src,sup,ass);
	  return;
	}
	sup=src;
	++ass;
      }
      *dst=*src->v.begin();
      for (psib_it child=src.begin();child!=src.end();++child)
	rec_build(child,sup,ass,dst_tr.append_child(dst));
    } else {
      assert(src.number_of_children()==0);
      if (is_turn(*src)) {
	bool l=boost::get<bool>(*ass++);
	bool r=boost::get<bool>(*ass++);
	if (l) {
	  if (r)
	    *dst=create_reversal<Vertex>();
	  else
	    *dst=create_left<Vertex>();
	} else if (r)
	  *dst=create_right<Vertex>();
	else
	  dst_tr.erase(dst);
      } else {
	assert(is_variable(*src));
	bool set=false;
	for (VertexSet::const_iterator it=src->v.begin();
	     it!=src->v.end();++it) 
	  if (boost::get<bool>(*ass++)) {
	    assert(set==false);
	    set=true;
	    *dst=*it;
	  }
	if (set==false)
	  dst_tr.erase(dst);
      }
    } 
  }

  void build_tree::skip_subvariables(ppre_it src,ppre_it sup,ass_it& ass) {
    using namespace id;

    if (src->v.size()==1) {
      if (src->vlocs.size()<sup->vlocs.size()) {
	sup=src;
	++ass;
      }
      for (psib_it child=src.begin();child!=src.end();++child)
	skip_subvariables(child,sup,ass);
    } else {
      assert(src.number_of_children()==0);
      if (is_turn(*src)) {
	ass+=2;
      } else {
	assert(is_variable(*src));
	ass+=src->v.size();
      }
    }
  }

} //~namespace modeling
