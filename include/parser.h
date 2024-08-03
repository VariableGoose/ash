#pragma once

#include "arkin_core.h"

typedef struct CmdTable CmdTable;
struct CmdTable {
    ArStr program;
    ArStrList args;
};

extern CmdTable parse_cmd(ArArena *arena, ArStr cmd_str);
