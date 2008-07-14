#include <fstream>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include <boost/foreach.hpp>

#include "io_util.h"
#include "bool_util.h"

#include "BDe.h"
#include "scoring.h"
#include "dtree_learn.h"

using namespace boost;
using namespace std;
using namespace modeling;

typedef vector<const Assignment*> ptr_seq;

struct hmmdf {
  hmmdf(int b) : blocksize(b),r1(1),r2(2) {}
  int blocksize;
  double r1;
  double r2;

  double operator()(const Assignment& a) const {
    std::vector<int> level(blocksize);
    double res=0;
    Assignment::const_iterator i=a.begin();
    for (int j=0;j<blocksize;++j) {
      level[j]=u(i);
      res+=bdf(level[j]);
      level[j]=t(level[j]);
      i+=blocksize;
    }
    res=r1*res+r2*wr(level.begin());
    return res;
  }
  int u(Assignment::const_iterator i) const {
    int res=0;
    for (int j=0;j<blocksize;++j,++i)
      res+=tobool(*i);
    return res;
  }
  double bdf(int u) const {
    return (u==0 || u==blocksize) ? 1.0 : 
        0.9*(1.0-
              (fabs(double(blocksize)/2.0 - double(u))/
               (double(blocksize)/2.0 - 1.0)));
  }
  int t(int u) const {
    return u==blocksize ? 1 : (u==0 ? 0 : -1);
  }
  double wr(std::vector<int>::const_iterator i) const {
    int u=0;
    for (int j=0;j<blocksize;++j,++i) {
      if (*i==-1)
        return 0;
      u+=*i;
    }
    return bdf(u);
  }
};

typedef pair<double,Assignment> fpair;
    
const Assignment* t_selection(const vector<fpair>& v) {
  return &(tournament_select(v.begin(),v.end(),2,less<fpair>())->second);
}

vector<fpair>::iterator rtr_select(const Assignment& a,double score,
                                   vector<fpair>& pop,int tsize) {
  assert(tsize>0);
  vector<fpair>::iterator closest;
  if (tsize==1) { //simply do size-2 tournament selection
    closest=pop.begin()+randint(pop.size());
  } else {
    Selector select(pop.size());

    int closestSim=NEG_INFINITY;
    for (int i=0;i<tsize;++i) {
      vector<fpair>::iterator tmp=pop.begin()+select();
      int sim=(-hamming_distance(a.begin(),a.end(),tmp->second.begin()));
      if (sim>closestSim) {
        sim=closestSim;
        closest=tmp;
      }
    }
  }
  return (closest->first<score) ? closest : pop.end();
}

template<typename Opt>
void hBOA(Opt score,int length,int pop_size,int n_gen,double maxscore) {
  vector<fpair> pop(pop_size,fpair(0.0,Assignment(length,false)));
  BOOST_FOREACH(fpair& p,pop) {
    generate(p.second.begin(),p.second.end(),&randbool);
    p.first=score(p.second);
  }
  
  int n_evals(pop_size);

  cout << "genration # " << 0
       << ", # evals " << n_evals
       << ", best fitness " << max_element(pop.begin(),pop.end())->first 
       << endl;

  for (int gen=0;gen<n_gen;++gen,n_evals+=pop_size) {
    //tournament-select instances to model
    ptr_seq to_model(pop_size);
    generate(to_model.begin(),to_model.end(),bind(t_selection,ref(pop)));
    
    //build an empty model
    Graph g(pop_size);
    g.reserve(length);
    for (int i=0;i<length;++i) {
      BoolMarginal& m=g.create_bool_marginal();
      assert(m.size()==pop_size);
      BoolMarginal::iterator j=m.begin();
      BOOST_FOREACH(const Assignment* a,to_model) {
        *j=tobool((*a)[i]);
        ++j;
      }
      g.push_back(BoolDTree(m.begin(),m.end()));
    }

    //update the gain offset
    BDe::gainOffset=(log2(gain_t(distance(to_model.begin(),
                                          to_model.end())))/2.0);

    //learn dependencies
    dtree_learn(g);
    
    //sample new instances and RTR them into the population
    assert(g.ordering().size()==length);
    for (int i=0;i<pop_size;++i) {
      Assignment a(length,false);
      generate_assignment(g.ordering().begin(),g.ordering().end(),g,a);
      int rtr_tsize=min(length,int(0.05*double(pop_size)+0.5));
      double s=score(a);
      vector<fpair>::iterator j=rtr_select(a,s,pop,rtr_tsize);
      if (j!=pop.end()) {
        j->first=s;
        j->second=a;
      }
    }

    double best=max_element(pop.begin(),pop.end())->first;
    cout << "genration # " << gen 
         << ", # evals " << n_evals
         << ", best fitness " << max_element(pop.begin(),pop.end())->first 
         << endl;
    if (best>=maxscore)
      break;
  }
  cout << "best solution: " 
       << max_element(pop.begin(),pop.end())->second << endl;

}

#if 0
    //use the model to sample new instances and integrate them into the pop
    n_evals+=pop.run_generation
        (int(pop.configuration().replacement_ratio()*pop.size()),
         bind(generate_assignment<vector<int>::const_iterator>,
              g.ordering().begin(),g.ordering().end(),ref(g),_1),
         int(pop.size()*tMin+0.5));

      //if we have improved the fitness max, resample
      best=max(best,max_element(pop.trees().begin(),pop.trees().end(),
                                pop.fitness_comp())->second.fitness);

      if (best>=cutoff) {
        for (tpmap::const_iterator it=pop.trees().begin();
             it!=pop.trees().end();++it)
          if (it->second.fitness>=cutoff) {
            vtree tmp(it->first);
            pop.ni().simplify()(tmp);;
            cout << "opt: " << tmp << ": " 
                 << mycomplexity(tmp.begin()) << endl;
          }
        break;
      }

      int minmag=POS_INFINITY;
      for (tpmap::const_iterator it=pop.trees().begin();
           it!=pop.trees().end();++it)
        if (it->second.fitness>=best) {
          for (ass_set::const_iterator ass=it->second.asses.begin();
               ass!=it->second.asses.end();++ass) {
            int mag=magnitude(*ass);
            if (mag<minmag)
              minmag=mag;
          }
        }
      cout << "minmag " << minmag << " ";

      /**/
      /**cout << "     " << gen
           << " sampled " << n_evals
           << " best " << best << endl;**/
    }
    return make_pair(n_evals,best);
}
#endif

int main(int argc,char** argv) {
  assert(argc==5);
  srand(lexical_cast<int>(argv[1]));
  int blocksize=lexical_cast<int>(argv[2]);
  int length=blocksize*blocksize;
  int pop_size=lexical_cast<int>(argv[3]);
  int n_gen=lexical_cast<int>(argv[4]);

  cout << "hBOA, HMMDF ~"
       << " block size: " << blocksize
       << " population size: " << pop_size
       << " n generations: " << n_gen << endl;
  
  hBOA(hmmdf(blocksize),length,pop_size,n_gen,blocksize+2.0);
}

