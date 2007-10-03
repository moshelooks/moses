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

/**
 * exceptions.h
 *
 * $Header: /vetta/cvsroot/novamente/src/utils/exceptions.h,v 1.1.1.1 2003/11/12 15:35:22 barra Exp $
 *
 * Copyright(c) 2001 Thiago Maia, Andre Senna
 * All rights reserved.
 */

#ifndef EXCEPTIONS_H
#define EXCEPTIONS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdarg.h>

/**
 * Generic exception to be called in runtime, whenever an unexpected condition
 * is detected.
 */
class RuntimeException {

    public:

        /**
         * Generic exception to be called in runtime, whenever an unexpected
         * condition is detected.
         *
         * @param Exception message in printf standard format.
         */
        RuntimeException(const char*, ...);
};

class AssertionException {

    public:

        AssertionException(const char*, ...);
        AssertionException(const char* fmt, va_list ap);
};

void cassert(bool, const char *, ...);

#endif
