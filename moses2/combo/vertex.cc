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

#include "vertex.h"
#include <boost/lexical_cast.hpp>

std::ostream& operator<<(std::ostream& out,const combo::builtin& h) {
  using namespace combo;
  switch(h) {
  case id::null_vertex:
    return out << "null_vertex";
  case id::logical_and:
    return out << "and";
  case id::logical_or:
    return out << "or";
  case id::logical_not:
    return out << "not";
  case id::logical_true:
    return out << "true";
  case id::logical_false:
    return out << "false";
  case id::contin_if:
    return out << "contin_if";
  case id::plus:
    return out << "+";
  case id::times:
    return out << "*";
  case id::div:
    return out << "/";
  case id::log:
    return out << "log";
  case id::exp:
    return out << "exp";
  case id::sin:
    return out << "sin";
  case id::greater_than_zero:
    return out << "0<";
  case id::impulse:
    return out << "impulse";
  default:
    return out << "BUILTIN : UNKNOWN_HANDLE";
  }
}

std::ostream& operator<<(std::ostream& out,const combo::argument& a) {
  if (a.is_negated())
    return out << "not(#" << -a.idx << ")";
  return out << "#" << a.idx;
}

std::ostream& operator<<(std::ostream& out,const combo::vertex& v) {
  if (const combo::argument* a=boost::get<combo::argument>(&v))
    return out << (*a);
  if (const combo::builtin* h=boost::get<combo::builtin>(&v))
    return out << (*h);
  if (const combo::action* act=boost::get<combo::action>(&v))
    return out << (*act);
  if (const combo::perception* per=boost::get<combo::perception>(&v))
    return out << (*per);
  return out << boost::get<combo::contin_t>(v);
}

std::istream& operator>>(std::istream& in,combo::vertex& v) {
  using namespace combo;
  std::string str;
  in >> str;
  assert(!str.empty());
  //builtin
  if (str=="and")
    v=id::logical_and;
  else if (str=="or")
    v=id::logical_or;
  else if (str=="not")
    v=id::logical_not;
  else if (str=="true")
    v=id::logical_true;
  else if (str=="false")
    v=id::logical_false;
  else if (str=="contin_if")
    v=id::contin_if;
  else if (str=="+")
    v=id::plus;
  else if (str=="*")
    v=id::times;
  else if (str=="/")
    v=id::div;
  else if (str=="log")
    v=id::log;
  else if (str=="exp")
    v=id::exp;
  else if (str=="sin")
    v=id::sin;
  else if (str=="0<")
    v=id::greater_than_zero;
  else if (str=="impulse")
    v=id::impulse;
  else if (str=="null_vertex")
    v=id::null_vertex;
  //action
  else if (str=="and_seq")
    v=id::sequential_and;
  else if (str=="or_seq")
    v=id::sequential_or;
  else if (str=="exec_seq")
    v=id::sequential_exec;
  else if (str=="boolean_action_if")
    v=id::boolean_action_if;
  else if (str=="contin_action_if")
    v=id::contin_action_if;
  else if (str=="action_action_if")
    v=id::action_action_if;
  else if (str=="failure")
    v=id::action_failure;
  else if (str=="success")
    v=id::action_success;
  else if (str=="dummy_action")
    v=id::dummy_action;
  //perception
  else if (str=="dummy_ultrametric")
    v=id::dummy_ultrametric;
  else if (str=="dummy_transitive")
    v=id::dummy_transitive;
  else if (str=="dummy_irreflexive")
    v=id::dummy_irreflexive;
  else if (str=="dummy_reflexive")
    v=id::dummy_reflexive;
  else if (str=="dummy_identity_of_indiscernibles")
    v=id::dummy_identity_of_indiscernibles;
  //argument
  else if (str[0]=='#') {
    v=argument(boost::lexical_cast<int>(str.substr(1)));
    assert(get_argument(v).idx!=0);
  }
  //constant
  else {
    v=boost::lexical_cast<contin_t>(str);
  }
  return in;
}

namespace combo {

  void copy_without_null_vertices(vtree::iterator src,
				  vtree& dst_tr,vtree::iterator dst) {
    *dst=*src;
    for (vtree::sibling_iterator sib=src.begin();sib!=src.end();++sib)
      if (*sib!=id::null_vertex)
	copy_without_null_vertices(sib,dst_tr,dst_tr.append_child(dst));
  }

} //~namespace combo
