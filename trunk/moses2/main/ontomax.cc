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

#include "main/edaopt.h"
#include "eda/initialization.h"

void recbuild(onto_tree& tr,onto_tree::iterator it,
	      int b,int maxd,int d,int s) {
  *it=lexical_cast<string>(d)+lexical_cast<string>(s);
  if (d<maxd) {
    tr.append_children(it,b);
    int child_s=0;
    for (onto_tree::sibling_iterator sib=it.begin();sib!=it.end();++sib)
      recbuild(tr,sib,b,maxd,d+1,s*b+child_s++);
  }
}

int main(int argc,char** argv) { 
  assert(argc==7);
  optargs args(argc,argv," depth branching");
  int depth=lexical_cast<int>(argv[5]);
  int branching=lexical_cast<int>(argv[6]);
  cout_log_best_and_gen logger;

  
  onto_tree tr("");
  recbuild(tr,tr.begin(),branching,depth,0,0);
  field_set fs(field_set::onto_spec(tr),args.length);
  instance_set<contin_t> population(args.popsize,fs);
  foreach(instance& inst,population) {
    occam_randomize_onto(fs,inst);
  }

  optimize(population,args.n_select,args.n_generate,args.max_gens,
	   ontomax(fs),
	   terminate_if_gte<contin_t>((depth+pow(float(branching),
						 depth)-1)*args.length),
	   tournament_selection(2),
	   univariate(),local_structure_probs_learning(),
	   replace_the_worst(),logger);
}
