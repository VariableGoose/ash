#include "lexer.h"
#include "arkin_core.h"
#include "arkin_log.h"

typedef struct Lexer Lexer;
struct Lexer {
    ArStr cmd;
    U64 i;
};

static U8 lexer_peek(Lexer lexer, U64 ahead) {
    return lexer.cmd.data[lexer.i + ahead];
}

static U8 lexer_next(Lexer *lexer) {
    return lexer->cmd.data[++lexer->i];
}

static void lexer_skip(Lexer *lexer, U64 ahead) {
    lexer->i += ahead;
}

static B8 lexer_valid(Lexer lexer) {
    return lexer.i < lexer.cmd.len;
}

static void push_token(ArArena *arena, TokenList *list, TokenType type, ArStr part) {
    Token *token = ar_arena_push_type_no_zero(arena, Token);
    *token = (Token) {
        .next = NULL,
        .type = type,
        .part = part,
    };
    ar_sll_queue_push(list->first, list->last, token);
}

TokenList lex(ArArena *arena, ArStr cmd) {
    Lexer lexer = {
        .cmd = cmd,
    };

    TokenList list = {0};
    while (lexer_valid(lexer)) {
        char c = lexer_peek(lexer, 0);
        while (ar_char_is_whitespace(c) && lexer_valid(lexer)) {
            c = lexer_next(&lexer);
        }

        U64 start = lexer.i;
        while (!ar_char_is_whitespace(c) && lexer_valid(lexer)) {
            c = lexer_next(&lexer);
        }

        ArStr part = ar_str_sub(cmd, start, lexer.i);
        TokenType type = TOKEN_IDENT;
        if (ar_str_match(part, ar_str_lit("|"), AR_STR_MATCH_FLAG_EXACT)) {
            type = TOKEN_PIPE;
        } else if (ar_str_match(part, ar_str_lit("<"), AR_STR_MATCH_FLAG_EXACT)) {
            type = TOKEN_IN;
        } else if (ar_str_match(part, ar_str_lit(">"), AR_STR_MATCH_FLAG_EXACT)) {
            type = TOKEN_OUT;
        } else if (ar_str_match(part, ar_str_lit("2>"), AR_STR_MATCH_FLAG_EXACT)) {
            type = TOKEN_ERR;
        } else if (ar_str_match(part, ar_str_lit("&>"), AR_STR_MATCH_FLAG_EXACT)) {
            type = TOKEN_OUT_ERR;
        }

        push_token(arena, &list, type, part);
    }

    return list;
}
