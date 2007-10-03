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

#ifndef MARG_VERTEX_H
#define MARG_VERTEX_H

#include "ProtoVertex.h"

typedef pair<ptree::pre_order_iterator,int> mref;
typedef vector<mref> marg;
//typdef pair<ptree::pre_order_iterator,marg>
typedef tree<MargVertex> mtree;

template<typename It>
void make_margs(It from,It to,ptree& t) {
  for (It it=from;it!=to;++it)
