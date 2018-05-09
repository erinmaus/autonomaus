// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_BINDINGS_LUA_MAPP_HPP
#define AUTONOMAUS_BINDINGS_LUA_MAPP_HPP

#include "autonomaus/bindings/lua.hpp"
#include "twoflower/brochure.hpp"

namespace autonomaus { namespace lua
{
	sol::table register_mapp(lua_State* L, Autonomaus& autonomaus);

	sol::object to_lua(lua_State* L, const twoflower::Resource& resource);
	sol::object to_lua(lua_State* L, const twoflower::Action& action);
	sol::object to_lua(lua_State* L, const twoflower::Requirement& requirement);

	twoflower::Resource resource_from_lua(lua_State* L, int index);
} }

#endif
