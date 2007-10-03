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

#ifndef MEDIANSTORE_H
#define MEDIANSTORE_H

#include <vector>

class MedianStore {
public:
  MedianStore(int size) : _store(size) { }

  void addItem(int idx);
  void removeItem(int idx);
  int getCountBelowEq(int idx) const;
  int getItemByCount(int count) const;
private:
  std::vector<int> _store;
};

#endif
