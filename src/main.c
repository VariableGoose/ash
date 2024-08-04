#include "arkin_core.h"
#include "arkin_log.h"
#include "lexer.h"
#include "parser.h"

#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

typedef struct Context Context;
struct Context {
    char cwd[1024];
    ArStrList paths;
};

Context context_init(ArArena *arena) {
    Context ctx = {0};

    getcwd(ctx.cwd, ar_arrlen(ctx.cwd));

    return ctx;
}

void prompt(Context ctx) {
    printf("%s > ", ctx.cwd);
}

void sighandler(I32 sig) {
    (void) sig;
}

typedef enum {
    INPUT_TYPE_STR,
    INPUT_TYPE_EOF,
    INPUT_TYPE_SIGINT,
} InputType;

InputType get_cmd_str(char *buffer, U32 buffer_length) {
    if (fgets(buffer, buffer_length, stdin) == NULL) {
        if (feof(stdin)) {
            return INPUT_TYPE_EOF;
        } else {
            return INPUT_TYPE_SIGINT;
        }
    }

    return INPUT_TYPE_STR;
}

I32 main(I32 argc, char *argv[]) {
    // NOTE: If the shell finds itself in an infinite loop, press Ctrl+\ to
    // send a SIGQUIT signal to the shell itself.
    arkin_init(&(ArkinCoreDesc) {
            .error.callback = ar_log_error_callback,
        });
    ArArena *arena = ar_arena_create_default();

    Context ctx = context_init(arena);

    // Intercept Ctrl-C signal as to not quit the shell when canceling a
    // command. sa_handler cannot be set to SIG_IGN because that doesn't
    // make fgets return.
    struct sigaction action = {
        .sa_handler = sighandler,
    };
    sigaction(SIGINT, &action, NULL);

    char cmd_buffer[1024] = {0};

    if (isatty(STDIN_FILENO)) {
        while (true) {
            prompt(ctx);

            InputType input_type = get_cmd_str(cmd_buffer, ar_arrlen(cmd_buffer));
            switch (input_type) {
                case INPUT_TYPE_STR: break;
                case INPUT_TYPE_EOF: exit(0);
                case INPUT_TYPE_SIGINT:
                    printf("\n");
                    continue;
            }

            ArStr cmd_str = ar_str_cstr(cmd_buffer);
            cmd_str = ar_str_trim(cmd_str);

            TokenList tokens = lex(arena, cmd_str);
            for (Token *tok = tokens.first; tok != NULL; tok = tok->next) {
                ar_info("%d: %.*s", tok->type, (I32) tok->part.len, tok->part.data);
            }
        }
    } else {
        ar_info("Non-interactive mode.");
    }

    arkin_terminate();
    return 0;
}
