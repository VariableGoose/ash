#include "parser.h"
#include "arkin_core.h"
#include "arkin_log.h"

typedef struct Parser Parser;
struct Parser {
    ArStr cmd;
    U64 i;
};

U8 parser_peek(Parser parser, U64 ahead) {
    return parser.cmd.data[parser.i + ahead];
}

U8 parser_next(Parser *parser) {
    return parser->cmd.data[parser->i++];
}

void parser_skip(Parser *parser, U64 ahead) {
    parser->i += ahead;
}

B8 parser_valid(Parser parser) {
    return parser.i < parser.cmd.len;
}

CmdTable parse_cmd(ArArena *arena, ArStr cmd_str) {
    ArTemp scratch = ar_scratch_get(&arena, 1);

    Parser parser = {
        .cmd = cmd_str,
    };
    ArStrList args = AR_STR_LIST_INIT;
    while (parser_valid(parser)) {
        while (ar_char_is_whitespace(parser_peek(parser, 0)) && parser_valid(parser)) {
            parser_skip(&parser, 1);
        }

        U64 start = parser.i;
        if (parser_peek(parser, 0) == '"') {
            parser_skip(&parser, 1);
            while (parser_next(&parser) != '"' && parser_valid(parser));
        } else {
            while (!ar_char_is_whitespace(parser_next(&parser)) && parser_valid(parser));
        }
        U64 end = parser.i;
        ArStr arg = ar_str_sub(cmd_str, start, end);
        ar_str_list_push(scratch.arena, &args, arg);
    }

    // for (ArStrListNode *arg = args.first; arg != NULL; arg = arg->next) {
    //     ar_debug("%.*s", (I32) arg->str.len, arg->str.data);
    // }
}
