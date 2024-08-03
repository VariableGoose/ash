#include "arkin_core.h"
#include "arkin_log.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <stdlib.h>

typedef struct Context Context;
struct Context {
    char cwd[1024];
};

void prompt(Context ctx) {
    printf("%s > ", ctx.cwd);
}

void sighandler(I32 sig) {
    (void) sig;
}

I32 main(void) {
    Context ctx = {0};

    getcwd(ctx.cwd, 1024);

    // Intercept Ctrl-C signal as to not quit the shell when canceling a
    // command.
    struct sigaction action = {
        .sa_handler = sighandler,
    };
    sigaction(SIGINT, &action, NULL);

    char cmd_buffer[1024] = {0};

    if (isatty(STDIN_FILENO)) {
        while (true) {
            prompt(ctx);

            if (fgets(cmd_buffer, 1024, stdin) == NULL) {
                if (feof(stdin)) {
                    exit(EXIT_SUCCESS);
                }
                // SIGINT was sent.
                else {
                    printf("\n");
                    cmd_buffer[0] = 0;
                }
            }

            ArStr cmd_str = ar_str_cstr(cmd_buffer);
            cmd_str = ar_str_trim(cmd_str);
            ar_debug("'%.*s'", (I32) cmd_str.len, cmd_str.data);
        }
    } else {
        ar_info("Non-interactive mode.");
    }

    return EXIT_SUCCESS;
}
