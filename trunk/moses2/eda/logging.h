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

#ifndef _EDA_LOGGING_H
#define _EDA_LOGGING_H

#include <algorithm>
#include <iostream>
#include "eda/field_set.h"

namespace eda {

  struct cout_log_best_and_gen {
    template<typename It>
    void operator()(It from,It to,const field_set& fs,int gen) const {
      if (from==to)
	return;

      It best=std::max_element(from,to);
      std::cout << gen << " : "
		<< best->second << " " 
		<< fs.stream(best->first) << std::endl;
    }
  };

} //~namespace eda

#endif
