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

#ifndef _UTIL_NUMERIC_H
#define _UTIL_NUMERIC_H

#include <cmath>
#include <cstdlib>
#include <climits>
#include <vector>
#include <numeric>

#define PI 3.141592653589793
#define EXPONENTIAL 2.71828182845905
#define EPSILON 1e-6 //error when comparing 2 floats

#ifndef isnan
#define isnan(x) ((x) != (x))
#endif

#ifndef isfinite
#define isfinite(x) (!isnan((x) - (x)))
#endif

#ifndef isinf
#define isinf(x) (!isnan(x) & !isfinite(x))
#endif

#include <ext/numeric>

namespace util {
  //this needs to be changed for non-gcc
  using __gnu_cxx::power;
  using __gnu_cxx::iota;

  //random float in [0,1]
  inline float randfloat() { return (float)rand()/(float)RAND_MAX; }
  //random double in [0,1]
  inline double randdouble() { return double(rand())/double(RAND_MAX); }
  //random int in [0,n)
  inline int randint(int n) { return rand()%n; }
  //random boolean
  inline bool randbool() { return rand()%2==0; }


  // absolute_value_order 
  //   codes the following order, for T == int, -1,1,-2,2,-3,3,...
  template<typename T>
  struct absolute_value_order {
    bool operator()(T a,T b) const { return (a==-b) ? a<b : abs(a)<abs(b); }
  };
  template<typename T>
  struct absolute_value_equality {
    bool operator()(T a,T b) const { return (a==b || a==-b); }
  };

  // the following functions are adapted from the bit twiddling hacks page:
  // http://graphics.stanford.edu/~seander/bithacks.html
  // 32-bit version is faster, than the others 
  // but beware, you need to check the number of bits yourself
  // awkward phrasing cuz c++ doesn't allow partial specialization of functions
  //
  // a few possibilities were tried; gcc has a built-in function called
  // __builtin_popcount which is somewhat slower. Using a lookup table might
  // be somwhat faster under some circumstances, but was avoided because it
  // might hog the fast memory ...
  template<int> struct bits {
    template<typename T>
    static inline unsigned int count(T v);
  };
  template<> struct bits<32> {
    template<typename T>
    static inline unsigned int count(T v) {
      v = v - ((v >> 1) & 0x55555555);                       // reuse v as temp
      v = (v & 0x33333333) + ((v >> 2) & 0x33333333);        // temp
      return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
    }
    template<typename T>
    static inline void interleave(T& v) {
      static const unsigned int B[] = 
	{0x55555555, 0x33333333, 0x0F0F0F0F, 0x00FF00FF};
      static const unsigned int S[] = {1, 2, 4, 8};

      T upperbits=(v>>16);
      upperbits = (upperbits | (upperbits << S[3])) & B[3];
      upperbits = (upperbits | (upperbits << S[2])) & B[2];
      upperbits = (upperbits | (upperbits << S[1])) & B[1];
      upperbits = (upperbits | (upperbits << S[0])) & B[0];
      v&=0xFFFF; //take the lower 16 bits
      v = (v | (v << S[3])) & B[3];
      v = (v | (v << S[2])) & B[2];
      v = (v | (v << S[1])) & B[1];
      v = (v | (v << S[0])) & B[0];
      v = v | (upperbits << 1);
    }
  };
  template<> struct bits<64> {
    template<typename T>
    static inline unsigned int count(T v) {
      v = v - ((v >> 1) & (T)~(T)0/3);                           // temp
      v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);      // temp
      v = (v + (v >> 4)) & (T)~(T)0/255*15;                      // temp
      return ((T)(v * ((T)~(T)0/255)) >> (sizeof(v) - 1) * CHAR_BIT);
    }
    template<typename T>
    static inline void interleave(T& v) {
      unsigned int tmp1=((v>>48)<<16);
      tmp1|=(v<<32)>>48;
      bits<32>::interleave(tmp1);
      unsigned int tmp2=((v>>16) & 0xFFFF0000);
      tmp2|=(v & 0xFFFF);
      bits<32>::interleave(tmp2);
      v=(T(tmp1)<<32 | T(tmp2));
    }
  };
  template<> struct bits<128> {
    template<typename T>
    static inline unsigned int count(T v) {
      v = v - ((v >> 1) & (T)~(T)0/3);                           // temp
      v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);      // temp
      v = (v + (v >> 4)) & (T)~(T)0/255*15;                      // temp
      return ((T)(v * ((T)~(T)0/255)) >> (sizeof(v) - 1) * CHAR_BIT);
    }
  };

  // count_bits will work up to 128-bits
  template<typename T>
  inline unsigned int count_bits32(T v) {
    v = v - ((v >> 1) & 0x55555555);                    // reuse input as temp
    v = (v & 0x33333333) + ((v >> 2) & 0x33333333);     // temp
    return ((v + (v >> 4) & 0xF0F0F0F) * 0x1010101) >> 24; // count
  }
  template<typename T>
  inline unsigned int count_bits(T v) {
    v = v - ((v >> 1) & (T)~(T)0/3);                           // temp
    v = (v & (T)~(T)0/15*3) + ((v >> 2) & (T)~(T)0/15*3);      // temp
    v = (v + (v >> 4)) & (T)~(T)0/255*15;                      // temp
    return ((T)(v * ((T)~(T)0/255)) >> (sizeof(v) - 1) * CHAR_BIT);
  }

  inline size_t next_power_of_two(size_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
  }

  inline unsigned int integer_log2(size_t v) {
    static const int MultiplyDeBruijnBitPosition[32] = 
      {
	0, 1, 28, 2, 29, 14, 24, 3, 30, 22, 20, 15, 25, 17, 4, 8, 
	31, 27, 13, 23, 21, 19, 16, 7, 26, 12, 18, 6, 11, 5, 10, 9
      };

    v |= v >> 1; // first round down to power of 2 
    v |= v >> 2;
    v |= v >> 4;
    v |= v >> 8;
    v |= v >> 16;
    v = (v >> 1) + 1;
    return MultiplyDeBruijnBitPosition[(v * 0x077CB531UL) >> 27];
  }
  
  inline unsigned int nbits_to_pack(size_t arity) {
    return next_power_of_two(integer_log2(arity-1)+1);
  }

  //sums of natural logarithms (for a particular floating-point type)
  template<typename FloatT>
  FloatT logsum(size_t n) {
    static std::vector<FloatT> sums;
    if (n>=sums.size()) {
      int to=next_power_of_two(n)+2;
      sums.reserve(to);
      if (sums.empty()) {
	sums.push_back(0);
	sums.push_back(0);
      }
      
      FloatT v=sums.back();
      for (int i=sums.size();i<to;++i) {
	v+=log(FloatT(i-1));
	sums.push_back(v);
      }
    }
    return sums[n];
  }

  //compare 2 FloatT with precision EPSILON
  template<typename FloatT> bool isEqual(FloatT x, FloatT y) {
    const FloatT epsilon = EPSILON;
    FloatT diff = std::abs(x-y);
    FloatT amp = std::abs(x+y);
    if(amp*amp > epsilon)
      return diff <= epsilon * amp;
    else return diff <= epsilon;
  }

} //~namespace util

#endif
