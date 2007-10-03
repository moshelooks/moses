/****
   Copyright 2005-2007, Moshe Looks and Novamente LLC

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

#ifndef _MOSES_BUILD_KNOBS_H
#define _MOSES_BUILD_KNOBS_H

#include "moses/knob_mapper.h"
#include "moses/using.h"
#include "combo/tree_type.h"
#include <boost/utility.hpp>
#include "eda/field_set.h"

namespace moses {

  //need to call a generator method... (dispatched based on type

  struct build_knobs : boost::noncopyable {
    build_knobs(vtree& exemplar,const combo::tree_type& t,knob_mapper& mapper,
		contin_t step_size=1.0,contin_t expansion=2.0,
		eda::field_set::arity_t depth=4);

    void build_logical(vtree::iterator it);
    void build_contin(vtree::iterator it);
  protected:
    vtree& _exemplar;
    combo::tree_type _type;
    knob_mapper& _mapper;
    int _arity;
    contin_t _step_size,_expansion;
    eda::field_set::arity_t _depth;
    
    void logical_canonize(vtree::iterator);
    void add_logical_knobs(vtree::iterator it,bool add_if_in_exemplar=true);
    void sample_logical_perms(vtree::iterator it,vector<vtree>& perms);
    void logical_probe(const vtree& tr,vtree::iterator it, bool add_if_in_exemplar);
    void logical_cleanup();
    bool disc_probe(vtree::iterator parent,disc_knob_base& kb);

    void contin_canonize(vtree::iterator);
    void canonize_div(vtree::iterator it);
    void add_constant_child(vtree::iterator it,contin_t v);
    vtree::iterator canonize_times(vtree::iterator it);
    void linear_canonize(vtree::iterator it);
    void rec_canonize(vtree::iterator it);
    void append_linear_combination(vtree::iterator it);
    vtree::iterator mult_add(vtree::iterator it,const vertex& v);
  };

} //~namespace moses

#endif
