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

#ifndef _MOSES_KNOB_MAPPER_H
#define _MOSES_KNOB_MAPPER_H

#include <map>
#include "eda/field_set.h"
#include "moses/knobs.h"

namespace moses {
  struct knob_mapper {
    typedef eda::field_set field_set;

    //important: knobs are kept sorted in an order consistant with that of the
    //field_set _fields that is constructed according to their corresponding specs
    typedef std::multimap<field_set::disc_spec,disc_knob> disc_map;
    typedef disc_map::value_type disc_v;
    typedef std::multimap<field_set::contin_spec,contin_knob> contin_map;
    typedef contin_map::value_type contin_v;

    disc_map disc;
    contin_map contin;
  };

} //~namespace moses

#endif
