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

#include "io_util.h"
#include <boost/spirit/core.hpp>

namespace {
  using namespace boost::spirit;
  using std::string;

  tree<string> tr;
  tree<string>::iterator at=tr.begin();

  void begin_internal(const char* from, const char* to) {
    at= tr.empty() 
      ? tr.insert(at,string(from,to-1))
      : tr.append_child(at,string(from,to-1));
  }
  void end_internal(const char) {
    at=tr.parent(at);
  }
  void add_leaf(const char* from, const char* to) {
    if (tr.empty())
      at=tr.insert(at,string(from,to));
    else
      tr.append_child(at,string(from,to));
  }

  struct TreeGrammar : public grammar<TreeGrammar> {
    std::vector<std::string> v;
    
    template<typename ScannerT>
    struct definition {
      definition(const TreeGrammar&) {
	term= 
	  lexeme_d[(+( anychar_p - ch_p('(') - ch_p(')') - space_p))]
	  [&add_leaf];
	beg=
	  lexeme_d[(+( anychar_p - ch_p('(') - ch_p(')') - space_p)) >> '('];
	expr=
	  (beg[&begin_internal] >> +expr >> ch_p(')')[&end_internal]) |
	  term;
	//expr=term | (term >> '(' >> +expr >> ')');
      }
      rule<ScannerT> expr,beg,term;
      
      const rule<ScannerT>& start() const { return expr; }
    };
  };

} //~namespace

tree<std::string> parse_string_tree(const std::string& str) {
  TreeGrammar tg;
  tr.clear();
  at=tr.begin();
  parse(str.c_str(),tg,space_p);
  
  tree<std::string> tmp(tr);
  tr.clear();
  return tmp;
}

std::istream& operator>>(std::istream& in,tree<std::string>& t) {
  t.clear();
  std::string str,tmp;
  int nparen=0;
  do {
    in >> tmp;
    nparen+=count(tmp.begin(),tmp.end(),'(')-count(tmp.begin(),tmp.end(),')');
    str+=tmp+' ';
  } while (in.good() && nparen>0);
  if (nparen!=0) {
    std::cerr << "Paren mismatch parsing tree: '" << str << "'" << std::endl;
    exit(1);
  }
  t=parse_string_tree(str);
  return in;
}

std::ostream& operator<<(std::ostream& out, const std::vector<bool>& v) {
  out << "[ ";
  for (std::vector<bool>::const_iterator it=v.begin();it!=v.end();++it)
    out << (int)(*it);
  out << "]";
  return out;
}
