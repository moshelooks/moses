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

#ifndef _UTIL_BASED_VARIANT_H
#define _UTIL_BASED_VARIANT_H

#include <boost/variant.hpp>

namespace util {
  namespace detail {
    template<typename Base>
    struct based_variant_visitor : public boost::static_visitor<Base*> {
      template<typename T>
      Base* operator()(T& t) const { return &t; }
    };
    template<typename Base>    
    struct const_based_variant_visitor : public boost::static_visitor<const Base*> {
      template<typename T>
      const Base* operator()(const T& t) const { return &t; }
    };
  } //~namespace detail

  template<typename Variant,typename Base>
  struct based_variant : public Variant {
    template<typename T>
    based_variant(const T& v) : Variant(v) { }
    based_variant() { }

    Base* operator->() {
      return boost::apply_visitor(detail::based_variant_visitor<Base>(),*this); 
    }
    const Base* operator->() const {
      return boost::apply_visitor(detail::const_based_variant_visitor<Base>(),*this);
    }

    /*operator Base&() { 
      return *boost::apply_visitor(detail::based_variant_visitor<Base>(),*this); 
    }
    operator const Base&() const { 
      return *boost::apply_visitor(detail::const_based_variant_visitor<Base>(),
      *this);
      }*/
  };

} //~namespace util

#endif
