// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/time.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/bindings/lua.hpp"
#include "autonomaus/bindings/mapp.hpp"
#include "autonomaus/bindings/state.hpp"
#include "autonomaus/bindings/mashina.hpp"

static int AUTONOMAUS_KEY = 0;

static int panic(lua_State* L)
{
	std::string m = lua_tostring(L, -1);
	autonomaus::Log::warn("Lua panic: {}", m);

	throw std::runtime_error(m);
}

lua_State* autonomaus::lua::create_state(Autonomaus& autonomaus)
{
	lua_State* L = luaL_newstate();
	lua_atpanic(L, &panic);

	if (L != nullptr)
	{
		lua_pushlightuserdata(L, &AUTONOMAUS_KEY);
		lua_pushlightuserdata(L, &autonomaus);
		lua_rawset(L, LUA_REGISTRYINDEX);

		luaL_openlibs(L);

		sol::state_view S(L);
		sol::table A(L, sol::create);
		A["Game"] = lua::register_mapp(L, autonomaus);
		A["Cortex"] = lua::register_state(L, autonomaus);
		A["System"] = sol::table(L, sol::create);
		A["System"]["time"] = get_current_time;

		S["A"] = A;
		S["B"] = lua::register_mashina(L, autonomaus);
	}
	return L;
}

autonomaus::Autonomaus& autonomaus::lua::get_autonomaus(lua_State* L)
{
	lua_pushlightuserdata(L, &AUTONOMAUS_KEY);
	lua_rawget(L, LUA_REGISTRYINDEX);

	auto autonomaus = (Autonomaus*)lua_touserdata(L, -1);
	lua_pop(L, 1);

	return *autonomaus;
}
