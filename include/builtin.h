#ifndef BUILTIN_H
#define BUILTIN_H


extern char* builtin_str[];
extern int (*builtin_func[])(char**);

int num_builtins(void);
int cd(char** args);
int quit(char** args);


#endif
