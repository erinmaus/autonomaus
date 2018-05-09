// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <set>
#include <string>
#include "autonomaus/core/log.hpp"
#include "autonomaus/bindings/mashina.hpp"
#include "autonomaus/mashina/mashina.hpp"
#include "autonomaus/state/memory/mashinaCortex.hpp"

namespace bmashina
{
	template <typename M>
	struct ToString<M, sol::object>
	{
		inline static typename String<M>::Type get(M& mashina, const sol::object& value)
		{
			lua_State* L = value.lua_state();
			if (L != nullptr && value.valid())
			{
				int count = value.push();
				auto value = lua_tostring(L, -1);

				std::string result;
				if (value == nullptr)
				{
					result = lua_typename(L, lua_type(L, -1));
				}
				else
				{
					result = value;
				}

				lua_pop(L, count);
				return result;
			}

			return "(empty Lua value)";
		}
	};
}

class StringTable
{
public:
	StringTable() = default;
	~StringTable() = default;

	const char* get(const std::string& value);

private:
	std::unordered_set<std::string> strings;
};

const char* StringTable::get(const std::string& value)
{
	this->strings.emplace(value);
	return this->strings.find(value)->c_str();
}

static StringTable* get_string_table(lua_State* L)
{
	static int TAG = 0;

	lua_pushlightuserdata(L, &TAG);
	lua_rawget(L, LUA_REGISTRYINDEX);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);
		lua_pushlightuserdata(L, &TAG);

		sol::stack::push(L, StringTable());
		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L, &TAG);
		lua_rawget(L, LUA_REGISTRYINDEX);
	}

	StringTable& table = sol::stack::get<StringTable>(L, -1);
	lua_pop(L, 1);

	return &table;
}

typedef autonomaus::mashina::Reference<sol::object> LuaReference;
static std::shared_ptr<LuaReference> create_reference(const std::string& name, sol::this_state S)
{
	lua_State* L = S;
	auto string_table = get_string_table(L);

	auto reference = std::make_shared<LuaReference>(string_table->get(name));
	return reference;
}

static bool is_reference(sol::object object)
{
	return object.is<LuaReference>();
}

typedef autonomaus::mashina::Local<sol::object> LuaLocal;
struct LuaLocalProxy {};
static std::shared_ptr<LuaLocalProxy> create_local()
{
	return std::make_shared<LuaLocalProxy>();
}

static bool is_local(sol::object object)
{
	return object.is<LuaLocal>() || object.is<LuaLocalProxy>();
}

static sol::object get_name(sol::object object, sol::this_state S)
{
	lua_State* L = S;
	if (object.is<LuaLocal*>())
	{
		return sol::make_object(L, object.as<LuaLocal*>()->name);
	}
	else if (object.is<LuaReference*>())
	{
		return sol::make_object(L, object.as<LuaReference*>()->name);
	}
	else
	{
		return sol::nil;
	}
}

namespace autonomaus { namespace mashina
{
	class LuaProxyNode : public Node
	{
	public:
		LuaProxyNode(lua_State* L, int index);

		Status update(Executor& executor) override;

		static void public_deactivate(LuaProxyNode* node, Executor& executor);

	protected:
		void activated(Executor& executor) override;
		void deactivated(Executor& executor) override;

	private:
		sol::object call(Executor& executor, const std::string& method);
		void event(Executor& executor, const std::string& method);

		lua_State* L;
		sol::table instance;
		std::map<std::string, LuaLocal> locals;
	};
} }

autonomaus::mashina::LuaProxyNode::LuaProxyNode(lua_State* L, int index) :
	L(L), instance(L, sol::create)
{
	sol::table definition(L, index);
	std::string name = definition["name"];

	for (auto& i: definition)
	{
		if (i.second.is<LuaLocalProxy>())
		{
			auto string_table = get_string_table(L);
			std::string local_name = name + "::" + i.first.as<std::string>();
			instance[i.first] = std::make_shared<LuaLocal>(string_table->get(local_name));
		}
		else
		{
			instance[i.first] = i.second;
		}
	}

	instance["deactivate"] = [this](sol::object self, mashina::Executor& executor)
	{
		this->deactivate(executor);
	};

	instance["node"] = (Node*)this;

	if (instance["new"] != sol::nil)
	{
		instance["new"](instance);
	}
}

autonomaus::mashina::Status
autonomaus::mashina::LuaProxyNode::update(Executor& executor)
{
	auto result = call(executor, "update");
	if (result == sol::nil || !result.is<int>())
	{
		return autonomaus::mashina::Status::failure;
	}
	else
	{
		auto value = (Status)result.as<int>();
		switch (value)
		{
			case Status::working:
			case Status::success:
			case Status::failure:
				return value;
			default:
				return Status::failure;
		}
	}
}

void autonomaus::mashina::LuaProxyNode::activated(Executor& executor)
{
	event(executor, "activated");
}

void autonomaus::mashina::LuaProxyNode::deactivated(Executor& executor)
{
	event(executor, "deactivated");
}

sol::object autonomaus::mashina::LuaProxyNode::call(
	Executor& executor,
	const std::string& method)
{
	instance["tree"] = &tree();

	if (instance[method] != sol::nil)
	{
		sol::protected_function m = instance[method];
		auto result = m(instance, &executor.mashina(), &executor.state(), &executor);
		if (!result.valid())
		{
			sol::error e = result;
			Log::warn("(Mashina) Error executing method {} on node: {}", method, e.what());

			auto L = instance.lua_state();
			return sol::make_object(L, (int)mashina::Status::failure);
		}

		return result;
	}

	return sol::make_object(L, (int)mashina::Status::failure);
}

void autonomaus::mashina::LuaProxyNode::event(
	Executor& executor,
	const std::string& method)
{
	instance["tree"] = &tree();

	if (instance[method] != sol::nil)
	{
		sol::protected_function m = instance[method];
		auto result = m(instance, &executor.mashina(), &executor.state(), &executor);
		if (!result.valid())
		{
			sol::error e = result;
			Log::warn("(Mashina) Error executing method {} on node: {}", method, e.what());
		}
	}
}

std::shared_ptr<autonomaus::mashina::Tree> tree_create(sol::this_state S)
{
	lua_State* L = S;
	return std::make_shared<autonomaus::mashina::Tree>(autonomaus::lua::get_autonomaus(L));
}

decltype(auto) tree_children(
	autonomaus::mashina::Tree* tree,
	autonomaus::mashina::Node* parent)
{
	std::vector<autonomaus::mashina::Node*> children;
	for (auto i = tree->children_begin(*parent); i != tree->children_end(*parent); ++i)
	{
		auto& node = *i;
		children.push_back(&node);
	}

	return sol::as_returns(std::move(children));
}

LuaLocal* tree_constant(autonomaus::mashina::Tree* tree, sol::this_state S)
{
	lua_State* L = S;
	sol::object object(L, 2);
	return const_cast<LuaLocal*>(&tree->constant<sol::object>(object));
}

LuaLocal* tree_local(autonomaus::mashina::Tree* tree, const std::string& key, sol::this_state S)
{
	lua_State* L = S;
	auto table = get_string_table(L);
	return const_cast<LuaLocal*>(&tree->local<sol::object>(table->get(key)));
}

sol::object tree_child(autonomaus::mashina::Tree* tree, sol::object p, sol::this_state S)
{
	lua_State* L = S;
	autonomaus::mashina::Node* node;
	if (tree->empty())
	{
		node = &tree->root<autonomaus::mashina::LuaProxyNode>(L, 2);
	}
	else
	{
		auto parent = p.as<autonomaus::mashina::Node*>();
		if (!tree->has(*parent))
		{
			return sol::nil;
		}

		node = &tree->child<autonomaus::mashina::LuaProxyNode>(*parent, L, 3);
	}

	return sol::make_object<autonomaus::mashina::Node*>(L, node);
}

autonomaus::mashina::Node* tree_root(autonomaus::mashina::Tree* tree)
{
	return &tree->root();
}

bool tree_empty(autonomaus::mashina::Tree* tree)
{
	return tree->empty();
}

static const bmashina::detail::BaseReference* to_reference(sol::object o)
{
	if (o.is<LuaLocal*>())
	{
		return o.as<LuaLocal*>();
	}
	else if (o.is<LuaReference*>())
	{
		return o.as<LuaReference*>();
	}

	return nullptr;
}

static bool tree_input(
	autonomaus::mashina::Tree* tree,
	autonomaus::mashina::Node* node,
	sol::object f,
	sol::object t)
{
	if (!tree->has(*node))
	{
		return false;
	}

	const bmashina::detail::BaseReference* from = to_reference(f);
	const bmashina::detail::BaseReference* to = to_reference(t);

	if (from == nullptr || to == nullptr)
	{
		return false;
	}

	tree->input(*node, *from, *to);

	return true;
}

static bool tree_output(
	autonomaus::mashina::Tree* tree,
	autonomaus::mashina::Node* node,
	sol::object f,
	sol::object t)
{
	if (!tree->has(*node))
	{
		return false;
	}

	const bmashina::detail::BaseReference* from = to_reference(f);
	const bmashina::detail::BaseReference* to = to_reference(t);

	if (from == nullptr || to == nullptr)
	{
		return false;
	}

	tree->output(*node, *from, *to);

	return true;
}

static int tree_execute(
	autonomaus::mashina::Tree* tree,
	autonomaus::mashina::Executor* executor)
{
	return (int)tree->execute(*executor);
}

std::shared_ptr<autonomaus::mashina::Executor> executor_create(sol::this_state S)
{
	lua_State* L = S;
	return std::make_shared<autonomaus::mashina::Executor>(autonomaus::lua::get_autonomaus(L));
}

static void executor_visit(
	autonomaus::mashina::Executor* executor,
	autonomaus::mashina::Node* node)
{
	executor->visit(*node);
}

static void executor_drop(autonomaus::mashina::Executor* executor)
{
	executor->drop();
}

static int executor_update(
	autonomaus::mashina::Executor* executor,
	autonomaus::mashina::Node* node)
{
	return (int)executor->update(*node);
}

static autonomaus::mashina::State* executor_state(
	autonomaus::mashina::Executor* executor)
{
	return &executor->state();
}

std::shared_ptr<autonomaus::mashina::State> state_create(sol::this_state S)
{
	lua_State* L = S;
	return std::make_shared<autonomaus::mashina::State>(autonomaus::lua::get_autonomaus(L));
}

sol::object state_index(autonomaus::mashina::State* state, sol::object key, sol::this_state S)
{
	lua_State* L = S;
	if (key.is<LuaLocal*>())
	{
		auto reference = key.as<LuaLocal*>();
		if (state->has(*reference))
		{
			auto value = state->get(*reference);
			if (value.lua_state() == L)
			{
				return value;
			}
		}
	}
	else if (key.is<LuaReference*>())
	{
		auto reference = key.as<LuaReference*>();
		if (state->has(*reference))
		{
			auto value = state->get(*reference);
			if (value.lua_state() == L)
			{
				return value;
			}
		}
	}

	return sol::nil;
}

void state_newindex(autonomaus::mashina::State* state, sol::object key, sol::object value, sol::this_state S)
{
	if (key.is<LuaLocal*>())
	{
		auto reference = key.as<LuaLocal*>();
		if (value == sol::nil)
		{
			state->unset(*reference);
		}
		else
		{
			state->set(*reference, value);
		}
	}
	else if (key.is<LuaReference*>())
	{
		auto reference = key.as<LuaReference*>();
		if (value == sol::nil)
		{
			state->unset(*reference);
		}
		else
		{
			state->set(*reference, value);
		}
	}
}

static void register_task(const std::string& name, sol::this_state S)
{
	lua_State* L = S;
	if (lua_type(L, 2) == LUA_TTABLE)
	{
		if (!luaL_getmetafield(L, 2, "__call"))
		{
			luaL_error(L, "expected callable object for argument #2");
		}
		else
		{
			lua_pop(L, 1);
		}
	}
	else
	{
		luaL_checktype(L, 2, LUA_TFUNCTION);
	}

	sol::function r(L, 2);
	auto& autonomaus = autonomaus::lua::get_autonomaus(L);
	auto mashina = autonomaus.get_userdata<autonomaus::MashinaCortex>();

	if (mashina == nullptr)
	{
		luaL_error(L, "Mashina cortex not valid. Bad logic!");
	}

	mashina->register_task(name, r);
}

sol::table autonomaus::lua::register_mashina(lua_State* L, Autonomaus& autonomaus)
{
	sol::table result(L, sol::create);
	result["Reference"] = create_reference;
	result["is_reference"] = is_reference;
	result["Local"] = create_local;
	result["is_local"] = is_local;
	result["get_wire_name"] = get_name;
	result["Task"] = register_task;
	result["Status"] = sol::table(L, sol::create);
	result["Status"]["Success"] = (int)mashina::Status::success;
	result["Status"]["Failure"] = (int)mashina::Status::failure;
	result["Status"]["Working"] = (int)mashina::Status::working;

	result.new_usertype<autonomaus::mashina::Tree>(
		"Tree",
		sol::call_constructor, &tree_create,
		"children", &tree_children,
		"constant", &tree_constant,
		"tlocal", &tree_local,
		"child", &tree_child,
		"root", &tree_root,
		"empty", &tree_empty,
		"input", &tree_input,
		"output", &tree_output,
		"execute", &tree_execute);

	result.new_usertype<autonomaus::mashina::Executor>(
		"Executor",
		sol::call_constructor, &executor_create,
		"visit", &executor_visit,
		"drop", &executor_drop,
		"update", &executor_update,
		"state", &executor_state);

	result.new_usertype<autonomaus::mashina::State>(
		"State",
		sol::call_constructor, &state_create,
		sol::meta_function::index, &state_index,
		sol::meta_function::new_index, &state_newindex);

	return result;
}
