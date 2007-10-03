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

#include <functional>
#include "tree.h"
#include "math_util.h"

namespace fitness {
  using namespace id;

  static const int ANT_X=32;
  static const int ANT_Y=32;
  static char trail[ANT_Y][ANT_X+1];
  static const char trail2[ANT_Y][ANT_X+1]={
    " 888                            ",
    "   8                            ",
    "   8                     888    ",
    "   8                    8    8  ",
    "   8                    8    8  ",
    "   8888 88888        88         ",
    "            8                8  ",
    "            8       8           ",
    "            8       8           ",
    "            8       8        8  ",
    "                    8           ",
    "            8                   ",
    "            8                8  ",
    "            8       8           ",
    "            8       8     888   ",
    "                 8     8        ",
    "                                ",
    "            8                   ",
    "            8   8       8       ",
    "            8   8          8    ",
    "            8   8               ",
    "            8   8               ",
    "            8             8     ",
    "            8          8        ",
    "   88  88888    8               ",
    " 8              8               ",
    " 8              8               ",
    " 8      8888888                 ",
    " 8     8                        ",
    "       8                        ",
    "  8888                          ",
    "                                "};

  template<typename T>
  class Ant : public std::unary_function<tree<T>,float> {
  public:    
    typedef typename tree<T>::sibling_iterator sib_it;
    enum Direction { north=0,east=1,south=2,west=3 };
    static void turn_left(Direction& d) { d=(Direction)(((int)d+3)%4); }
    static void turn_right(Direction& d) { d=(Direction)(((int)d+1)%4); }
    static void reverse(Direction& d) { d=(Direction)(((int)d+2)%4); }


    Ant(int steps=600) : 
      _steps(steps) { }

    pair<bool,bool> dominates(const tree<T>& x,const tree<T>& y) const {
      float xFit=(*this)(x),yFit=(*this)(y);
      return 
	xFit>yFit ? make_pair(true,false) :
	yFit>xFit ? make_pair(false,true) : 
	make_pair(false,false);
    }
			
    float operator()(const tree<T>& t) const {
      //cout << "zz" << endl;
      copy(trail2[0],trail2[0]+sizeof(trail2),trail[0]); //nasty..
      //cout << "top, " << t << endl;
      if (t.empty())
	return NEG_INFINITY;
      int x=0,y=0;
      Direction facing=east;
      int at_time=0;
      int sc=0;
      do {
	int tmp=at_time;
	sc+=eval(t.begin(),x,y,facing,at_time);
	if (at_time==tmp)
	  break;
      } while (at_time<_steps);
      //cout << "bot, ok" << endl;
      return (float)sc;
    }

    int eval(sib_it it,int& x,int& y,Direction& facing,int& at_time) const {
      /**
      for (int i=0;i<32;++i) {
	for (int j=0;j<32;++j)
	  cout << trail[i][j];
	cout << endl;
      }
      **/
      if (at_time>=_steps)
	return 0;
      if (is_progn(*it)) { //need to recurse
	int res=0;
	for (sib_it sib=it.begin();sib!=it.end();++sib)
	  res+=eval(sib,x,y,facing,at_time);
	return res;
      } else if (it.number_of_children()==0) {
	++at_time;
	if (is_move(*it)) { //move forward
	  switch(facing) {
	  case east : 
	    x=(x+1)%ANT_X;
	    break;
	  case west : 
	    x=(x-1+ANT_X)%ANT_X;
	    break;
	  case south:
	    y=(y+1)%ANT_Y;
	    break;
	  case north:
	    y=(y-1+ANT_Y)%ANT_Y;
	    break;
	  }
	  if (trail[y][x]!=' ') {
	    trail[y][x]=' ';
	    return 1;
	  }
	} else if (is_left(*it)) {  //rotate left
	  turn_left(facing);
	} else if (is_right(*it)) { //rotate right
	  turn_right(facing);
	} else {
	  ++at_time;
	  assert(is_reversal(*it));
	  reverse(facing);
	}
	return 0;
      } else {
	assert(is_if_food(*it));
	int xn=x,yn=y;
	//if-food-ahead
	switch(facing) {
	case east: 
	  xn=(x+1)%ANT_X;
	  break;
	case west: 
	  xn=(x-1+ANT_X)%ANT_X;
	  break;
	case south:
	  yn=(y+1)%ANT_Y;
	  break;
	case north:
	  yn=(y-1+ANT_Y)%ANT_Y;
	  break;
	}
	return eval(trail[yn][xn]!=' ' ? it.begin() : ++it.begin(),
		    x,y,facing,at_time);
      }
    }
  private:
    const int _steps;
  };
  
} //~namespace fitness
