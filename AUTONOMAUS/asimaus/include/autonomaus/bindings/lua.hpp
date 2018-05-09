// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_BINDINGS_LUA_HPP
#define AUTONOMAUS_BINDINGS_LUA_HPP

#include <lua.hpp>
#include "autonomaus/core/autonomaus.hpp"

#define SOL_SAFE_USERTYPE
#define SOL_SAFE_FUNCTION
#define SOL_CHECK_ARGUMENTS
#include "autonomaus/deps/sol2/sol.hpp"

namespace autonomaus { namespace lua
{
	lua_State* create_state(Autonomaus& autonomaus);
	Autonomaus& get_autonomaus(lua_State* L);
} }

#endif
