#include <errno.h>
#include <limits.h>
#include <linux/limits.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

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


char* user;
char host[HOST_NAME_MAX];
char cwd[PATH_MAX];

char* getusername() {
    struct passwd* pw = getpwuid(geteuid());
    if (!pw) {
        PRINT_ERRNO();
        exit(EXIT_FAILURE);
    }

    return pw -> pw_name;
}

void init(void) {
    if (!(user = getusername()) || gethostname(host, HOST_NAME_MAX) || !getcwd(cwd, PATH_MAX)) {
        PRINT_ERRNO();
        exit(EXIT_FAILURE);
    }
}

void print_ps(void) {
    printf(SET_COLOR("%s", COLOR_GREEN) SET_COLOR(" @ %s %s\n", COLOR_CYAN), user, host, cwd);
    printf(SET_COLOR("$ ", COLOR_GREEN));
}


int cd(char** args);
int quit(char** args);

char* builtin_str[] = {
    "cd",
    "exit"
};

int (*builtin_func[])(char**) = {
    &cd,
    &quit
};

int num_builtins() {
    return sizeof(builtin_str) / sizeof(char*);
}

int cd(char** args) {
    if (!args[1]) {
        PRINT_ERR("insufficient arguments");
    } else {
        if (chdir(args[1]) != 0 || !getcwd(cwd, PATH_MAX)) {
            PRINT_ERRNO();
        }
    }

    return 1;
}

int quit(char** args) {
    return 0;
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

    enum State { CHAR, SPACE, QUOTE } state = SPACE;
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
        for (int i = 0; i < num_builtins(); ++i)
            if (strcmp(args[0], builtin_str[i]) == 0)
                return builtin_func[i](args);
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

int main(int argc, char* argv[]) {
    init();
    loop();
    return EXIT_SUCCESS;
}
