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

#ifndef MOSES_H
#define MOSES_H

#include "lmodeling.h"
#include "dtree_learn.h"
#include "hash_util.h"
#include "func_util.h"
#include "bool_util.h"
//#include "udraw.h"
#include <boost/tuple/tuple.hpp>
#include "BDe.h"

namespace moses {
  using namespace local;

  typedef vector<const Assignment*> ptr_seq;
  typedef hash_set<Assignment,boost::hash<Assignment> > ass_set;

  int mycomplexity(const vtree::iterator it);

  int magnitude(const Assignment& ass);

  struct fitness_size_comp {
    bool operator()(const pair<vtree,fitness_t>& y, 
                    const pair<vtree,fitness_t>& x) const {
      return (x.second<y.second ? true : 
              (x.second>y.second ? false :
               (mycomplexity(x.first.begin())>
                mycomplexity(y.first.begin()))));
    }
  };

  typedef set<pair<vtree,fitness_t>,fitness_size_comp> candidate_map;


  struct point {
    point() : fitness(NEG_INFINITY) { }
    point(fitness_t f,const Assignment& a) : fitness(f) { asses.insert(a); }
    fitness_t fitness;
    ass_set asses;
  };
  typedef random_access_hash_map<vtree,point> tpmap;

  struct comp {
    typedef bool result_type;
    
    bool operator()(const point& p1,const point& p2) const {
      return (p1.fitness<p2.fitness);
    }
    bool operator()(const point& p1,fitness_t f) const {
      return (p1.fitness<f);
    }
    bool operator()(fitness_t f,const point& p2) const {
      return (f<p2.fitness);
    }
    bool operator()(const tpmap::value_type& u1,
                    const tpmap::value_type& u2) const {
      return (u1.second.fitness<u2.second.fitness);
    }
    bool operator()(const tpmap::value_type& u1,fitness_t f) const {
      return (u1.second.fitness<f);
    }
    bool operator()(fitness_t f,const tpmap::value_type& u2) const {
      return (f<u2.second.fitness);
    }
  };

  struct fitness_cache : public hash_map<vtree,fitness_t> {
  public:
    static fitness_cache& instance() { return _instance; }
    int max_size() const { return 10000; }
    pair<hash_map<vtree,fitness_t>::iterator,bool>
    insert(const hash_map<vtree,fitness_t>::value_type& x) {
      if (size()>=max_size())
        erase(begin());
      return hash_map<vtree,fitness_t>::insert(x);
    }
  private:
    static fitness_cache _instance;
    fitness_cache() { }
  };

  template<typename FF,typename TreeDist>
  struct config {
    config(const FF& f_,const TreeDist& d_,int window_size_)
      : f(f_),d(d_),window_size(window_size_) { }
    const FF& f;
    const TreeDist& d;
    const int window_size;

    int panic_factor() const { return 3; }
    float sample_p(int length) const { return 5.0f/(float)length; }
    //float sample_p(int length) const { return 0.5; }
    int n_to_model(int pop_size) const { return pop_size; }
    float replacement_ratio() const { return 0.5; }//1.0; }
    int t_size() const { return 2; }
    int n_pops() const { return 8; }
    float n_gens_ratio() const { return 1.0; }
  };
  
  template<typename Simplify,typename FF,typename TreeDist>
  struct population : public noncopyable {
    typedef neighborhood<Simplify> neighborhood;
    typedef config<FF,TreeDist> config;

    population(const neighborhood_params<Simplify>& np,const config& c,
               const vtree& vtr) : _ni(np),_c(c) {
      _ni.exemplar(vtr);
    }

    //clear and replace with a randomly generate pop of size n
    int reset(int n,semantic_sampler<Vertex,Simplify>& ss,float tMin=1.0f) {
      _pts.clear();

      cout << "making knobs for " << _ni.exemplar() << endl;

      _ni.make_knobs(ss);

      int pop_size=int(pow(ni().nbits(),1.05f)*n);
      cout << "pop size = " << pop_size << endl;

      _pts.reserve(pop_size);

      //update the gain offset
      //BDe::gainOffset=(log2(gain_t(pop_size))/2.0);

      /**return run_generation(pop_size,bind(random_assignment,ref(_ni.knobs()),
                                          _c.sample_p(_ni.length()),_1),
                                          int(pop_size*tMin+0.5));**/
      /**return run_generation(pop_size,bind(uniform_scaled_assignment,
                                          ref(_ni.knobs()),_1),
                                          int(pop_size*tMin+0.5));**/
      return run_generation(pop_size,assigner(_ni.knobs()),
			    int(pop_size*tMin+0.5));
    }

    const Assignment& t_select(int t_size) const {
      const ass_set& as=tournament_select(_pts.begin(),_pts.end(),t_size,
                                          fitness_comp())->second.asses;
      ass_set::const_iterator pos=as.begin();
      advance(pos,randint(as.size()));
      return *pos;
    }
    
    //returns # of fitness evaluations
    template<typename Sampler>
    int run_generation(int n,const Sampler& sample,int tMin) { 
      int n_evals=0;
      Assignment ass(_ni.length());
      vtree vtr;
      
      //tpmap toAdd;

      for (int i=_c.panic_factor()*n;i>=0 && n>0;--i) {
        //sample a new instance
        sample(ass);
        //cout << "sampled " << ass << endl;
        instantiate(ass,_ni.knobs(),_ni.exemplar(),vtr);
        //cout << "now " << vtr << endl;

        //_ni.simplify()(vtr);

        //cout << "cow " << vtr << endl;

        //get the fitness and check for duplication
	tpmap::iterator tppos=_pts.find(vtr);
        if (tppos!=_pts.end()) {
          tppos->second.asses.insert(ass);
          continue;
        }
	fitness_cache::iterator pos=fitness_cache::instance().find(vtr);
        if (pos==fitness_cache::instance().end()) {
          ++n_evals;
          pos=fitness_cache::instance().insert
            (make_pair(vtr,_c.f(vtr)
                       -0.001*float(mycomplexity(vtr.begin()))
                       )).first;
        }

        --n; //we have created an instance new to the pop, so decrement n
        if (_pts.size()<_pts.capacity()) { //if we are too small, add for sure
          _pts.insert(make_pair(vtr,point(pos->second,ass)));
        } else { //else run an rtr tournament to decide
          /**
          tpmap::iterator ins=toAdd.find(vtr);
          if (ins!=toAdd.end()) {
            ins->second.asses.insert(ass);
          } else {
            toAdd.insert(make_pair(vtr,point(pos->second,ass)));
          }
          **/
        
          /**/
          int tsize=min(int(_ni.nbits()+0.5),tMin);

          //tpmap::iterator ins=rtr_tournament(vtr,pos->second,tsize);
	  tpmap::iterator ins=rtr_hamming_tournament(ass,pos->second,tsize);

          if (ins!=_pts.end()) { //replace an existing point
            _pts.replace(ins,make_pair(vtr,point(pos->second,ass)));
          }
          /**/
        }
      }

      return n_evals;
    }

    const neighborhood& ni() const { return _ni; }
    int size() const { return _pts.size(); }
    const tpmap& trees() const { return _pts; }
    comp fitness_comp() const { return comp(); }
    const config& configuration() const { return _c; }

    void nondominating(candidate_map& cmap,const hash_set<vtree>& vis) const {
      for (tpmap::const_iterator it=_pts.begin();it!=_pts.end();++it) {
        vtree tmp(it->first);
        //cout << "nondom " << tmp << endl;
        _ni.simplify()(tmp);
        //cout << " -> " << tmp << endl;
        if (vis.find(tmp)==vis.end())
          cmap.insert(make_pair(tmp,ceil(it->second.fitness)));
      }

      for (candidate_map::iterator it=++cmap.begin();it!=cmap.end();) {
	candidate_map::const_iterator dom=cmap.begin();
        for (;dom!=it;) {
          pair<bool,bool> res=_c.f.dominates(dom->first,it->first);
          if ((res.first && !res.second) || 
              (!res.first && !res.second)) { //dom or equal dominates
            break;
          } else if (res.second && !res.first && dom->second<=it->second) {
	    candidate_map::iterator nxt=dom;
            ++nxt;
            cmap.erase(dom);
            dom=nxt;
            cout << "reva" << endl;
          } else { //neither dominates, nor equal
            ++dom;
          }
        }
        
        if (dom!=it) {
	  candidate_map::iterator nxt=it;
          ++nxt;
          cmap.erase(it);
          it=nxt;
        } else {
          ++it;
        }
      }
    }
  private:
    neighborhood _ni;
    const config& _c;
    tpmap _pts;

    tpmap::iterator rtr_tournament(const vtree& tr,fitness_t f,int tsize) {
      tpmap::iterator closest;
      assert(_pts.size()>=tsize);
      if (tsize==1) { //simply do size-2 tournament selection
        closest=_pts.begin()+randint(_pts.size());
      } else {
        Selector select(_pts.size());

        double closestSim=NEG_INFINITY;
        for (int i=0;i<tsize;++i) {
	  tpmap::iterator tmp=_pts.begin()+select();
          
          double sim=_c.d(tr,tmp->first);

          if (sim>closestSim) {
            sim=closestSim;
            closest=tmp;
          }
        }
      }
      //if (f>closest->second.fitness)
      //cout << "RTR " << closest->second.fitness << " -> " << f << endl;
      return (f>closest->second.fitness ? closest :
              ((f<closest->second.fitness || randbool()) ? _pts.end() : 
               closest));
    }

    tpmap::iterator rtr_hamming_tournament(const Assignment& ass,
                                           fitness_t f,int tsize) {
      tpmap::iterator closest;
      assert(_pts.size()>=tsize);
      if (tsize==1) { //simply do size-2 tournament selection
        closest=_pts.begin()+randint(_pts.size());
      } else {
        Selector select(_pts.size());

        int closestSim=NEG_INFINITY;
        for (int i=0;i<tsize;++i) {
	  tpmap::iterator tmp=_pts.begin()+select();
          
	  ass_set::const_iterator pos=tmp->second.asses.begin();
          advance(pos,randint(tmp->second.asses.size()));
          
          //double sim=_c.d(tr,tmp->first);
          int sim=(-hamming_distance(ass.begin(),ass.end(),pos->begin()));

          if (sim>closestSim) {
            sim=closestSim;
            closest=tmp;
          }
        }
      }
      //if (f>closest->second.fitness)
      //cout << "RTR " << closest->second.fitness << " -> " << f << endl;
      return (f>closest->second.fitness ? closest :
              ((f<closest->second.fitness || randbool()) ? _pts.end() : 
               closest));
    }


  };
  
  template<typename Simplify,typename FF,typename TreeDist>
  pair<int,fitness_t> boa(population<Simplify,FF,TreeDist>& pop,
                          int n_generations,float tMin,
                          fitness_t cutoff) {
    int n_evals(0);
    fitness_t best(NEG_INFINITY);
    for (int gen=0;gen<n_generations;++gen) {
      //tournament-select instances to model
      ptr_seq to_model(pop.configuration().n_to_model(pop.size()));
      generate(to_model.begin(),to_model.end(),
               bind(addressof<const Assignment>,
                    bind(&population<Simplify,FF,TreeDist>::t_select,ref(pop),
                         pop.configuration().t_size())));
      //build a model
      Graph g;
      make_empty_model(pop.ni().knobs(),
                       make_indirect_iterator(to_model.begin()),
                       make_indirect_iterator(to_model.end()),g);


      //update the gain offset
      BDe::gainOffset=(log2(gain_t(distance(to_model.begin(),
                                            to_model.end())))/2.0);
      //BDe::gainOffset=(log(gain_t(distance(to_model.begin(),
      //to_model.end())))/2.0);

      //learn dependencies
      dtree_learn(g);

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
      // cout << "nubest: " << it->first << " " << mycomplexity(it->first.begin()) << endl;
      /**/
      /**cout << "     " << gen
           << " sampled " << n_evals
           << " best " << best << endl;**/
    }
    return make_pair(n_evals,best);
  }

  template<typename Simplify,typename FF,typename TreeDist>
  struct region_expander : public noncopyable {
    typedef neighborhood_params<Simplify> neighborhood_params;
    typedef neighborhood<Simplify> neighborhood;
    typedef config<FF,TreeDist> config;
    typedef population<Simplify,FF,TreeDist> population;

    region_expander(const neighborhood_params& ni_params,const config& c,int n,
                    const tree<Vertex>& tr)
      : _ni_params(ni_params),_c(c),_n(n),_best_seen(NEG_INFINITY),
        _n_evals(0),_ss(_ni_params.selector())
    {
      _cmap.insert(make_pair(tr,fitness_t(0)));
    }
    
    int n_evals() const { return _n_evals; }
    fitness_t best_fitness() const { return _best_seen; }

    void expand(int neval,float tMin,fitness_t cutoff) {
      assert(!empty());

      //select a region to use
      candidate_map::iterator exemplar=_cmap.begin();
      fitness_t fbase=ceil(exemplar->second);
      int smin=POS_INFINITY,smax=NEG_INFINITY;  
      vector<pair<candidate_map::iterator,int> > tmp;
      for (candidate_map::iterator to=_cmap.begin();
           ceil(to->second)>=fbase && to!=_cmap.end();++to) {
        int c=mycomplexity(to->first.begin());
        if (c-11>smin)
          break;
        tmp.push_back(make_pair(to,c));
        smin=min(smin,c);
        smax=max(smax,c);
      }
      int ssum=0;
      for (unsigned int i=0;i<tmp.size();++i) {
        int p=(1<<(smax-tmp[i].second));
        ssum+=p;
        tmp[i].second=p;
        //cout << "adding " << (tmp[i].first->first) << " | " << p << endl;
      }
      exemplar=roulette_select(tmp.begin(),tmp.end(),
                               select2nd<pair<candidate_map::iterator,int> >(),
                               ssum)->first;
      
      population pop(_ni_params,_c,exemplar->first);
      _visited.insert(exemplar->first);
      _cmap.erase(exemplar);

      //a pain to implement checking for repeating regions, so we don't

      //generate initial pop
      _n_evals+=pop.reset(_n,_ss,tMin);

      cout << "model size: " << pop.ni().length() << endl;

      //run some boa
      int lim=min(neval,int(_c.n_gens_ratio()*pop.ni().nbits()));
      //int tt=int(sqrtf(pop.ni().nbits())+0.5)+1;
      int tsize=min(int(pop.ni().nbits()+0.5),int(float(pop.size())*tMin+0.5));
      int tt=int(sqrtf(float(pop.size())/float(tsize)))+1;

      fitness_t initial=NEG_INFINITY;
      for (int i=0,p=0;i<lim && _n_evals<neval && p<tt;++i,++p) {
        int n;
        fitness_t b;
        tie(n,b)=boa(pop,1,tMin,cutoff);
        _n_evals+=n;
        if (b>_best_seen)
          _best_seen=b;

        cout << "sampled " << _n_evals
             << " best " << _best_seen << endl;

        if (b>=cutoff)
          return;

        if (n<5)
          break;

        if (b>initial) {
          p=0;
          initial=b;
        }
      }
      //compute non-dominating set and add it to the q
      pop.nondominating(_cmap,_visited);

      cout << "q size " << _cmap.size() << endl;

      if (_cmap.size()>2) {
        cout << _cmap.begin()->first << " "
             << mycomplexity(_cmap.begin()->first.begin()) << " " 
             << _cmap.begin()->second << endl;
        cout << (++_cmap.begin())->first << " "
             << mycomplexity((++_cmap.begin())->first.begin()) << " " 
             << (++_cmap.begin())->second << endl;
        cout << (++(++_cmap.begin()))->first << " "
             << mycomplexity((++(++_cmap.begin()))->first.begin()) << " " 
             << (++(++_cmap.begin()))->second << endl;
      } else if (_cmap.size()==2) {
        cout << _cmap.begin()->first << " "
             << mycomplexity(_cmap.begin()->first.begin()) << " "
             << _cmap.begin()->second << endl;
        cout << (++_cmap.begin())->first << " "
             << mycomplexity((++_cmap.begin())->first.begin()) << " "
             << (++_cmap.begin())->second << endl;
      }  else if (_cmap.size()==1) {
        cout << _cmap.begin()->first << " "
             << mycomplexity(_cmap.begin()->first.begin()) << " "
             << _cmap.begin()->second << endl;
      }
    }

    bool empty() const { return _cmap.empty(); }

  private:
    const neighborhood_params _ni_params;
    const config& _c;
    int _n;
    fitness_t _best_seen;    
    int _n_evals;
    semantic_sampler<Vertex,Simplify> _ss;

    candidate_map _cmap;
    hash_set<vtree> _visited;
  };

  template<typename Simplify,typename FF,typename TreeDist>
  int megamoses(const neighborhood_params<Simplify>& ni_params,
                const config<FF,TreeDist>& c,int pop_size,int n_evals,
                float tMin,fitness_t cutoff=-0.99f,
                const tree<Vertex>& tr=tree<Vertex>(create_and<Vertex>())) {
    region_expander<Simplify,FF,TreeDist> re(ni_params,c,pop_size,tr);
    while (re.n_evals()<n_evals) {
      //run a generation
      re.expand(n_evals,tMin,cutoff);

      //print the generation number and best solution
      /**cout << "sampled " << re.n_evals()
         << " best " << re.best_fitness() << endl;**/

      if (re.best_fitness()>=cutoff || re.empty())
        break;
    }

    

    return re.n_evals();
  }

} //~namespace moses

#endif
