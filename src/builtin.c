#include <unistd.h>

#include "builtin.h"
#include "constants.h"
#include "global.h"


char* builtin_str[] = {
    "cd",
    "exit"
};

int (*builtin_func[])(char**) = {
    cd,
    quit
};

int num_builtins(void) {
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
