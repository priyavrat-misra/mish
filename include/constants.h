#ifndef CONSTANTS_H
#define CONSTANTS_H


#include <errno.h>
#include <stdio.h>
#include <string.h>

#define PRINT_ERR(e)    fprintf(stderr, SET_COLOR(SHELL_NM ": %s\n", COLOR_RED), e)
#define PRINT_WRN(w)    fprintf(stdout, SET_COLOR(SHELL_NM ": %s\n", COLOR_YELLOW), w)
#define PRINT_ERRNO()   PRINT_ERR(strerror(errno))

#define SET_COLOR(s, c) ANSI_ESC(c) s ANSI_RESET

#define STR(s) #s
#define ANSI_ESC(c)     "\033[3" STR(c) "m"
#define ANSI_RESET      "\033[0m"

#define COLOR_BLACK     0
#define COLOR_RED       1
#define COLOR_GREEN     2
#define COLOR_YELLOW    3
#define COLOR_BLUE      4
#define COLOR_MAGENTA   5
#define COLOR_CYAN      6
#define COLOR_WHITE     7

#define SHELL_NM        "mish"
#define BUFF_INI_SIZE   8
#define GROWTH_FACTOR   2


#endif
