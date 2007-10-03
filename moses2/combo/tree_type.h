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

#ifndef _TREE_TYPE_H
#define _TREE_TYPE_H

#include "combo/vertex.h"
#include "util/tree.h"
#include "util/numeric.h"
#include "util/hash_map.h"
#include "util/exception.h"
#include "combo/using.h"

namespace combo {

  namespace id {
    enum node_type {
      application,
      //elementary types
      boolean,
      contin,
      action_result,
      //unknown type
      unknown
    };
  }
  typedef id::node_type node_type;

  //structure that codes the type of a tree
  typedef util::tree<node_type> tree_type;

  //the output type is the last child of the root
  inline id::node_type output_type(const tree_type& t) { 
    assert(!t.begin().is_childless());
    return *t.begin().last_child();
  }

  bool is_boolean_output(const vertex& v);

  bool is_contin_output(const vertex& v);

  //WARNING : should be action.h but could not do that due to dependency issues
  //Warning : retruns false for action_if as it can be any output type
  bool is_action_result_output(const vertex& v);

  bool is_boolean_input(const vertex& v);

  //Warning : retruns false for contin_if as it has both
  //contin and boolean types as inputs
  //if v is an argument it return false too
  bool is_contin_input(const vertex& v);

  //WARNING : should be action.h but could not do that due to dependency issues
  //Warning : retruns false for action_if as it can be any input type
  bool is_action_result_input(const vertex& v);

  node_type vertex_output_type(const vertex& v);

  //Warning : if vertex v is id::contin_if returns id::unknown
  //if v is an argument it return id::unknown too
  node_type vertex_input_type(const vertex& v);

  //like above but include contin_if into account
  //Warning : it is assumed that input_vertex is in its permited range
  //if not the result shoud be ignored as it has no meaning
  node_type vertex_input_type(const vertex& v, int input_index);

  template<typename Tree>
  bool check_tree_type(const Tree& tr, const tree_type& ty) {
    assert(tr.is_valid(tr.begin()) && ty.is_valid(ty.begin()));
    return check_tree_type(tr, tr.begin(), ty, ty.begin());
  }

  template<typename Tree>
  bool check_tree_type(const Tree& tr, const typename Tree::iterator& tr_it,
		       const tree_type& ty, const tree_type::iterator& ty_it) {
    typedef typename Tree::iterator pre_it;
    typedef typename Tree::sibling_iterator sib_it;
    //if application
    if(*ty_it==id::application) {
      node_type output = vertex_output_type(*tr_it);
      if(is_argument(*tr_it)) {
	argument arg = get_argument(*tr_it);
	if(arg.is_negated())
	  arg.negate();
	if(output==id::unknown)
	  return true;
        else return output==*ty.child(ty_it, arg.idx);
      }
      else {
	if(*ty.last_child(ty_it)==output) {
	  if(tr_it.is_childless())
	    return true;
	  else {
	    for(sib_it sib = tr_it.begin(); sib != tr_it.end(); ++sib) {
	      tree_type new_ty = ty;
	      tree_type::iterator new_ty_it = new_ty.begin();
	      *new_ty.last_child(new_ty_it) = 
		vertex_input_type(*tr_it, tr.sibling_index(sib));
	      if(!check_tree_type(tr, pre_it(sib), new_ty, new_ty_it))
		return false;
	    }
	    return true;
	  }
	}
	else return false;
      }
    }
    //if constant
    else if(*ty_it==id::boolean)
      return is_boolean(*tr_it);
    else if(*ty_it==id::contin)
      return is_contin(*tr_it);
    else if(*ty_it==id::action_result)
      return is_action_result(*tr_it);
    else {//in this case ty_it==id::unknown
      return true;
    }
  }

  //infer the type of a tree
  //the tree is checked entirely while inferring
  //if the checking goes wrong throw a TypeCheckException
  template<typename Tree>
  tree_type infer_tree_type(const Tree& tr) throw(TypeCheckException) {
    assert(tr.is_valid(tr.begin()));
    //return infer_tree_type(tr, tr.begin());

    //to get things simpler the type is infered using infer_tree_type_fast
    //and then checked using check_tree_type. It might be that coding the
    //inference-checker is more interesting by its complexity but it is
    //not sure
    tree_type res;
    try {
      res = infer_tree_type_fast(tr);
    } catch(const TypeCheckException& tce) {
      throw tce;
    }
    if(!check_tree_type(tr, res)) {
      throw TypeCheckException();
    }
    return res;
  }

  //TODO
  template<typename Tree>
  tree_type infer_tree_type(const Tree& tr,
			    const typename Tree::iterator& tr_it)
    throw(TypeCheckException)
  {
    tree_type res;
    if(is_argument(*tr_it)) {
      //TODO
    }
    return res;
  }

  //like infer_tree_type, but faster because do not check that
  //the tree is entirely well typed, look only at the leaves and the root.
  //If bad typed the evaluation is not guaranteed to run correctly
  //(might send EvalException while running).
  template<typename Tree>
  tree_type infer_tree_type_fast(const Tree& tr) throw(TypeCheckException) {
    typedef typename Tree::leaf_iterator leaf_it;
    typedef typename Tree::iterator pre_it;
    typedef typename Tree::sibling_iterator sib_it;
    typedef typename tree_type::iterator type_pre_it;
    tree_type res;
    util::hash_map<int, node_type> inputs_t;
    node_type output_t = id::unknown;
    pre_it tr_it = tr.begin();
    //argument
    if(is_argument(*tr_it)) {
      argument arg = get_argument(*tr_it);
      if(arg.is_negated()) {
	arg.negate();
	//it is assumed that if negated the argument is necessarily bool
	output_t = id::boolean;
      }
      std::pair<int, node_type> p(arg.idx, output_t);
      inputs_t.insert(p);
    }
    //constant
    else if(is_boolean(*tr_it)) {
      res.set_head(id::boolean);
      return res;
    }
    else if(is_contin(*tr_it)) {
      res.set_head(id::contin);
      return res;
    }
    else if(is_action_result(*tr_it)) {
      res.set_head(id::action_result);
      return res;
    }
    //function
    else {
      assert(!tr_it.is_childless());
      output_t = vertex_output_type(*tr_it);
      for(leaf_it i = tr.begin_leaf(); i != tr.end_leaf(); ++i) {
	pre_it ip = tr.parent(i);
	if(tr.is_valid(ip)) {
	  assert(!is_constant(*ip));
	  node_type at = id::unknown; //argument type
	  at = vertex_input_type(*ip, tr.sibling_index(sib_it(i)));
	  node_type i_type = vertex_output_type(*i); //type of leaf i
	  if(i_type==id::unknown)
	    i_type=at;
	  else if(i_type!=at) throw TypeCheckException();
	  if(is_argument(*i)) {
	    argument arg = get_argument(*i);
	    if(arg.is_negated()) {
	      arg.negate();
	      //it is assumed that if negated the argument is necessarily bool
	      if(i_type!=id::boolean) throw TypeCheckException(arg.idx);
	    }
	    util::hash_map<int, node_type>::iterator arg_it;
	    arg_it = inputs_t.find(arg.idx);
	    if(arg_it==inputs_t.end()) {//argument not found
	      std::pair<int, node_type> p(arg.idx, i_type);
	      inputs_t.insert(p);
	    }
	    else if(i_type != arg_it->second)
	      throw TypeCheckException(arg.idx);
	  }
	}
      }
    }
    //Build res wrt inputs_t and output_t
    type_pre_it res_it = res.set_head(id::application);
    for(util::hash_map<int, node_type>::iterator arg_it = inputs_t.begin();
	arg_it != inputs_t.end(); ++arg_it) {
      int diff = arg_it->first - res.number_of_children(res_it) - 1;
      if(0 < diff)
	res.append_children(res_it, id::unknown, diff);
      res.append_child(res_it, arg_it->second);
    }
    res.append_child(res_it, output_t);
    return res;
  }

  //return the number of arguments of type contin
  int contin_arity(const tree_type& ty);

  //return the number of arguments of type boolean
  int boolean_arity(const tree_type& ty);

  //WARNING : should be action.h but could not do that due to dependency issues
  //return the number of arguments of type action_result
  int action_result_arity(const tree_type& ty);

} //~namespace combo

std::ostream& operator<<(std::ostream&, const combo::node_type&);
std::istream& operator>>(std::istream&, combo::node_type&);

#endif
