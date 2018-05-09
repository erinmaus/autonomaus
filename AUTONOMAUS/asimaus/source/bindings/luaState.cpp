// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/bindings/state.hpp"
#include "autonomaus/state/memory/brain.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/state/memory/message.hpp"

static autonomaus::Message get_message(lua_State* L, int index)
{
	autonomaus::Message message;
	if (lua_istable(L, index))
	{
		lua_pushnil(L);
		while (lua_next(L, index) != 0)
		{
			std::string field;
			switch (lua_type(L, -2))
			{
				case LUA_TNUMBER:
					field = std::to_string(lua_tonumber(L, -2));
					break;
				case LUA_TSTRING:
					field = lua_tostring(L, -2);
					break;
				default:
					lua_pop(L, 1);
					continue;
			}

			std::size_t index = 0;
			switch (lua_type(L, -1))
			{
				case LUA_TNUMBER:
					message.set_single(field, lua_tonumber(L, -1));
					break;
				case LUA_TSTRING:
					message.set_string(field, lua_tostring(L, -1));
					break;
				case LUA_TBOOLEAN:
					message.set_boolean(field, lua_toboolean(L, -1));
					break;
				case LUA_TTABLE:
					lua_pushnumber(L, 1);
					lua_rawget(L, -2);
					while (!lua_isnil(L, -1))
					{
						switch (lua_type(L, -1))
						{
							case LUA_TNUMBER:
								message.set_single(field, lua_tonumber(L, -1), index);
								break;
							case LUA_TSTRING:
								message.set_string(field, lua_tostring(L, -1), index);
								break;
							case LUA_TBOOLEAN:
								message.set_boolean(field, lua_toboolean(L, -1), index);
								break;
						}

						++index;
						lua_pop(L, 1);
						lua_pushnumber(L, (int)index + 1);
						lua_rawget(L, -2);
					}

					lua_pop(L, 1);
					break;
			}

			lua_pop(L, 1);
		}
	}

	return message;
}

static void send_message(lua_State* L, const autonomaus::Message& message)
{
	if (!message.valid())
	{
		lua_pushnil(L);
		return;
	}

	lua_newtable(L);
	for (auto& field: message)
	{
		lua_pushstring(L, field.first.c_str());

		if (field.second.length() > 1)
		{
			lua_newtable(L);

			for (std::size_t i = 0; i < field.second.length(); ++i)
			{
				lua_pushnumber(L, (int)i + 1);

				switch (field.second.get_type())
				{
					case autonomaus::Message::Field::type_single:
					case autonomaus::Message::Field::type_integer:
						lua_pushnumber(L, field.second.get_single(i));
						break;
					case autonomaus::Message::Field::type_string:
						lua_pushstring(L, field.second.get_string(i).c_str());
						break;
					case autonomaus::Message::Field::type_boolean:
						lua_pushboolean(L, field.second.get_boolean(i));
					default:
						lua_pushnil(L);
						break;
				}

				lua_rawset(L, -3);
			}
		}
		else
		{
			switch (field.second.get_type())
			{
				case autonomaus::Message::Field::type_single:
				case autonomaus::Message::Field::type_integer:
					lua_pushnumber(L, field.second.get_single());
					break;
				case autonomaus::Message::Field::type_string:
					lua_pushstring(L, field.second.get_string().c_str());
					break;
				case autonomaus::Message::Field::type_boolean:
					lua_pushboolean(L, field.second.get_boolean());
					break;
				default:
					lua_pushnil(L);
					break;
			}
		}

		lua_rawset(L, -3);
	}
}

sol::table autonomaus::lua::to_lua(lua_State* L, const Message& message)
{
	send_message(L, message);
	sol::table result(L, -1);

	lua_pop(L, 1);

	return result;
}

static int cortex_poke(lua_State* L)
{
	auto cortex = sol::stack::get<autonomaus::Cortex*>(L, 1);
	const char* action;
	if (!lua_isstring(L, 2) || (action = lua_tostring(L, 2)) == nullptr)
	{
		sol::stack::push<autonomaus::Cortex::Result>(L, autonomaus::Cortex::Result::Status::failure);
	}

	sol::stack::push<autonomaus::Cortex::Result>(L, cortex->poke(action, get_message(L, 3)));
	return 1;
}

static int cortex_pull(lua_State* L)
{
	auto cortex = sol::stack::get<autonomaus::Cortex*>(L, 1);
	const char* action;
	if (!lua_isstring(L, 2) || (action = lua_tostring(L, 2)) == nullptr)
	{
		return 0;
	}

	auto result = cortex->pull(action, get_message(L, 3));
	send_message(L, result);

	return 1;
}

static int cortex_get(lua_State* L)
{
	std::string name = luaL_checkstring(L, 1);
	auto& autonomaus = autonomaus::lua::get_autonomaus(L);

	auto brain = autonomaus.get_userdata<autonomaus::Brain>();
	if (brain == nullptr)
	{
		return 0;
	}

	auto cortex = brain->get(name);
	if (cortex == nullptr)
	{
		return 0;
	}

	sol::stack::push<autonomaus::Cortex*>(L, cortex);
	return 1;
}

static int result_pending(lua_State* L)
{
	auto result = sol::stack::get<autonomaus::Cortex::Result>(L, 1);
	lua_pushboolean(L, result.get() == autonomaus::Cortex::Result::Status::working);
	return 1;
}

static int result_succeeded(lua_State* L)
{
	auto result = sol::stack::get<autonomaus::Cortex::Result>(L, 1);
	lua_pushboolean(L, result.get() == autonomaus::Cortex::Result::Status::success);
	return 1;
}

static int result_failed(lua_State* L)
{
	auto result = sol::stack::get<autonomaus::Cortex::Result>(L, 1);
	lua_pushboolean(L, result.get() == autonomaus::Cortex::Result::Status::failure);
	return 1;
}

static int result_cancel(lua_State* L)
{
	auto result = sol::stack::get<autonomaus::Cortex::Result>(L, 1);
	result.cancel();
	return 0;
}

static int result_message(lua_State* L)
{
	auto result = sol::stack::get<autonomaus::Cortex::Result>(L, 1);
	send_message(L, result.message());
	return 1;
}

sol::table autonomaus::lua::register_state(lua_State* L, Autonomaus& autonomaus)
{
	sol::table result(L, sol::create);
	result["poke"] = &cortex_poke;
	result["pull"] = &cortex_pull;
	result["get"] = &cortex_get;

	result["Result"] = sol::table(L, sol::create);
	result["Result"]["pending"] = &result_pending;
	result["Result"]["succeeded"] = &result_succeeded;
	result["Result"]["failed"] = &result_failed;
	result["Result"]["cancel"] = &result_cancel;
	result["Result"]["message"] = &result_message;

	return result;
}
