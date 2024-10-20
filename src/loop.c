#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#include "builtin.h"
#include "constants.h"
#include "loop.h"
#include "global.h"


void print_ps(void) {
    printf(SET_COLOR("%s", COLOR_GREEN) SET_COLOR(" @ %s %s\n", COLOR_CYAN), user, host, cwd);
    printf(SET_COLOR("$ ", COLOR_GREEN));
}

char* getcmd(void) {
    int buffer_size = BUFF_INI_SIZE * sizeof(char);
    int buffer_pos = 0;
    char* buffer = malloc(buffer_size);
    if (!buffer) {
        PRINT_ERRNO();
        exit(EXIT_FAILURE);
    }

    int c;
    while (1) {
        c = getchar();
        if (c == EOF) {
            printf("\n");
            free(buffer);
            exit(EXIT_SUCCESS);
        } else if (c == '\n') {
            buffer[buffer_pos] = '\0';
            return buffer;
        } else {
            buffer[buffer_pos] = c;
        }

        if (++buffer_pos == buffer_size) {
            buffer_size *= GROWTH_FACTOR;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                PRINT_ERRNO();
                exit(EXIT_FAILURE);
            }
        }
    }

    return buffer;
}

char** tokenize(char* cmd) {
    int buffer_size = BUFF_INI_SIZE * sizeof(char*);
    int buffer_pos = 0;
    char** buffer = malloc(buffer_size);
    if (!buffer) {
        PRINT_ERRNO();
        exit(EXIT_FAILURE);
    }

    enum State { SPACE, CHAR, QUOTE } state = SPACE;
    while (*cmd) {
        switch (state) {
            case SPACE:
                if (*cmd == '\"') {
                    buffer[buffer_pos++] = cmd + 1;
                    state = QUOTE;
                } else if (*cmd != ' ') {
                    buffer[buffer_pos++] = cmd;
                    state = CHAR;
                }
                if (buffer_pos == buffer_size) {
                    buffer_size *= GROWTH_FACTOR;
                    buffer = realloc(buffer, buffer_size);
                    if (!buffer) {
                        PRINT_ERRNO();
                        exit(EXIT_FAILURE);
                    }
                }
                break;
            case CHAR:
                if (*cmd == ' ') {
                    *cmd = '\0';
                    state = SPACE;
                }
                break;
            case QUOTE:
                if (*cmd == '\"') {
                    *cmd = '\0';
                    state = SPACE;
                }
                break;
        }
        ++cmd;
    }

    buffer[buffer_pos] = NULL;
    return buffer;
}

int launch(char** args) {
    int status;
    int pid = fork();
    if (pid < 0) {
        PRINT_ERRNO();
    } else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            PRINT_ERRNO();
            exit(EXIT_FAILURE);
        }
    } else {
        do {
            waitpid(pid, &status, WUNTRACED);
        } while (!WIFEXITED(status) && !WIFSIGNALED(status));
    }

    return 1;
}

int execute(char** args) {
    if (!args[0]) {
        return 1;
    } else {
        for (int i = 0; i < num_builtins(); ++i) {
            if (strcmp(args[0], builtin_str[i]) == 0) {
                return builtin_func[i](args);
            }
        }
    }

    return launch(args);
}

void loop(void) {
    char* cmd;
    char** args;
    int status;

    do {
        print_ps();
        cmd = getcmd();
        args = tokenize(cmd);
        status = execute(args);

        free(cmd);
        free(args);
    } while (status);
}
