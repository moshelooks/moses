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

int main(int argc,char** argv) { 
  assert(argc==6);
  optargs args(argc,argv);
  int depth=lexical_cast<int>(argv[5]);
  cout_log_best_and_gen logger;

  /*field_set fs(field_set::spec(field_set::contin_spec(2.0,2.5,0.5,depth),
    args.length));*/
  field_set fs(field_set::contin_spec(0.0,0.5,0.5,depth),args.length);
  instance_set<contin_t> population(args.popsize,fs);
  foreach(instance& inst,population) {
    occam_randomize_contin(fs,inst);
    cout << fs.stream(inst) << endl;
    cout << fs.stream_raw(inst) << endl;
  }

  contin_t epsilon=fs.contin().front().epsilon();
  optimize(population,args.n_select,args.n_generate,args.max_gens,
	   sphere(fs),
	   terminate_if_gte<contin_t>(-args.length*epsilon),
	   //terminate_if_gte(args.length*(7-2*epsilon)*(7-2*epsilon)),
	   tournament_selection(2),
	   univariate(),local_structure_probs_learning(),
	   replace_the_worst(),logger);
}
