// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_BINDINGS_LUA_MASHINA_HPP
#define AUTONOMAUS_BINDINGS_LUA_MASHINA_HPP

#include "autonomaus/bindings/lua.hpp"

namespace autonomaus { namespace lua
{
	sol::table register_mashina(lua_State* L, Autonomaus& autonomaus);
} }

#endif
