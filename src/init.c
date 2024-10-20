#include <stdlib.h>
#include <pwd.h>
#include <limits.h>
#include <unistd.h>

#include "constants.h"
#include "init.h"

#define DECLARE_GLOBALS
#include "global.h"

char* getusername(void) {
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
