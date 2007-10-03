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

#ifndef _MOSES_SCORING_FUNCTIONS_H
#define _MOSES_SCORING_FUNCTIONS_H

#include "moses/using.h"

namespace moses {

  struct even_parity {
    template<typename It>
    bool operator()(It from,It to) const { 
      bool parity=true;
      while (from!=to)
	parity ^= *from++;
      return parity;
    }
  };

  struct disjunction {
    template<typename It>
    bool operator()(It from,It to) const { 
      while (from!=to)
	if (*from++)
	  return true;
      return false;
    }
  };

  struct simple_symbolic_regression {
    simple_symbolic_regression(int o=4) : order(o) { }
    int order;
    template<typename It>
    contin_t operator()(It from,It to) const { 
      cout << "ff" << endl;
      contin_t res=0;
      dorepeat(order)
	res=(res+contin_t(1))*(*from);
      cout << "uck" << endl;
      return res;
    }
  };

} //~namespace moses

#endif
