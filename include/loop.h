#ifndef LOOP_H
#define LOOP_H


void print_ps(void);
char* getcmd(void);
char** tokenize(char* cmd);
int launch(char** args);
int execute(char** args);
void loop(void);


#endif
