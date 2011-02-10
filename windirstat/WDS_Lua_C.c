#include "WDS_Lua_C.h"
#define luaall_c

// Core
#include "lapi.c"
#include "lcode.c"
#include "ldebug.c"
#include "ldo.c"
#include "ldump.c"
#include "lfunc.c"
#include "lgc.c"
#include "llex.c"
#include "lmem.c"
#include "lobject.c"
#include "lopcodes.c"
#include "lparser.c"
#include "lstate.c"
#include "lstring.c"
#include "ltable.c"
#include "ltm.c"
#include "lundump.c"
#include "lvm.c"
#include "lzio.c"

#include "lauxlib.c"
#include "lbaselib.c"
#include "ldblib.c"
#ifndef WDS_LUA_NO_IOLIB
#include "liolib.c"
#endif // WDS_LUA_NO_IOLIB
#ifndef WDS_LUA_NO_INIT
#include "linit.c"
#endif // WDS_LUA_NO_INIT
#ifndef WDS_LUA_NO_MATHLIB
#include "lmathlib.c"
#endif // WDS_LUA_NOMATH
#ifndef WDS_LUA_NO_LOADLIB
#include "loadlib.c"
#endif // WDS_LUA_NO_LOADLIB
#ifndef WDS_LUA_NO_OSLIB
#include "loslib.c"
#endif // WDS_LUA_NO_OSLIB
#include "lstrlib.c"
#include "ltablib.c"

#include "lua.c"
