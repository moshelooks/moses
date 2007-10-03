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

#ifndef IO_UTIL_H
#define IO_UTIL_H

#include <iostream>
#include <string>
#include <vector>

#include "tree.h"

#include "basic_types.h"
#include <boost/lexical_cast.hpp>

tree<std::string> parse_string_tree(const std::string& str);

std::istream& operator>>(std::istream& in,tree<std::string>& t);

template<typename T> 
void print_tree(const tree<T>& t,std::ostream& out=std::cout);

template<typename T1,typename T2>
void tree_convert(const tree<T1>& src,tree<T2>& dst);

template<typename T>
std::istream& operator>>(std::istream& in,tree<T>& t);

template<typename T>
std::istream& operator>>(std::istream& in,std::vector<T>& v);

template<typename T>
void print(const T& t) { std::cout << t << " "; }

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v);

std::ostream& operator<<(std::ostream& out, const std::vector<bool>& v);

/*** implementation ***/

template<typename T> 
void print_tree(const tree<T>& t,std::ostream& out=std::cout) {
  for (typename tree<T>::iterator it=t.begin();it!=t.end();++it) {
    for(int i=0; i<t.depth(it); ++i) 
      out << " ";
    out << (*it) << std::endl;
  }
}

template<typename T1,typename T2>
class TreeConverter {
public:
  void operator()(const tree<T1>& src,tree<T2>& dst) const {
    dst=tree<T2>(T2());
    typename tree<T1>::iterator src_it=src.begin();
    typename tree<T2>::iterator dst_it=dst.begin();
    while (src_it!=src.end()) {
      dst_it=dst.insert_after(dst_it,T2());
      recRepl(src,src_it,dst,dst_it);
      src_it.skip_children();
      ++src_it;
    }
    dst.erase(dst.begin());
  }
private:
  template<typename src_iterator,typename dst_iterator>
  void recRepl(const tree<T1>& src,src_iterator src_it,
	       tree<T2>& dst,dst_iterator dst_it) const {
    dst_it=dst.replace(dst_it,boost::lexical_cast<T2>(*src_it));
    for (typename tree<T1>::sibling_iterator sib=src.begin(src_it);
	 sib!=src.end(src_it);++sib)
      recRepl(src,sib,dst,dst.append_child(dst_it));
  }
};
template<typename T1,typename T2>
void tree_convert(const tree<T1>& src,tree<T2>& dst) { 
  TreeConverter<T1,T2>()(src,dst); 
}

template<typename T>
std::istream& operator>>(std::istream& in,tree<T>& t) {
  tree<std::string> tmp;
  in >> tmp;
  try {
    tree_convert(tmp,t);
  } catch (boost::bad_lexical_cast&) {
    std::cerr << "bad node data in tree " << std::endl << t << std::endl;
    exit(1);
  }
  return  in;
}

template<typename First,typename Second>
std::istream& operator>>(std::istream& in,std::pair<First,Second>& p) {
  in >> p.first;
  in >> p.second;
  return in;
}

template<typename T>
std::istream& operator>>(std::istream& in,std::vector<T>& v) {
  v.clear();
  while (in.good()) {
    T tmp;
    in >> tmp;
    if (!in.good())
      break;
    v.push_back(tmp);
  }
  return in;
}

template<typename T>
std::ostream& operator<<(std::ostream& out, const std::vector<T>& v) {
  out << "[ ";
  for (typename std::vector<T>::const_iterator it=v.begin();it!=v.end();++it)
    out << (*it) << " ";
  out << "]";
  return out;
}

#endif
