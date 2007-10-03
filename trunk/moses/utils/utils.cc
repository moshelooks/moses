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
 * utils.cc
 *
 * $Header: /vetta/cvsroot/novamente/src/utils/utils.cc,v 1.1.1.1.2.4 2005/02/11 21:38:19 gama Exp $
 *
 * Copyright(c) 2001 Thiago Maia, Andre Senna
 * All rights reserved.
 */

#include <stdlib.h>
#include <stdarg.h>
#include <dirent.h>
#include "utils.h"
#include "exceptions.h"

char* bold(int i) {
    static char buf[256];
    char* answer = buf;
    sprintf(answer, "[1m%d[0m", i);
    return answer;
}

char* bold(const char* str) {
    static char buf[256];
    char* answer = buf;
    sprintf(answer, "[1m%s[0m", str);
    return answer;
}

void setIconAndWindowTitle(const char* title) {
    cprintf(SHY, "]0;%s", title);
    fflush(stdout);
}

int currentDebugLevel = NORMAL;

int cprintf(int debugLevel, const char *format, ...) {
    if (debugLevel > currentDebugLevel) return 0;
    va_list ap;
    va_start(ap, format);
    int answer = vprintf(format, ap);
    fflush(stdout);
    va_end(ap);
    return answer;
}
    
char* padstr(char* s, unsigned int size, padAlignment a) {
    switch (a) {
        case CENTER:
            {
                unsigned int lpadding = (size - strlen(s)) / 2;
                unsigned int rpadding = lpadding;
                if (lpadding + rpadding + strlen(s) != size) rpadding++;
                char* answer = (char*) malloc((size + 1) * sizeof(char));
                for (unsigned int i = 0; i < size; i++) answer[i] = ' ';
                for (unsigned int i = 0; i < strlen(s); i++) answer[i + lpadding] = s[i];
                answer[size] = '\0';
                return answer;
            }
        case LEFT:
            {
                unsigned int rpadding = size - strlen(s);
                char* answer = (char*) malloc((size + 1) * sizeof(char));
                for (unsigned int i = rpadding; i < size; i++) answer[i] = ' ';
                for (unsigned int i = 0; i < strlen(s); i++) answer[i] = s[i];
                answer[size] = '\0';
                return answer;
            }
        case RIGHT:
            {
                unsigned int lpadding = size - strlen(s);
                char* answer = (char*) malloc((size + 1) * sizeof(char));
                for (unsigned int i = 0; i < lpadding - 1; i++) answer[i] = ' ';
                for (unsigned int i = 0; i < strlen(s); i++) answer[i + lpadding] = s[i];
                answer[size] = '\0';
                return answer;
            }
    }
    throw new RuntimeException("illegal format argument");
}

FileList *FileList::getAllFilesRecursively(char *arg) {

    //printf("Getting files from <%s>\n", arg);
    FileList *answer = new FileList();
    DIR *d = opendir(arg);
    if (d == NULL) {
        //printf("is file\n");
        //is file
        answer->fileList = (char **) malloc(sizeof(char *));
        answer->fileList[0] = strdup(arg);
        answer->size = 1;
        return answer;
    } else {
        //printf("is dir\n");
        // is dir
        struct dirent *dp;
        while ((dp = readdir(d)) != NULL) {
            if (!strcmp(dp->d_name, ".") || (!strcmp(dp->d_name, ".."))) {
                continue;
            }
            char buf[1 << 16];
            sprintf(buf, "%s", arg);
            if (arg[strlen(arg) - 1] != '/') strcat(buf, "/");
            strcat(buf, dp->d_name);
            FileList *aux = getAllFilesRecursively(buf);
            answer->fileList = (char **) realloc(answer->fileList, ((answer->size + aux->size) * sizeof(char *)));
            for (int i = 0; i < aux->size; i++) {
                answer->fileList[answer->size + i] = strdup(aux->fileList[i]);
            }
            answer->size += aux->size;
            delete aux;
        }
        closedir(d);
        return answer;
    }
}

FileList::FileList() {
    size = 0;
    fileList = NULL;
    //fileList = (char **) malloc(0);
}

FileList::FileList(char *arg) {

    size = 0;
    fileList = NULL;
    //fileList = (char **) malloc(0);

    DIR *d = opendir(arg);
    if (d != NULL) {
        struct dirent *dp;
        while ((dp = readdir(d)) != NULL) {
            if (!strcmp(dp->d_name, ".") || (!strcmp(dp->d_name, ".."))) {
                continue;
            }
            char buf[1 << 16];
            sprintf(buf, "%s", arg);
            if (arg[strlen(arg) - 1] != '/') strcat(buf, "/");
            strcat(buf, dp->d_name);
            fileList = (char **) realloc(fileList, ++size * sizeof(char *));
            fileList[size - 1] = strdup(buf);
        }
        closedir(d);
    } else {
        FILE *f = fopen(arg, "r");
        if (f == NULL) {
            throw new RuntimeException("Unable to open file or directory %s", arg);
        }
        fclose(f);
        size = 1;
        fileList = (char **) malloc(sizeof(char *));
        fileList[0] = strdup(arg);
    }
}

FileList::~FileList() {
    for (int i = 0; i < size; i++) {
        free(fileList[i]);
    }
    free(fileList);
}

int FileList::getSize() {
    return size;
}

char *FileList::getFile(int i) {
    if (i >= size) {
        throw new RuntimeException("invalid file index");
    }
    return fileList[i];
}

char *nextLine(char *from, char *&line){
	line = from;
	
	if (from == NULL){
		return(NULL);
	}
	
	while ((*from != '\n') && ((*from != '\0'))){
		from++;
	}
	
	if (*from == '\0'){
		from = NULL;
	}else{
		*from = '\0';
		from++;
	}
	
	return(from);
}

int bitcount(unsigned long n) {
    /* works for 32-bit numbers only    */
    /* fix last line for 64-bit numbers */

    register unsigned long tmp;

    tmp = n - ((n >> 1) & 033333333333)
            - ((n >> 2) & 011111111111);
    return ((tmp + (tmp >> 3)) & 030707070707) % 63;
}


