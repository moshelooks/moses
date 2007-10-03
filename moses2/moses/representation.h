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

#ifndef _MOSES_REPRESENTATION_H
#define _MOSES_REPRESENTATION_H

#include "moses/using.h"
#include "moses/knob_mapper.h"
#include "reduct/reduct.h"
#include "combo/tree_type.h"
#include <boost/utility.hpp>

namespace moses {

  struct representation : public knob_mapper,boost::noncopyable {
    typedef eda::instance instance;

    representation(const reduct::rule& simplify,const vtree& exemplar_,
		   const combo::tree_type& t);

    void transform(const instance&);
    void clear_exemplar();

    const field_set& fields() const { return _fields; }
    const vtree& exemplar() const { return _exemplar; }
  protected:
    vtree _exemplar;
    field_set _fields;
  };

} //~namespace moses

#endif
