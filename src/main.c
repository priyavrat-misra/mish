#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define BUFF_INI_SIZE 16
#define GROWTH_FACTOR 2
#define ARG_DLIMITERS " \t\r\n"

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
    if (args[1] == NULL) {
        fprintf(stderr, "insufficient arguments\n");
    } else {
        if (chdir(args[1]) != 0) {
            fprintf(stderr, "invalid directory\n");
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
        fprintf(stderr, "malloc failed\n");
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
                fprintf(stderr, "realloc failed\n");
                exit(EXIT_FAILURE);
            }
        }
    }

    return buffer;
}

char** parse(char* cmd) {
    int buffer_size = BUFF_INI_SIZE * sizeof(char*);
    int buffer_pos = 0;
    char** buffer = malloc(buffer_size);
    if (!buffer) {
        fprintf(stderr, "malloc failed\n");
        exit(EXIT_FAILURE);
    }

    char* arg = strtok(cmd, ARG_DLIMITERS);
    while (arg) {
        buffer[buffer_pos] = arg;
        if (++buffer_pos == buffer_size) {
            buffer_size *= GROWTH_FACTOR;
            buffer = realloc(buffer, buffer_size);
            if (!buffer) {
                fprintf(stderr, "realloc failed\n");
                exit(EXIT_FAILURE);
            }
        }
        arg = strtok(NULL, ARG_DLIMITERS);
    }

    buffer[buffer_pos] = NULL;
    return buffer;
}

int launch(char** args) {
    int status;
    int pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed\n");
    } else if (pid == 0) {
        if (execvp(args[0], args) == -1) {
            fprintf(stderr, "exec failed\n");
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
    if (args[0] == NULL) {
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
        printf("$ ");
        cmd = getcmd();
        args = parse(cmd);
        status = execute(args);

        free(cmd);
        free(args);
    } while (status);
}

int main(int argc, char* argv[]) {
    loop();
    return EXIT_SUCCESS;
}
