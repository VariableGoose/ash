#pragma once

#include "arkin_core.h"

typedef enum {
    TOKEN_IDENT,
    TOKEN_PIPE, // |
    TOKEN_IN, // <
    TOKEN_OUT, // >
    TOKEN_ERR, // 2>
    TOKEN_OUT_ERR, // &>

    TOKEN_COUNT,
} TokenType;

typedef struct Token Token;
struct Token {
    Token *next;
    TokenType type;
    ArStr part;
};

typedef struct TokenList TokenList;
struct TokenList {
    Token *first;
    Token *last;
};

extern TokenList lex(ArArena *arena, ArStr cmd);
