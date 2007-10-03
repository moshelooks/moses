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
 * exceptions.cc
 *
 * $Header: /vetta/cvsroot/novamente/src/utils/exceptions.cc,v 1.1.1.1.2.1 2004/04/30 17:52:22 barra Exp $
 *
 * Copyright(c) 2001 Thiago Maia, Andre Senna
 * All rights reserved.
 */

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "exceptions.h"
#include "utils.h"

RuntimeException::RuntimeException(const char* fmt, ...) {
    
    char    buf[1024];

    va_list        ap;
    va_start(ap, fmt);

    vsnprintf(buf, sizeof(buf), fmt, ap);
    strcat(buf, "\n");

    fflush(stdout);
    cprintf(SILENT, buf);
    fflush(stdout);

    va_end(ap);
}

AssertionException::AssertionException(const char* fmt, ...) {
    
    char    buf[1024];

    va_list        ap;
    va_start(ap, fmt);

    vsnprintf(buf, sizeof(buf), fmt, ap);
    strcat(buf, "\n");

    fflush(stdout);
    cprintf(SILENT, buf);
    fflush(stdout);

    va_end(ap);
}

AssertionException::AssertionException(const char* fmt, va_list ap) {
    
    char    buf[1024];

    vsnprintf(buf, sizeof(buf), fmt, ap);
    strcat(buf, "\n");

    fflush(stdout);
    cprintf(SILENT, buf);
    fflush(stdout);
}

void cassert(bool condition, const char *fmt, ...) {

    if (condition) return;

    va_list        ap;
    va_start(ap, fmt);
	AssertionException ex = AssertionException(fmt, ap);
    va_end(ap); 
	throw ex;
}
