/*
    Copyright (C) 2019 Kushview, LLC.  All rights reserved.
    
    This program is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    This program is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <limits>

#include "sol/sol.hpp"

#ifdef _MSC_VER
 #pragma warning(disable: 4244) // convert possible data loss
 #pragma warning(disable: 4310) // cast truncates constant value
 #pragma warning(disable: 4334) // result of 32-bit shift implicitly converted to 64 bits (was 64-bit shift intended?)
#endif

#include "../deps/lua-5.3.5/src/lauxlib.c"
#include "../deps/lua-5.3.5/src/liolib.c"
#include "../deps/lua-5.3.5/src/lopcodes.c"
#include "../deps/lua-5.3.5/src/lstate.c"
#include "../deps/lua-5.3.5/src/lobject.c"
#include "../deps/lua-5.3.5/src/lmathlib.c"
#include "../deps/lua-5.3.5/src/loadlib.c"
#include "../deps/lua-5.3.5/src/lvm.c"
#include "../deps/lua-5.3.5/src/lfunc.c"
#include "../deps/lua-5.3.5/src/lstrlib.c"
// #include "../deps/lua-5.3.5/src/lua.c"
#include "../deps/lua-5.3.5/src/linit.c"
#include "../deps/lua-5.3.5/src/lstring.c"
#include "../deps/lua-5.3.5/src/lundump.c"
#include "../deps/lua-5.3.5/src/lctype.c"
#include "../deps/lua-5.3.5/src/ltable.c"
#include "../deps/lua-5.3.5/src/ldump.c"
#include "../deps/lua-5.3.5/src/loslib.c"
#include "../deps/lua-5.3.5/src/lgc.c"
#include "../deps/lua-5.3.5/src/lzio.c"
#include "../deps/lua-5.3.5/src/ldblib.c"
#include "../deps/lua-5.3.5/src/lutf8lib.c"
#include "../deps/lua-5.3.5/src/lmem.c"
#include "../deps/lua-5.3.5/src/lcorolib.c"
#include "../deps/lua-5.3.5/src/lcode.c"
#include "../deps/lua-5.3.5/src/ltablib.c"
#include "../deps/lua-5.3.5/src/lbitlib.c"
#include "../deps/lua-5.3.5/src/lapi.c"
#include "../deps/lua-5.3.5/src/lbaselib.c"
#include "../deps/lua-5.3.5/src/ldebug.c"
#include "../deps/lua-5.3.5/src/lparser.c"
#include "../deps/lua-5.3.5/src/llex.c"
#include "../deps/lua-5.3.5/src/ltm.c"
#include "../deps/lua-5.3.5/src/ldo.c"

// #include "../deps-rt/src/audio.c"
// #include "../deps-rt/src/midi.c"
