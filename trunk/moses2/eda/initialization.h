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

#ifndef _EDA_INITIALIZATION_H
#define _EDA_INITIALIZATION_H

#include "eda/field_set.h"

//various routines for initializing instances
namespace eda {
  
  //occam randomize a particular contin or onto field
  void occam_randomize(const field_set&,instance&,
		       field_set::const_contin_iterator);
  void occam_randomize(const field_set&,instance&,
		       field_set::const_onto_iterator);

  //occam randomize all contin or onto fields
  void occam_randomize_onto(const field_set&,instance&);
  void occam_randomize_contin(const field_set&,instance&);

  //uniformly randomize all bit or disc fields
  void uniform_randomize_bits(const field_set&,instance&);
  void uniform_randomize_disc(const field_set&,instance&);

  //occam randomize all contin and onto fields, and uniformly randomize all bit
  //and disc fields
  void randomize(const field_set&,instance&);

} //~namespace eda

#endif
