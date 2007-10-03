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

namespace rewrite {

  //ReduceLess
  template<typename T>
  void ReduceLess<T>::visit(tree<T>& t,sib_it it) const {
    if (!is_less(*it))
      return;

    if (is_true(*t.begin(it)) || is_false(*t.child(it,1))) {
      t.erase_children(it);
      *it=create_false();
    } else if (is_false(*t.begin(it)) && is_true(*t.child(it,1))) {
      t.erase_children(it);
      *it=create_true();
    } else if ((is_constant(*t.begin(it)) || is_false(*t.begin(it))) &&
	       (is_constant(*t.child(it,1)) || is_true(*t.child(it,1)))) {
      *it=(get_const(*t.begin(it))<get_const(*t.child(it,1))) ?
	create_true() : create_false();
      t.erase_children(it);
    } else if (is_not(*t.begin(it))) {
      if (is_not(*t.child(it,1))) {
	t.reorder_siblings(t.begin(it),t.child(it,1));
	t.erase(t.flatten(t.begin(it)));
	t.erase(t.flatten(t.child(it,1)));
      } else if (is_constant(*t.child(it,1)) ||
		 !SubtreeOrder<T>()(t.begin(t.begin(it)),t.child(it,1))) {
	t.erase(t.flatten(t.begin(it)));
	t.reorder_siblings(t.begin(it),t.child(it,1));
	t.insert_above(t.begin(it),create_not());
      }
    } else if (is_not(*t.child(it,1)) &&
	       (is_constant(*t.begin(it)) || 
		!SubtreeOrder<T>()(t.begin(it),t.begin(t.child(it,1))))) {
      t.insert_above(t.begin(it),create_not());
      t.reorder_siblings(t.begin(it),t.child(it,1));
      t.erase(t.flatten(t.begin(it)));
    }
  }

  //ReduceAlmost
  template<typename T>
  void ReduceAlmost<T>::visit(tree<T>& t,sib_it it) const {
    if (is_constant(*it)) {
      if (get_const(*it)==1.0f)
	*it=create_true();
      else if (get_const(*it)==0.0f)
	*it=create_false();
    }
  }

} //~namespace rewrite
