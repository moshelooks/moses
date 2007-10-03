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

#ifndef INTERFACEUTIL_H
#define INTERFACEUTIL_H

#include "math_util.h"
#include <sstream>

#include <iostream>
#include <string>

class Printable {
 public:
  virtual ~Printable() { }
  virtual std::string str() const=0;
};

class Clonable {
 public:
  virtual ~Clonable() { }
  virtual Clonable* clone() const=0;
};

inline std::ostream& operator<<(std::ostream& os,const Printable& n);

std::ostream& operator<<(std::ostream& os,const Printable& n) {
  os << n.str();
  return os;
}

#endif

