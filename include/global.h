#ifndef GLOBAL_H
#define GLOBAL_H

#include <limits.h>

#ifdef DECLARE_GLOBALS
char* user;
char host[HOST_NAME_MAX];
char cwd[PATH_MAX];
#else
extern char* user;
extern char host[HOST_NAME_MAX];
extern char cwd[PATH_MAX];
#endif

#endif
