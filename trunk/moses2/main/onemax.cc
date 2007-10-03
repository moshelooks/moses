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

int main(int argc,char** argv) { 
  optargs args(argc,argv);
  cout_log_best_and_gen logger;
  field_set fs(field_set::disc_spec(2),args.length); //all boolean

  instance_set<int> population(args.popsize,fs);
  foreach(instance& inst,population)
    generate(fs.begin_bits(inst),fs.end_bits(inst),randbool);

  optimize(population,args.n_select,args.n_generate,args.max_gens,
	   one_max(),terminate_if_gte<int>(args.length),tournament_selection(2),
	   univariate(),local_structure_probs_learning(),
	   replace_the_worst(),logger);
}
