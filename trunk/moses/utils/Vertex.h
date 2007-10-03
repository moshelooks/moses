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

#ifndef VERTEX_H
#define VERTEX_H

#include <boost/variant.hpp>
#include <boost/functional/hash.hpp>
#include <boost/lexical_cast.hpp>
#include <ext/hash_set>
#include "tree.h"
#include "id.h"
#include "basic_types.h"

#include <list>

#include <iostream>

enum Handle { 
  __vertex_ant_progn=1,__vertex_ant_if_food,__vertex_ant_move,__vertex_ant_right,__vertex_ant_left,__vertex_ant_reversal,
  __vertex_logical_and,__vertex_logical_or,__vertex_logical_not,__vertex_logical_true,__vertex_logical_false,__vertex_bool_if,

  __vertex_plus,__vertex_times,__vertex_inv,__vertex_log,__vertex_exp,__vertex_sin,

  __vertex_sim_teacher,__vertex_sim_ball,__vertex_sim_near,__vertex_sim_holding,
  __vertex_sim_walktowards,__vertex_sim_pickup,__vertex_sim_done,__vertex_sim_identity,__vertex_ifelse
};
class Argument { 
public:
  Argument(int i) : idx(i) { } 
  int idx;
  bool operator<(Argument rhs) const { return idx<rhs.idx; }
  bool operator==(Argument rhs) const { return idx==rhs.idx; }
};
inline std::ostream& operator<<(std::ostream& out,const Argument& a) {
  return (out << "#" << a.idx);
}
typedef boost::variant<Handle,Argument,contin_t> Vertex;
namespace boost {
  inline size_t hash_value(const Vertex& v) {
    if (const Handle* h=boost::get<Handle>(&v))
      return (size_t)(*h);
    return hash_value(boost::get<Argument>(v).idx);
  }
}
//typedef __vertex_gnu_cxx::hash_set<Vertex,boost::hash<Vertex> > VertexSet;
typedef std::set<Vertex> VertexSet;
typedef tree<Vertex> vtree;

inline bool operator==(const Vertex& v,Handle h) {
  if (const Handle* vh=boost::get<Handle>(&v))
    return (*vh==h);
  return false;
}

namespace id {
  //haxx

  template<> inline bool is_associative(const Vertex& v) { 
    return (v==__vertex_ant_progn || v==__vertex_logical_and || v==__vertex_logical_or);
  }
  template<> inline bool is_commutative(const Vertex& v) { 
    return (v==__vertex_logical_and || v==__vertex_logical_or);
  }

  template<> inline bool is_constant(const Vertex& v) { 
    return (boost::get<contin_t>(&v)); 
  }

  inline bool is_progn(Handle h) { return (h==__vertex_ant_progn); }
  inline bool is_if_food(Handle h) { return (h==__vertex_ant_if_food); }
  inline bool is_move(Handle h) { return (h==__vertex_ant_move); }
  inline bool is_right(Handle h) { return (h==__vertex_ant_right); }
  inline bool is_left(Handle h) { return (h==__vertex_ant_left); }
  inline bool is_reversal(Handle h) { return (h==__vertex_ant_reversal); }

  inline bool is_turn(Handle h) { 
    return (h==__vertex_ant_right || h==__vertex_ant_left || h==__vertex_ant_reversal);
  }

  inline bool is_near(Handle h) { return (h==__vertex_sim_near); }
  inline bool is_identity(Handle h) { return (h==__vertex_sim_identity); }

  inline bool is_teacher(Handle h) { return (h==__vertex_sim_teacher); }
  inline bool is_ball(Handle h) { return (h==__vertex_sim_ball); }

  inline bool is_holding(Handle h) { return (h==__vertex_sim_holding); }
  inline bool is_walktowards(Handle h) { return (h==__vertex_sim_walktowards); }
  inline bool is_pickup(Handle h) { return (h==__vertex_sim_pickup); }
  inline bool is_done(Handle h) { return (h==__vertex_sim_done); }

  inline bool is_ifelse(Handle h) { return (h==__vertex_ifelse); }

  template<> inline bool is_and(const Handle& h) { return (h==__vertex_logical_and); }
  template<> inline bool is_or(const Handle& h) { return (h==__vertex_logical_or); }
  template<> inline bool is_not(const Handle& h) { return (h==__vertex_logical_not); }

  template<> inline bool is_near(const Vertex& h) { return (h==__vertex_sim_near); }
  template<> inline bool is_identity(const Vertex& h) { return (h==__vertex_sim_identity); }

  template<> inline bool is_teacher(const Vertex& h) { return (h==__vertex_sim_teacher); }
  template<> inline bool is_ball(const Vertex& h) { return (h==__vertex_sim_ball); }

  template<> inline bool is_holding(const Vertex& h) { return (h==__vertex_sim_holding); }
  template<> inline bool is_walktowards(const Vertex& h) { return (h==__vertex_sim_walktowards); }
  template<> inline bool is_pickup(const Vertex& h) { return (h==__vertex_sim_pickup); }
  template<> inline bool is_done(const Vertex& h) { return (h==__vertex_sim_done); }

  template<> inline bool is_ifelse(const Vertex& h) { return (h==__vertex_ifelse); }


  template<> inline bool is_plus(const Vertex& v) { return (v==__vertex_plus); }
  template<> inline bool is_times(const Vertex& v) { return (v==__vertex_times); }
  template<> inline bool is_inv(const Vertex& v) { return (v==__vertex_inv); }
  template<> inline bool is_log(const Vertex& v) { return (v==__vertex_log); }
  template<> inline bool is_exp(const Vertex& v) { return (v==__vertex_exp); }
  template<> inline bool is_sin(const Vertex& v) { return (v==__vertex_sin); }
  
  template<> inline Vertex create_plus<Vertex>() { 
    return Vertex(__vertex_plus); }
  template<> inline Vertex create_times<Vertex>() { 
    return Vertex(__vertex_times); }
  template<> inline Vertex create_inv<Vertex>() { 
    return Vertex(__vertex_inv); }
  template<> inline Vertex create_log<Vertex>() { 
    return Vertex(__vertex_log); }
  template<> inline Vertex create_exp<Vertex>() { 
    return Vertex(__vertex_exp); }
  template<> inline Vertex create_sin<Vertex>() { 
    return Vertex(__vertex_sin); }

  template<typename T>
  T create_const(contin_t c) { return Vertex(c); }
  template<> inline contin_t get_const(const Vertex& v) { 
    return (boost::get<contin_t>(v));
  }

  template<> inline bool is_true(const Vertex& v) { 
    return (v==__vertex_logical_true); }
  template<> inline bool is_false(const Vertex& v) { 
    return (v==__vertex_logical_false); }
  template<> inline bool is_bool_if(const Vertex& v) { 
    return (v==__vertex_bool_if); }

  template<> inline bool is_variable(const Vertex& v) { 
    return (!boost::get<Handle>(&v)); }

  template<> inline bool is_argument(const Vertex& v) {
    return (boost::get<Argument>(&v)); }

  template<> inline int get_argument_idx(const Vertex& v) {
    return boost::get<Argument>(v).idx; }
  
  inline bool is_progn(const Vertex& v) { return (v==__vertex_ant_progn); }
  inline bool is_if_food(const Vertex& v) { return (v==__vertex_ant_if_food); }
  inline bool is_move(const Vertex& v) { return (v==__vertex_ant_move); }
  inline bool is_right(const Vertex& v) { return (v==__vertex_ant_right); }
  inline bool is_left(const Vertex& v) { return (v==__vertex_ant_left); }
  inline bool is_reversal(const Vertex& v) { return (v==__vertex_ant_reversal); }
  inline bool is_turn(const Vertex& v) { 
    return (v==__vertex_ant_right || v==__vertex_ant_left || v==__vertex_ant_reversal);
  }

  inline bool is_near(const Vertex& v) { return (v==__vertex_sim_near); }
  inline bool is_identity(const Vertex& v) { return (v==__vertex_sim_identity); }

  template<> inline bool is_and(const Vertex& v) { return (v==__vertex_logical_and); }
  template<> inline bool is_or(const Vertex& v) { return (v==__vertex_logical_or); }
  template<> inline bool is_not(const Vertex& v) { return (v==__vertex_logical_not); }

  template<> inline Vertex create_progn<Vertex>() { 
    return Vertex(__vertex_ant_progn); }
  template<> inline Vertex create_if_food<Vertex>() { 
    return Vertex(__vertex_ant_if_food); }
  template<> inline Vertex create_move<Vertex>() { 
    return Vertex(__vertex_ant_move); }
  template<> inline Vertex create_right<Vertex>() { 
    return Vertex(__vertex_ant_right); }
  template<> inline Vertex create_left<Vertex>() { 
    return Vertex(__vertex_ant_left); }
  template<> inline Vertex create_reversal<Vertex>() { 
    return Vertex(__vertex_ant_reversal); }

  template<> inline Vertex create_and<Vertex>() { 
    return Vertex(__vertex_logical_and); }
  template<> inline Vertex create_or<Vertex>() { 
    return Vertex(__vertex_logical_or); }
  template<> inline Vertex create_not<Vertex>() { 
    return Vertex(__vertex_logical_not); }

  template<> inline Vertex create_true<Vertex>() { 
    return Vertex(__vertex_logical_true); }
  template<> inline Vertex create_false<Vertex>() { 
    return Vertex(__vertex_logical_false); }
  template<> inline Vertex create_bool_if<Vertex>() { 
    return Vertex(__vertex_bool_if); }

  template<> inline Vertex create_argument<Vertex>(int idx) {
    return Vertex(Argument(idx)); }

  template<> inline Vertex create_teacher() { return Vertex(__vertex_sim_teacher); }
  template<> inline Vertex create_ball() { return Vertex(__vertex_sim_ball); }

  template<> inline Vertex create_near() { return Vertex(__vertex_sim_near); }
  template<> inline Vertex create_identity() { return Vertex(__vertex_sim_identity); }
  
  template<> inline Vertex create_holding() { return Vertex(__vertex_sim_holding); }
  template<> inline Vertex create_walktowards() { 
    return Vertex(__vertex_sim_walktowards); }
  template<> inline Vertex create_done() { return Vertex(__vertex_sim_done); }
  template<> inline Vertex create_pickup() { return Vertex(__vertex_sim_pickup); }
  template<> inline Vertex create_ifelse() { return Vertex(__vertex_ifelse); }

} //~namespace id

inline std::ostream& operator<<(std::ostream& out,const Vertex& v) {
  using namespace id;
  if (const Argument* a=boost::get<Argument>(&v))
    return out << (*a);
  Handle h=boost::get<Handle>(v);
  switch(h) {
  case __vertex_ant_progn:
    return out << "progn";
  case __vertex_ant_if_food:
    return out << "if_food";
  case __vertex_ant_move:
    return out << "m";
  case __vertex_ant_right:
    return out << "r";
  case __vertex_ant_left:
    return out << "l";
  case __vertex_ant_reversal:
    return out << "rev";
  case __vertex_logical_and:
    return out << "and";
  case __vertex_logical_or:
    return out << "or";
  case __vertex_logical_not:
    return out << "not";
  case __vertex_logical_true:
    return out << "true";
  case __vertex_logical_false:
    return out << "false";
  case __vertex_bool_if:
    return out << "if";
  case __vertex_sim_near:
    return out << "near";
  case __vertex_sim_holding:
    return out << "holding";
  case __vertex_sim_walktowards:
    return out << "walktowards";
  case __vertex_sim_pickup:
    return out << "pickup";
  case __vertex_sim_done:
    return out << "done";
  case __vertex_sim_identity:
    return out << "identical";
  case __vertex_sim_teacher:
    return out << "teacher";
  case __vertex_sim_ball:
    return out << "ball";
  case __vertex_ifelse:
    return out << "ifelse";
  case __vertex_plus:
    return out << "+";
  case __vertex_times:
    return out << "+";
  case __vertex_inv:
    return out << "inv";
  case __vertex_log:
    return out << "log";
  case __vertex_exp:
    return out << "exp";
  case __vertex_sin:
    return out << "sin";
  }
  return out << "UNKNOWN_HANDLE";
}

inline std::istream& operator>>(std::istream& in,Vertex& v) {
  using namespace id;
  std::string str;
  in >> str;
  if (str=="progn")
    v=create_progn<Vertex>();
  else if (str=="if")
    v=create_bool_if<Vertex>();
  else if (str=="if_food")
    v=create_if_food<Vertex>();
  else if (str=="m")
    v=create_move<Vertex>();
  else if (str=="l")
    v=create_left<Vertex>();
  else if (str=="r")
    v=create_right<Vertex>();
  else if (str=="rev")
    v=create_reversal<Vertex>();
  else if (str=="and")
    v=create_and<Vertex>();
  else if (str=="or")
    v=create_or<Vertex>();
  else if (str=="not")
    v=create_not<Vertex>();
  else if (str=="true")
    v=create_true<Vertex>();
  else if (str=="false")
    v=create_false<Vertex>();
  else if (str=="near")
    v=create_near<Vertex>();
  else if (str=="identical")
    v=create_identity<Vertex>();

  else if (str=="done")
    v=create_done<Vertex>();
  else if (str=="teacher")
    v=create_teacher<Vertex>();
  else if (str=="ball")
    v=create_ball<Vertex>();
  else if (str=="holding")
    v=create_holding<Vertex>();
  else if (str=="walktowards")
    v=create_walktowards<Vertex>();
  else if (str=="ifelse")
    v=create_ifelse<Vertex>();
  else if (str=="pickup")
    v=create_pickup<Vertex>();

  else if (str=="+")
    v=create_plus<Vertex>();
  else if (str=="*")
    v=create_times<Vertex>();
  else if (str=="inv")
    v=create_inv<Vertex>();
  else if (str=="exp")
    v=create_exp<Vertex>();
  else if (str=="log")
    v=create_log<Vertex>();
  else if (str=="sin")
    v=create_sin<Vertex>();

  else if (str[0]=='#')
    v=create_argument<Vertex>(boost::lexical_cast<int>(str.substr(1)));
  return in;
}

#endif
