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

#ifndef __EXCEPTION_H
#define __EXCEPTION_H

#include <string>
#include "combo/vertex.h"

class Exception {
protected:
  std::string _message;
public:
  Exception() {}
  Exception(std::string m) : _message(m) {}

  std::string get_message() { return _message; }
};

class EvalException : public Exception {
  combo::vertex _vertex;
public:
  EvalException() {}
  EvalException(combo::vertex v) : _vertex(v) {
    _message = "Eval Exception";
  }

  combo::vertex get_vertex() { return _vertex; }
};

class TypeCheckException : public Exception {
  int _arg;
public:
  TypeCheckException() {
    _message = "Type check Exception";
  }
  TypeCheckException(int arg) : _arg(arg) {
    TypeCheckException::TypeCheckException();
  }
};

#endif
