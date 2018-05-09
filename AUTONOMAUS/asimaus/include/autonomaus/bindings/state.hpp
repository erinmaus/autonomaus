// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_BINDINGS_LUA_STATE_HPP
#define AUTONOMAUS_BINDINGS_LUA_STATE_HPP

#include "autonomaus/bindings/lua.hpp"
#include "autonomaus/state/memory/message.hpp"

namespace autonomaus { namespace lua
{
	sol::table register_state(lua_State* L, Autonomaus& autonomaus);
	sol::table to_lua(lua_State* L, const Message& message);
} }

#endif
