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

#ifndef _COMBO_VERTEX_H
#define _COMBO_VERTEX_H

//#include "combo/using.h"

#include <boost/functional/hash.hpp>
#include <boost/variant.hpp>
#include <boost/range.hpp>

#include "util/hash_set.h"

#include <iostream>

#include "util/tree.h"
#include "util/numeric.h"

#include "combo/action.h"
#include "perception.h"


namespace combo {
  //this idiom allows builtin to live in namespace combo, but all 
  //ids to be hidden inside the namespace id
  namespace id {
    enum builtin { 
      null_vertex=0,
      logical_and,logical_or,logical_not,logical_true,logical_false,
      contin_if,
      plus,times,div,exp,log,sin,
      greater_than_zero,
      impulse,
      builtin_count //to get the number of builtin
    };
  }
  typedef id::builtin builtin;

  /*
    class argument
      represents the index, attribute idx, of an input variable
      of a function coded into a combo tree. In the case of a boolean
      argument a negative value corresponds to a negative literal.
      idx == 0 is invalide.
      For example idx == -3 represents the literal NOT(#3) where #3 is the
      third argument of the function.
  */
  class argument { 
  public:
    explicit argument(int i) : idx(i) { assert(idx!=0); } 
    int idx;

    void negate() { idx=-idx; }
    bool is_negated() const { return idx<0; }
    bool operator<(argument rhs) const { 
      static util::absolute_value_order<int> comp;
      return comp(idx,rhs.idx);
    }
    bool operator==(argument rhs) const { return idx==rhs.idx; }
    bool operator!=(argument rhs) const { return idx!=rhs.idx; }
  };

  typedef double contin_t;

  typedef boost::variant<builtin,argument,contin_t,action,perception> vertex;
} //~namespace combo

std::ostream& operator<<(std::ostream&,const combo::builtin&);
std::ostream& operator<<(std::ostream&,const combo::argument&);
std::ostream& operator<<(std::ostream&,const combo::vertex&);

std::istream& operator>>(std::istream&,combo::vertex&);

inline bool operator==(const combo::vertex& v,combo::builtin h) {
  if (const combo::builtin* vh=boost::get<combo::builtin>(&v))
    return (*vh==h);
  return false;
}
inline bool operator==(combo::builtin h,const combo::vertex& v) { 
  return (v==h); 
}
inline bool operator!=(const combo::vertex& v,combo::builtin h) {
  return !(v==h);
}
inline bool operator!=(combo::builtin h,const combo::vertex& v) { 
  return !(v==h); 
}


//WARNING : should be action.h but could not do that due to dependency issues
inline bool operator==(const combo::vertex& v,combo::action a) {
  if (const combo::action* va=boost::get<combo::action>(&v))
    return (*va==a);
  return false;
}
//WARNING : should be action.h but could not do that due to dependency issues
inline bool operator==(combo::action a,const combo::vertex& v) {
  return (v==a);
}
//WARNING : should be action.h but could not do that due to dependency issues
inline bool operator!=(const combo::vertex& v,combo::action a) {
  return !(v==a);
}
//WARNING : should be action.h but could not do that due to dependency issues
inline bool operator!=(combo::action a,const combo::vertex& v) {
  return !(v==a);
}

//WARNING : should be perception.h but could not do that due to dependency issues
inline bool operator==(const combo::vertex& v,combo::perception p) {
  if (const combo::perception* vp=boost::get<combo::perception>(&v))
    return (*vp==p);
  return false;
}
//WARNING : should be perception.h but could not do that due to dependency issues
inline bool operator==(combo::perception p,const combo::vertex& v) {
  return (v==p);
}
//WARNING : should be perception.h but could not do that due to dependency issues
inline bool operator!=(const combo::vertex& v,combo::perception p) {
  return !(v==p);
}
//WARNING : should be perception.h but could not do that due to dependency issues
inline bool operator!=(combo::perception p,const combo::vertex& v) {
  return !(v==p);
}

namespace boost {
  //don't know why this is needed *in namespace boost*, but it is,
  //for e.g. calling a generic stl function that compares vertices for inequality
  inline bool operator!=(const combo::vertex& v1,const combo::vertex& v2) { 
    return !(v1==v2); 
  }

  inline size_t hash_value(const combo::vertex& v) {
    static const size_t action_plus_builtin_count=
      size_t(combo::id::builtin_count)+size_t(combo::id::action_count);
    static const size_t action_plus_perception_plus_builtin_count=
      action_plus_builtin_count+size_t(combo::id::perception_count);
    static const size_t max_arg_guess_plus_count=
      action_plus_perception_plus_builtin_count
      +
      15; //this should be not too be but more the the number of arguments than a
          //function is likely to ever have

    if (const combo::builtin* h=boost::get<combo::builtin>(&v))
      return size_t(*h);
    if (const combo::action* act=boost::get<combo::action>(&v))
      return size_t(*act)+size_t(combo::id::builtin_count);
    if (const combo::perception* per=boost::get<combo::perception>(&v))
      return size_t(*per)+action_plus_builtin_count;
    if (const combo::argument* a=boost::get<combo::argument>(&v))
      return size_t(a->idx*(a->is_negated()+2)) +
	action_plus_perception_plus_builtin_count;
    assert(boost::get<combo::contin_t>(&v));

    size_t tmp=max_arg_guess_plus_count;
    hash_combine(tmp,hash_value(boost::get<combo::contin_t>(v)));
    return tmp;
  }
} //~namespace boost

namespace combo { 
  typedef util::hash_set<vertex,boost::hash<vertex> > vset;
  //typedef std::set<vertex> vset;
  typedef util::tree<vertex> vtree;

  template<typename T>
  inline bool is_associative(const T& v) { 
    return (v==id::logical_and || v==id::logical_or ||
	    v==id::plus || v==id::times ||
	    //actions
	    v==id::sequential_and || v==id::sequential_or ||
	    v==id::sequential_exec);
  }
  template<typename T>
  inline bool is_commutative(const T& v) { 
    return (v==id::logical_and || v==id::logical_or ||
	    v==id::plus || v==id::times);
  }

  //properties of perceptions
  template<typename T>
  inline bool is_ultrametric(const T& v) { 
    return (v==id::dummy_ultrametric);
  }  
  template<typename T>
  inline bool is_transitive(const T& v) { 
    return (v==id::dummy_transitive);
  }

  template<typename T>
  inline bool is_irreflexive(const T& v) { 
    return (v==id::dummy_irreflexive);
  }  

  template<typename T>
  inline bool is_reflexive(const T& v) { 
    return (v==id::dummy_reflexive);
  }

  template<typename T>
  inline bool is_identity_of_indiscernibles(const T& v) { 
    return (v==id::dummy_identity_of_indiscernibles);
  }

  inline bool is_builtin(const vertex& v) {
    return (boost::get<builtin>(&v));
  }
  
  inline builtin get_builtin(const vertex& v) {
    return (boost::get<builtin>(v));
  }
  
  inline bool is_contin(const vertex& v) { 
    return (boost::get<contin_t>(&v)); 
  }
  inline contin_t get_contin(const vertex& v) { 
    return (boost::get<contin_t>(v));
  }
  inline bool is_argument(const vertex& v) {
    return (boost::get<argument>(&v)); 
  }
  inline argument& get_argument(vertex& v) {
    return boost::get<argument>(v); 
  }
  inline const argument& get_argument(const vertex& v) {
    return boost::get<argument>(v); 
  }
  inline bool is_negated(vertex& v) {
    if (argument* a=boost::get<argument>(&v))
      return a->is_negated();
    return false;
  }

  
  //WARNING : should be action.h but could not do that due to dependency issues
  inline bool is_action(const vertex& v) {
    return (boost::get<action>(&v));
  }
  
  //WARNING : should be action.h but could not do that due to dependency issues
  inline action get_action(const vertex& v) {
    return (boost::get<action>(v));
  }

  //WARNING : should be action.h but could not do that due to dependency issues
  inline bool is_action_result(const vertex& v) {
    //can add other actions that do not take arguments
    return (v==id::action_failure || v==id::action_success);
  }

  /*NOT SURE IT IS NEEDED
  //WARNING : should be perception.h but could not do that due to dependency issues
  inline bool is_perception(const vertex& v) {
    return (boost::get<perception>(&v));
  }
  
  //WARNING : should be perception.h but could not do that due to dependency issues
  inline action get_perception(const vertex& v) {
    return (boost::get<perception>(v));
    }*/

  inline vertex bool_to_vertex(bool b) { 
    return (b ? id::logical_true : id::logical_false); 
  }
  inline bool vertex_to_bool(const vertex& v) {
    assert(v==id::logical_true || v==id::logical_false);
    return (v==id::logical_true);
  }
  inline vertex negate(const vertex& v) {
    assert(v==id::logical_true || v==id::logical_false);
    return (v==id::logical_true ? id::logical_false : id::logical_true);
  }

  inline bool is_complement(const vertex& x,const vertex& y) {
    if (const argument* ax=boost::get<argument>(&x)) {
      if (const argument* ay=boost::get<argument>(&y)) {
	return (ax->idx==-ay->idx);
      }
    }
    return false;
  }

  template<typename T>
  inline bool is_boolean(const T& v) {
    return (v==id::logical_true || v==id::logical_false);
  }
  template<typename T>
  inline bool is_constant(const T& v) { 
    return (is_boolean(v) || is_contin(v) || is_action_result(v));
  }

  //copy a vtree, ignoring subtrees rooted in null vertices
  void copy_without_null_vertices(vtree::iterator src,
				  vtree& dst_tr,vtree::iterator dst);

  inline bool may_have_side_effects(vtree::iterator /*it*/) {
    return false;
  }

} //~namespace combo

namespace boost {
  template<>
  struct range_iterator<combo::vtree::pre_order_iterator> {
    typedef boost::counting_iterator<combo::vtree::sibling_iterator> type;
  };
  template<>
  struct range_const_iterator<combo::vtree::pre_order_iterator> {
    typedef boost::counting_iterator<combo::vtree::sibling_iterator> type;
  };

  template<>
  struct range_iterator<combo::vtree> {
    typedef boost::counting_iterator<combo::vtree::iterator> type;
  };
  template<>
  struct range_const_iterator<combo::vtree> {
    typedef boost::counting_iterator<combo::vtree::iterator> type;
  };

  /*  template<>
  struct range_iterator<combo::vtree::p_iterator> {
    typedef boost::counting_iterator<combo::vtree::sibling_iterator> type;
  };
  template<>
  struct range_const_iterator<combo::vtree::pre_order_iterator> {
    typedef boost::counting_iterator<combo::vtree::sibling_iterator> type;
    };*/
} //~namespace boost



#endif
