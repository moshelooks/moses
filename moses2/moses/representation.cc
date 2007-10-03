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

#include "moses/representation.h"
#include "util/lazy_random_selector.h"
#include "build_knobs.h"

namespace moses {
  representation::representation(const reduct::rule& simplify,const vtree& exemplar_,
				 const tree_type& t) : _exemplar(exemplar_) {
    //build the knobs
    build_knobs(_exemplar,t,*this);

    //handle knob merging

    //convert the knobs into a field specification
    std::multiset<field_set::spec> tmp;
    foreach(const disc_map::value_type& v,disc)
      tmp.insert(v.first);
    foreach(const contin_map::value_type& v,contin)
      tmp.insert(v.first);
    _fields=field_set(tmp.begin(),tmp.end());
            
    std::cout << "#knobs " << disc.size() << " + " << contin.size() << std::endl;
    std::cout << "exemplar: " << _exemplar << std::endl;
  }

  void representation::transform(const instance& inst) {
    contin_map::iterator ckb=contin.begin();
    for (field_set::const_contin_iterator ci=_fields.begin_contin(inst);
	 ci!=_fields.end_contin(inst);++ci,++ckb) {
      ckb->second.turn(*ci);
      //_exemplar.validate();
    }

    //need to add first onto & then contin
    //cout << _fields.stream(inst) << endl;
    disc_map::iterator dkb=disc.begin();
    for (field_set::const_disc_iterator di=_fields.begin_disc(inst);
	 di!=_fields.end_disc(inst);++di,++dkb) {
      dkb->second->turn(*di);
      //_exemplar.validate();
    }
    for (field_set::const_bit_iterator bi=_fields.begin_bits(inst);
	 bi!=_fields.end_bits(inst);++bi,++dkb) {
      dkb->second->turn(*bi);
    }
    //cout << _exemplar << endl;
  }

  void representation::clear_exemplar() {
    foreach(disc_v& v,disc)
      v.second->clear_exemplar();
    foreach(contin_v& v,contin)
      v.second.clear_exemplar();
  }
  
} //~namespace moses
