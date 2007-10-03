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

#ifndef _COMBO_EVAL_H
#define _COMBO_EVAL_H

#include "combo/vertex.h"
#include "combo/tree_type.h"
#include "util/tree.h"
#include "util/numeric.h"
#include "util/hash_map.h"
#include "util/exception.h"
#include "util/foreach.h"
#include "combo/using.h"
#include "combo/action_eval.h"

#define COEF_SAMPLE_COUNT 20.0 //involved in the formula that counts
                              //the number of trials needed to check a formula

namespace combo {
  //ALREADY DEFINED IN action_eval.h
  //inline vertex& binding(int idx) {
  //  static util::hash_map<int,vertex> map;
  //  return map[idx];
  //}

  template<typename It>
  vertex eval_throws(It it) throw(EvalException) { //could be made faster by doing dispatch
    typedef typename It::sibling_iterator sib_it;
    double res = 0;

    if (is_argument(*it)) {
      int idx=get_argument(*it).idx;
      //assumption : when idx is negative the argument is necessary boolean
      return (idx>0 ? binding(idx) : negate(binding(-idx)));
    } 
    //boolean operators
    else if (*it==id::logical_and) {
      for (sib_it sib=it.begin();sib!=it.end();++sib)
	if (eval_throws(sib)==id::logical_false)
	  return id::logical_false;
      return id::logical_true;
    } 
    else if (*it==id::logical_or) {
      for (sib_it sib=it.begin();sib!=it.end();++sib)
	if (eval_throws(sib)==id::logical_true)
	  return id::logical_true;
      return id::logical_false;
    }
    else if (*it==id::logical_not) {
      assert(it.has_one_child());
      return negate(eval_throws(it.begin()));
    }
    //mixed operators
    else if (*it==id::contin_if) {
      assert(it.number_of_children() == 3);
      sib_it sib=it.begin();
      vertex vcond = eval_throws(sib);
      assert(is_boolean(vcond));
      ++sib;
      if (vcond == id::logical_true) {
	//TODO : check the type of the result
	return eval_throws(sib);
      }
      else {
	++sib;
	//TODO : check the type of the result
	return eval_throws(sib);
      }
    }
    else if (*it==id::greater_than_zero) {
      assert(it.has_one_child());
      sib_it sib=it.begin();
      vertex x = eval_throws(sib);
      assert(is_contin(x));
      return bool_to_vertex(0 < get_contin(x));
    }
    else if (*it==id::impulse) {
      vertex i;
      assert(it.has_one_child());
      i = eval_throws(it.begin());
      assert(is_boolean(i));
      return (i==id::logical_true? 1.0 : 0.0);
    }
    //continuous operator
    else if (*it==id::plus) {
      res = 0;
      //assumption : plus can have 1 or more arguments
      for (sib_it sib=it.begin(); sib!=it.end(); ++sib) {
	vertex vres = eval_throws(sib);
	assert(is_contin(vres));
	res += get_contin(vres);
      }
    }
    else if (*it==id::times) {
      res = 1;
       //assumption : times can have 1 or more arguments
      for (sib_it sib=it.begin(); sib!=it.end(); ++sib) {
	vertex vres = eval_throws(sib);
	assert(is_contin(vres));
	res *= get_contin(vres);
      }
    }
    else if (*it==id::div) {
      contin_t x, y;
      assert(it.number_of_children()==2);
      sib_it sib=it.begin();
      vertex vx = eval_throws(sib);
      assert(is_contin(vx));
      x = get_contin(vx);
      ++sib;
      vertex vy = eval_throws(sib);
      assert(is_contin(vy));
      y = get_contin(vy);
      res = x/y;
    }
    else if (*it==id::log) {
      assert(it.has_one_child());
      vertex vx = eval_throws(it.begin());
      assert(is_contin(vx));
      contin_t x = get_contin(vx);
      res = log(x);
    }
    else if (*it==id::exp) {
      assert(it.has_one_child());
      vertex vx = eval_throws(it.begin());
      assert(is_contin(vx));
      res = exp(get_contin(vx));
    }
    else if (*it==id::sin) {
      assert(it.has_one_child());
      vertex vx = eval_throws(it.begin());
      assert(is_contin(vx));
      res = sin(get_contin(vx));
    }
    //action
    else if (is_action(*it)) {
      return action_eval_throws(it);
    }
    else if (it.is_childless()) {
      if (is_contin(*it)) return get_contin(*it);
      //assumption : no continuous or mixed operator has no child
      assert(*it!=id::contin_if && *it!=id::plus && *it!=id::times
	     && *it!=id::div && *it!=id::log && *it!=id::exp && *it!=id::sin
	     && *it!=id::greater_than_zero && *it!=id::impulse);
      //assumption : if the code is executed it is assumed that *it
      //is logical_true or logical_false
      //because the case and() and or() has already been treated above
      assert(*it==id::logical_true || *it==id::logical_false);
      return *it;
    }



    //An exception is thrown if res is NaN or inf
    if(isnan(res) || isinf(res)) throw EvalException(vertex(res));
    return res;
  }

  template<typename It>
  vertex eval(It it) throw() {
    try {
      return eval_throws(it);
    } catch (EvalException e) {
      return e.get_vertex();
    }
  }

  template<typename T>
  vertex eval(const util::tree<T>& tr) throw() {
    return eval(tr.begin()); 
  }

  template<typename T>
  vertex eval_throws(const util::tree<T>& tr) throw(EvalException) {
    return eval_throws(tr.begin());
  }
  
  //return the arity of a tree
  template<typename T>
  int arity(const util::tree<T>& tr) {
    int a = 0;
    for (typename util::tree<T>::iterator it=tr.begin();it!=tr.end();++it)
      if (is_argument(*it))
	a = std::max(a, std::abs(get_argument(*it).idx));
    return a;
  }
  
  /*
    this function take an arity in input and returns in output the number
    of samples that would be appropriate to check the semantics of its associated
    tree. (Note : could take the two trees to checking and according to their arity
    structure, whatever, find an appropriate number.)
  */
  inline int sample_count(int arity) {
    if(arity==0)
      return 1;
    else return (int)(COEF_SAMPLE_COUNT*log((float)arity + EXPONENTIAL));
  }


  //------------------------------------------------------------------------
  // Truth table
  //------------------------------------------------------------------------

  class truth_table : public std::vector<bool> {
  public:
    typedef std::vector<bool> super;

    truth_table() { }
    template<typename It>
    truth_table(It from,It to) : super(from,to) { }
    template<typename T>
    truth_table(const util::tree<T>& t,int arity)
      : super(util::power(2,arity)) { populate(t,arity); }
    template<typename T>
    truth_table(const util::tree<T>& t) {
      int a = arity(t);
      this->resize(util::power(2,a));
      populate(t,a);
    }

    template<typename Func>
    truth_table(const Func& f,int arity) : super(util::power(2,arity)) {
      iterator it=begin();
      for (int i=0;it!=end();++i,++it) {
	std::vector<bool> v(arity);
	for (int j=0;j<arity;++j)
	  v[j]=(i>>j)%2;
	(*it)=f(v.begin(),v.end());
      }
    }

    /*
      this operator allows to access quickly to the results of a
      truth_table. [from, to) points toward a chain of boolean describing
      the inputs of the function coded into the truth_table and
      the operator returns the results.
    */
    template<typename It>
    bool operator()(It from,It to) {
      const_iterator it=begin();
      for (int i=1;from!=to;++from,i=i<<1)
	if (*from)
	  it+=i;
      return *it;
    }

    size_type hamming_distance(const truth_table& other) const;
  protected:
    template<typename T>
    void populate(const util::tree<T>& tr,int arity) {
      iterator it=begin();
      for (int i=0;it!=end();++i,++it) {
	for (int j=0;j<arity;++j)
	  binding(j+1)=bool_to_vertex((i>>j)%2);
	(*it)=(eval(tr)==id::logical_true);
      }
    }
  };

  //-------------------------------------------------------------------------
  // contin table
  //-------------------------------------------------------------------------

  //shorthands used by class RndNumTable and contin_table
  typedef std::vector<contin_t> contin_vector;
  typedef contin_vector::iterator cv_it;
  typedef contin_vector::const_iterator const_cv_it;
  typedef std::vector<contin_vector> contin_matrix;
  typedef contin_matrix::iterator cm_it;
  typedef contin_matrix::const_iterator const_cm_it;
  
  /*
    class RndNumTable
      matrix of randomly generated contin_t of sample_count rows and arity columns
  */
  class RndNumTable {
  private:
    contin_matrix _matrix;
  public:
    //constructor
    RndNumTable() {}
    RndNumTable(int sample_count, int arity);

    //access method
    const contin_matrix& matrix() const { return _matrix; }
  };

  /*
    class contin_table
      contains sample_count evaluations obtained by evaluating t, a tree, over
      a RndNumTable rnt.
      assumption : t has only contin inputs and output
  */
  class contin_table : public contin_vector { //a column of results
  public:
    //typedef contin_vector super;

    //constructors
    contin_table() { }
    contin_table(const vtree& t, const RndNumTable& rnt);
    template<typename Func>
    contin_table(const Func& f, const RndNumTable& rnt) {
      foreach(const contin_vector& v,rnt.matrix())
	push_back(f(v.begin(),v.end()));
    }

    //equality operator
    bool operator==(const contin_table& ct) const;
    bool operator!=(const contin_table& ct) const { return !operator==(ct); }

    contin_t abs_distance(const contin_table& other) const;
  };

  //-------------------------------------------------------------------------
  // Mixed table
  //-------------------------------------------------------------------------

  class mixed_table {
    typedef tree_type::iterator type_pre_it;
    typedef tree_type::sibling_iterator type_sib_it;
    //Vector of bool or contin, depending on the output type of vtree
    //size = 2^|boolean inputs| * _rnt.size()
    //each slice of rnt.size() corresponds to a particular boolean input
    //all possible boolean inputs are enumerated in lexico order as for
    //truth_table
    //each element in a slice of rnt.size() corresponds to the result of
    //a particular contin input, all obtained from rnt
    std::vector<variant<bool,contin_t> > _vt;
    //NOTE : can be a bit optimized by using
    //variant<std::vector<bool>,std::vector<contin_t> > _vt;
    //instead

    int _contin_arg_count; //number of contin arguments
    int _bool_arg_count; //number of boolean arguments

    util::hash_map<int, int> _arg_map; //link the argument index with the
                                       //boolean or contin index, that is
                                       //for instance if the inputs are
                                       //(bool, contin, contin, bool, bool)
                                       //the the map is
                                       //(<0,0>,<1,0>,<2,1>,<3,1>,<4,2>)
    std::vector<int> _bool_arg; //link the ith bool arg with arg index
    std::vector<int> _contin_arg; //link the ith contin arg with arg index
    tree_type _prototype;
    node_type _output_type;

    RndNumTable _rnt;

    //take a prototype, set _prototype, _contin_arg_count, _bool_arg_count
    //_arg_map, _bool_arg, _contin_arg and _output_type
    void set_prototype(const tree_type& prototype) {
      _contin_arg_count = 0;
      _bool_arg_count = 0;
      _prototype = prototype;
      type_pre_it proto_it = prototype.begin();
      int arg_idx = 0;
      for(type_sib_it i=proto_it.begin();i!=proto_it.end();++i,++arg_idx) {
	if(type_pre_it(i)==_prototype.last_child(proto_it)) {
	  _output_type = *i;
	}
	else {
	  if(*i==id::boolean) {
	    std::pair<int, int> p(arg_idx, _bool_arg_count);
	    _arg_map.insert(p);
            _bool_arg.push_back(arg_idx);
	    _bool_arg_count++;
	  } else if(*i==id::contin) {
	    std::pair<int, int> p(arg_idx, _contin_arg_count);
	    _arg_map.insert(p);
            _contin_arg.push_back(arg_idx);
	    _contin_arg_count++;
	  }
	}
      }
    }

  public:
    //constructors
    mixed_table() {}
    mixed_table(const vtree& tr, const RndNumTable& rnt,
		const tree_type& prototype) {
      _rnt = rnt;
      if(prototype.empty()) {
	tree_type inferred_proto = infer_tree_type_fast(tr);
        set_prototype(inferred_proto);
      }
      else set_prototype(prototype);
      //populate _vt
      //works only for _bool_arg_count < 64
      unsigned long int bool_table_size = 1 << _bool_arg_count;
      for(unsigned long int i=0; i < bool_table_size; ++i) {
	for(int bai=0; bai < _bool_arg_count; ++bai) //bool arg index
	  binding(_bool_arg[bai]+1)=bool_to_vertex((i>>bai)%2);
	for(const_cm_it si=rnt.matrix().begin();si!=rnt.matrix().end();++si) {
	  int cai = 0; //contin arg index
	  for(const_cv_it j = (*si).begin(); j != (*si).end(); ++j, ++cai)
	    binding(_contin_arg[cai]+1)=*j;
	  vertex e = eval_throws(tr);
	  _vt.push_back(is_boolean(e)? vertex_to_bool(e) : get_contin(e));
	}
      }
    }
    //access mesthods
    const std::vector<variant<bool, contin_t> >& get_vt() const {
      return _vt;
    }

    //equality operator
    bool operator==(const mixed_table& mt) const {
      std::vector<variant<bool,contin_t> >::const_iterator il = _vt.begin();
      for(std::vector<variant<bool,contin_t> >::const_iterator
	  ir = mt._vt.begin(); ir != mt._vt.end(); ++il, ++ir) {
	if(boost::get<bool>(&(*il))) {
	  if(boost::get<bool>(*il)!=boost::get<bool>(*ir))
	    return false;
	}
	else if(!util::isEqual(boost::get<contin_t>(*il),
			       boost::get<contin_t>(*ir)))
	  return false;
      }
      return true;
    }
    bool operator!=(const mixed_table& mt) const {
      return !operator==(mt);
    }
  };



  //WARNING : should be in eval_action.h
  //but could not do that due to dependency issues
  //-------------------------------------------------------------------------
  // mixed_action table
  //-------------------------------------------------------------------------

  //this table permits to represent inputs and outputs of combo trees
  //that have boolean, continuous, action_result arguments and/or output
  
  class mixed_action_table {
    typedef tree_type::iterator type_pre_it;
    typedef tree_type::sibling_iterator type_sib_it;
    //Vector of bool or contin, depending on the output type of vtree
    //size = 2^|boolean+action_result inputs| * _rnt.size()
    //each slice of rnt.size() corresponds to a particular
    //boolean+action_result input, all possible boolean+action_result inputs
    //are enumerated in lexico order as for truth_table
    //each element in a slice of rnt.size() corresponds to the result of
    //a particular contin input, all obtained from rnt
    std::vector<variant<bool,contin_t> > _vt;
    //NOTE : can be a bit optimized by using
    //variant<std::vector<bool>,std::vector<contin_t> > _vt;
    //instead

    int _bool_arg_count; //number of boolean arguments
    int _contin_arg_count; //number of contin arguments
    int _action_arg_count; //number of action_result arguments

    util::hash_map<int, int> _arg_map; //link the argument index with the
                                       //boolean, action_result or contin
                                       //index, that is
                                       //for instance if the inputs are
                                       //(bool, contin, contin, bool, action)
                                       //the the map is
                                       //(<0,0>,<1,0>,<2,1>,<3,1>,<4,0>)
    std::vector<int> _bool_arg; //link the ith bool arg with arg index
    std::vector<int> _contin_arg; //link the ith contin arg with arg index
    std::vector<int> _action_arg; //link the ith action arg with arg index
    tree_type _prototype;
    node_type _output_type;

    RndNumTable _rnt;

    //take a prototype, set _prototype, _contin_arg_count, _bool_arg_count,
    //_action_arg_count, _arg_map, _bool_arg, _contin_arg, action_arg
    //and _output_type
    void set_prototype(const tree_type& prototype) {
      _contin_arg_count = 0;
      _bool_arg_count = 0;
      _action_arg_count = 0;
      _prototype = prototype;
      type_pre_it proto_it = prototype.begin();
      int arg_idx = 0;
      for(type_sib_it i=proto_it.begin();i!=proto_it.end();++i,++arg_idx) {
	if(type_pre_it(i)==_prototype.last_child(proto_it)) {
	  _output_type = *i;
	}
	else {
	  if(*i==id::boolean) {
	    std::pair<int, int> p(arg_idx, _bool_arg_count);
	    _arg_map.insert(p);
            _bool_arg.push_back(arg_idx);
	    _bool_arg_count++;
	  } 
	  else if(*i==id::contin) {
	    std::pair<int, int> p(arg_idx, _contin_arg_count);
	    _arg_map.insert(p);
            _contin_arg.push_back(arg_idx);
	    _contin_arg_count++;
	  } 
	  else if(*i==id::action_result) {
	    std::pair<int, int> p(arg_idx, _action_arg_count);
	    _arg_map.insert(p);
            _action_arg.push_back(arg_idx);
	    _action_arg_count++;
	  }
	}
      }
    }

  public:
    //constructors
    mixed_action_table() {}
    mixed_action_table(const vtree& tr, const RndNumTable& rnt,
		const tree_type& prototype) {
      _rnt = rnt;
      if(prototype.empty()) {
	tree_type inferred_proto = infer_tree_type_fast(tr);
        set_prototype(inferred_proto);
      }
      else set_prototype(prototype);
      //populate _vt
      //WARNING : works only for _bool_arg_count + _action_arg_count <= 64
      int bool_action_arg_count = (_bool_arg_count + _action_arg_count);
      unsigned long int bool_action_table_size = 1 << bool_action_arg_count;
      for(unsigned long int i=0; i < bool_action_table_size; ++i) {
	//bai stands for bool arg index
	for(int bai=0; bai < _bool_arg_count; ++bai)
	  binding(_bool_arg[bai]+1)=bool_to_vertex((i>>bai)%2);
	//aai stands for action arg index
	for(int aai=0; aai < _action_arg_count; ++aai) {
	  int arg_idx = _action_arg[aai]+1;
	  bool arg_val = (i>>(aai+_bool_arg_count))%2;
	  binding(arg_idx) = (arg_val?id::action_success : id::action_failure);
	}
	//contin populate
	for(const_cm_it si=rnt.matrix().begin();si!=rnt.matrix().end();++si) {
	  int cai = 0; //contin arg index
	  for(const_cv_it j = (*si).begin(); j != (*si).end(); ++j, ++cai)
	    binding(_contin_arg[cai]+1)=*j;
	  vertex e = eval_throws(tr);
	  if(is_boolean(e))
	    _vt.push_back(vertex_to_bool(e));
	  else if(is_action_result(e))
	    _vt.push_back(e==id::action_success? true : false); 
	  else if(is_contin(e))
	    _vt.push_back(get_contin(e));
	  //should never get to this part
	  else assert(false);
	}
      }
    }
    //access mesthods
    const std::vector<variant<bool, contin_t> >& get_vt() const {
      return _vt;
    }

    //equality operator
    bool operator==(const mixed_action_table& mat) const {
      std::vector<variant<bool,contin_t> >::const_iterator il = _vt.begin();
      for(std::vector<variant<bool,contin_t> >::const_iterator
	  ir = mat._vt.begin(); ir != mat._vt.end(); ++il, ++ir) {
	if(boost::get<bool>(&(*il))) {
	  if(boost::get<bool>(*il)!=boost::get<bool>(*ir))
	    return false;
	}
	else if(!util::isEqual(boost::get<contin_t>(*il),
			       boost::get<contin_t>(*ir)))
	  return false;
      }
      return true;
    }
    bool operator!=(const mixed_action_table& mat) const {
      return !operator==(mat);
    }
  };  


} //~namespace combo

inline std::ostream& operator<<(std::ostream& out,
				const combo::truth_table& tt) {
  for (std::vector<bool>::const_iterator it=tt.begin();it!=tt.end();++it)
    out << *it << " ";
  return out;
}

inline std::ostream& operator<<(std::ostream& out,
				const combo::contin_matrix& cm) {
  for(combo::const_cm_it i = cm.begin(); i != cm.end(); ++i) {
    for(combo::const_cv_it j = (*i).begin(); j != (*i).end(); ++j) {
      out << *j << '\t';
    }
    out << std::endl;
  }
  return out;
}

inline std::ostream& operator<<(std::ostream& out,
				const combo::RndNumTable& rnt) {
  out << rnt.matrix();
  return out;
}

inline std::ostream& operator<<(std::ostream& out,
				const combo::contin_table& ct) {
  for (combo::const_cv_it it=ct.begin(); it != ct.end(); ++it)
    out << *it << " ";
  return out;
}

inline std::ostream& operator<<(std::ostream& out,
				const combo::mixed_table& mt) {
  const std::vector<boost::variant<bool,combo::contin_t> >& vt = mt.get_vt();
  for(unsigned i = 0; i != vt.size(); ++i)
    out << (boost::get<combo::contin_t>(&vt[i])?
	    boost::get<combo::contin_t>(vt[i]):
	    boost::get<bool>(vt[i])) << " ";
  return out;
}

inline std::ostream& operator<<(std::ostream& out,
				const combo::mixed_action_table& mat) {
  const std::vector<boost::variant<bool,combo::contin_t> >& vt = mat.get_vt();
  for(unsigned i = 0; i != vt.size(); ++i)
    out << (boost::get<combo::contin_t>(&vt[i])?
	    boost::get<combo::contin_t>(vt[i]):
	    boost::get<bool>(vt[i])) << " ";
  return out;
}

namespace boost {
  inline size_t hash_value(const combo::truth_table& tt) {
    return hash_range(tt.begin(),tt.end());
  }
} //~namespace boost

#endif
