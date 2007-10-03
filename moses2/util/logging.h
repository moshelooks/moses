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

#ifndef _UTIL_LOGGING_H
#define _UTIL_LOGGING_H

//for now this is an evil little macro
//maybe later something nicer will go here...

#ifdef DISABLE_SIMPLE_LOGGING
#  define SIMPLE_LOG(logEvent)
#  define SIMPLE_LOGLN(logEvent)
#else
#  include <iostream>
#  include <sstream>
#  define SIMPLE_LOG(logEvent) \
    do { \
      std::stringstream __eda_log_buf; \
      __eda_log_buf << logEvent; \
      std::cout << __eda_log_buf.str(); \
      } while(0);
#  define SIMPLE_LOGLN(logEvent) \
    do { \
      std::stringstream __eda_log_buf; \
      __eda_log_buf << logEvent; \
      std::cout << __eda_log_buf.str() << " @@@ " << __FILE__ << " line " << __LINE__ << std::endl; \
      } while(0);
#endif

#if 1
#  include <iostream>
#  include <sstream>
#  define SIMPLE_PRINT(logEvent) \
    do { \
      std::stringstream __eda_log_buf; \
      __eda_log_buf << logEvent; \
      std::cout << __eda_log_buf.str(); \
      } while(0);
#  define SIMPLE_PRINTLN(logEvent) \
    do { \
      std::stringstream __eda_log_buf; \
      __eda_log_buf << logEvent; \
      std::cout << __eda_log_buf.str() << " @@@ " << __FILE__ << " line " << __LINE__ << std::endl; \
      } while(0);
#endif

#endif
