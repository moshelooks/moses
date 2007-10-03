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
 * utils.h
 *
 * $Header: /vetta/cvsroot/novamente/src/utils/utils.h,v 1.1.1.1.2.4 2004/05/31 14:30:59 murilo Exp $
 *
 * Copyright(c) 2001 Thiago Maia, Andre Senna
 * All rights reserved.
 */

#ifndef UTILS_H
#define UTILS_H

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <ext/hash_map>

using namespace __gnu_cxx;
using namespace std;

// defines print levels
#define SILENT          0
#define SHY             1
#define NORMAL          2
#define LOCAL_DEBUG     3
#define VERBOSE         4
#define DEBUG           5


/**
 * Returns a copy of the given string surrounded by ANSI bold tags. This copy
 * is allocated in the stack, so it must be duplicated if not used
 * immediately.
 *
 * @return Copy of the given string surrounded by ANSI bold tags.
 */
char* bold(const char*);

/**
 * Returns the string representation of an integer surrounded by ANSI bold
 * tags. This copy is allocated in the stack, so it must be duplicated if not
 * used immediately.
 *
 * @return The string representation of an integer surrounded by ANSI bold
 * tags.
 */
char* bold(int i);

/**
 * Prints the ANSI sequence to change the icon and window title in the window
 * manager.
 *
 * @param New title.
 */
void setIconAndWindowTitle(const char*);

/**
 * Keeps the current debug level in the system to control the cprintf function.
 */
extern int currentDebugLevel;

/**
 * Core printf function that prints messages according to the current debug
 * level. Format is identical to the standard printf format.
 *
 * @param Desired debug level.
 * @param Format according to the standard printf format.
 */
int cprintf(int, const char *format, ...);

struct eqstr {
    bool operator()(char *s1, char *s2) const {
        return strcmp(s1, s2) == 0;
    } 
};

struct eqconststr {
    bool operator()(const char *s1, const char *s2) const {
        return strcmp(s1, s2) == 0;
    } 
};

struct eqint {
    bool operator()(int s1, int s2) const {
        return s1 == s2;
    } 
};

struct eqlong {
    bool operator()(long s1, long s2) const {
        return s1 == s2;
    } 
};

struct eq2int {
    bool operator()(const char *s1, const char *s2) const {
        int size = 2 * sizeof(int);
        for (int i = 0; i < size; i++) {
            if (s1[i] != s2[i]) return false;
        }
        return true;
    } 
};

template <class _Key> struct hash2int { };

inline size_t __hash2int(const char* __s) {
    unsigned long __h = 0; 
    size_t size = 2 * sizeof(int);
    for (size_t i = 0; i < size; i++, __s++) {
        __h = 5*__h + *__s;
    }
    return size_t(__h);
}

template <> struct hash2int<char *> {
    size_t operator()(const char* __s) const { return __hash2int(__s); }
};

enum padAlignment { CENTER, LEFT, RIGHT };
char* padstr(char*, unsigned int, padAlignment);

class FileList {

    private:

        int size;
        char **fileList;

    public:

        FileList(char *);
        FileList();
        ~FileList();

        static FileList *getAllFilesRecursively(char *);

        int getSize();
        char *getFile(int);
};

/**
 * This method reads a line from a string and returns
 * the rest of the string.
 *
 * @param The char* from where the line should be read from.
 * @param A char* that will point to the start of the line.
 * @return The rest of the string.
 * Note: The string from where the line should be read from is
 * modified by nextLine.
 */
char *nextLine(char *, char *&);

/**
 * Counts the number of bits in 1 in the given unsigned long argument.
 */
int bitcount(unsigned long n);

#endif
