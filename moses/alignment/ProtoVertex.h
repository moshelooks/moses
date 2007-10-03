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

#ifndef PROTO_VERTEX_H
#define PROTO_VERTEX_H

#include "Vertex.h"
#include "set_util.h"
#include <boost/bind.hpp>

#include <iostream>
using namespace std;

namespace alignment {
  using namespace std;
  using namespace boost;

  struct ProtoVertex;
  typedef tree<ProtoVertex> ptree;

  inline size_t hash_value(ptree::pre_order_iterator it) { 
    return (size_t)it.node; }
  inline size_t hash_value(ptree::sibling_iterator it) { 
    return (size_t)it.node; }

  typedef pair<int,Vertex> vloc; //located vertex (in tree with first's idx)
  typedef list<vloc> VlocSeq;

  //for prototype trees
  struct ProtoVertex {
    ProtoVertex() { }
    explicit ProtoVertex(Handle h) { v.insert(h); }
    explicit ProtoVertex(const Vertex& vtx) { v.insert(vtx); }
    explicit ProtoVertex(const Vertex& vtx,const list<int>& l) { 
      v.insert(vtx); 
      transform(l.begin(),l.end(),back_inserter(vlocs),
		bind(make_pair<int,Vertex>,_1,vtx));
    }
    //typedef __gnu_cxx::hash_set<ptree::pre_order_iterator,
    //boost::hash<ptree::pre_order_iterator> > JSet;
    VertexSet v;
    //JSet jumps;
    VlocSeq vlocs;
    
    bool operator==(const ProtoVertex& rhs) const { 
      return (v==rhs.v);// && jumps==rhs.jumps);
    }
    bool operator!=(const ProtoVertex& rhs) const { 
      return !(operator==(rhs));
    }
    
    Handle front() const { 
      const Handle* h=boost::get<Handle>(&*v.begin());
      return (h ? *h : (Handle)0);
    }
  };
  
  inline size_t hash_value(const ProtoVertex& pv) { 
    return (size_t)pv.front(); }

  void reduce_vlocs(ptree&);

  class vtree2ptree {
  public:
    vtree2ptree(const vtree& src,ptree& dst) : indices(emptylist),tr(dst) {
      tr.set_head(ProtoVertex());
      rec_make(src.begin(),dst.begin());
    }

    vtree2ptree(const pair<vtree,list<int> >& src,ptree& dst)
      : indices(src.second),tr(dst) {
      tr.set_head(ProtoVertex());
      rec_make(src.first.begin(),dst.begin());
    }
  private:
    const list<int>& indices;
    ptree& tr;

    static const list<int> emptylist;

    void rec_make(vtree::sibling_iterator src,ptree::sibling_iterator dst);
  };

} //~namespace alignment
  
inline std::ostream& operator<<(std::ostream& out,
				const alignment::ProtoVertex& pv) {
  using namespace alignment;
  if (pv.v.size()==1)
    return out << *pv.v.begin();
  out << "{ ";
  for (VertexSet::const_iterator it=pv.v.begin();it!=pv.v.end();++it)
    out << *it << " ";
  out << "}";
  return out;
}

inline std::istream& operator>>(std::istream& in,alignment::ProtoVertex& pv) {
  Vertex v;
  in >> v;
  pv=alignment::ProtoVertex(v);
  return in;
}

namespace id {
  using alignment::ProtoVertex;

  inline bool is_progn(const ProtoVertex& pv) { 
    return is_progn(pv.front()); }
  inline bool is_if_food(const ProtoVertex& pv) { 
    return is_if_food(pv.front()); }
  inline bool is_move(const ProtoVertex& pv) { 
    return is_move(pv.front()); }
  inline bool is_right(const ProtoVertex& pv) { 
    return is_right(pv.front()); }
  inline bool is_left(const ProtoVertex& pv) { 
    return is_left(pv.front()); }
  inline bool is_reversal(const ProtoVertex& pv) { 
    return is_reversal(pv.front()); }
  inline bool is_turn(const ProtoVertex& pv) { 
    return is_turn(pv.front()); }

  inline bool is_and(const ProtoVertex& pv) { 
    return is_and(pv.front()); }
  inline bool is_or(const ProtoVertex& pv) { 
    return is_or(pv.front()); }
  inline bool is_not(const ProtoVertex& pv) { 
    return is_not(pv.front()); }

  inline bool is_near(const ProtoVertex& pv) { 
    return is_near(pv.front()); }
  inline bool is_identity(const ProtoVertex& pv) { 
    return is_identity(pv.front()); }
  /*
  inline bool is_walktowards(const ProtoVertex& pv) { 
    return is_walktowards(pv.front()); }
  inline bool is_pickup(const ProtoVertex& pv) { 
    return is_pickup(pv.front()); }
  inline bool is_tea(const ProtoVertex& pv) { 
    return is_pickup(pv.front()); }
  inline bool is_pickup(const ProtoVertex& pv) { 
    return is_pickup(pv.front()); }
  inline bool is_pickup(const ProtoVertex& pv) { 
    return is_pickup(pv.front()); }
  */


  inline bool is_variable(const ProtoVertex& pv) {
    return (pv.front()==0); }

  template<> inline ProtoVertex create_progn<ProtoVertex>() { 
    return ProtoVertex(create_progn<Vertex>()); }
  template<> inline ProtoVertex create_if_food<ProtoVertex>() { 
    return ProtoVertex(create_if_food<Vertex>()); }
  template<> inline ProtoVertex create_move<ProtoVertex>() { 
    return ProtoVertex(create_move<Vertex>()); }
  template<> inline ProtoVertex create_right<ProtoVertex>() { 
    return ProtoVertex(create_right<Vertex>()); }
  template<> inline ProtoVertex create_left<ProtoVertex>() { 
    return ProtoVertex(create_left<Vertex>()); }
  template<> inline ProtoVertex create_reversal<ProtoVertex>() { 
    return ProtoVertex(create_reversal<Vertex>()); }

  inline bool matches(const ProtoVertex& pv1,const ProtoVertex& pv2) {
    return (pv1.front()==0 ? 
	    (pv2.front()==0 && boost::get<Argument>(*pv1.v.begin())==
   	                       boost::get<Argument>(*pv2.v.begin())) :
	    (!has_empty_intersection(pv1.v.begin(),pv1.v.end(),
				     pv2.v.begin(),pv2.v.end()) ||
	     (is_turn(pv1.front()) && is_turn(pv2.front()))));
  }
  inline bool go_down(const ProtoVertex& pv) { return false; }
  inline bool is_commutative(const ProtoVertex& pv) { 
    return (is_and(pv) || is_or(pv));
  }
  inline bool is_associative(const ProtoVertex& pv) { 
    return (is_progn(pv) || is_and(pv) || is_or(pv));
  }
} //~namepsace id

#endif
