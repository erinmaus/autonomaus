// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <unordered_set>
#include "autonomaus/bindings/mapp.hpp"
#include "autonomaus/core/log.hpp"
#include "mapp/atlas/atlas.hpp"
#include "mapp/game/game.hpp"
#include "twoflower/brochure.hpp"
#include "twoflower/plan.hpp"
#include "twoflower/tourist.hpp"
#include "twoflower/wizard.hpp"

struct Cache
{
	Cache() = default;
	Cache(const Cache& other) = delete;
	Cache& operator =(const Cache& other) = delete;

	std::unordered_map<int, std::pair<twoflower::Resource, sol::table>> resources;
	std::unordered_map<int, sol::table> resource_actions;
	std::unordered_map<int, std::pair<twoflower::Action, sol::table>> actions;
	std::unordered_map<int, sol::table> requirements;
};

static Cache& cache_from_lua(lua_State* L)
{
	sol::state_view S(L);

	static int TAG = 0;
	lua_pushlightuserdata(L, &TAG);
	lua_rawget(L, LUA_REGISTRYINDEX);

	if (lua_isnil(L, -1))
	{
		lua_pop(L, 1);

		lua_pushlightuserdata(L, &TAG);

		std::shared_ptr<Cache> cache = std::make_shared<Cache>();
		sol::stack::push(L, cache);

		lua_rawset(L, LUA_REGISTRYINDEX);

		lua_pushlightuserdata(L, &TAG);
		lua_rawget(L, LUA_REGISTRYINDEX);
	}

	Cache& cache = *sol::stack::get<Cache*>(L, -1);
	lua_pop(L, 1);

	return cache;
}

static twoflower::Brochure& brochure_from_lua(lua_State* L)
{
	auto& autonomaus = autonomaus::lua::get_autonomaus(L);
	auto result = autonomaus.get_userdata<twoflower::Brochure>();
	if (!result)
	{
		throw std::runtime_error("twoflower::Brochure instance missing");
	}

	return *result;
}

static mapp::Game& game_from_lua(lua_State* L)
{
	auto& autonomaus = autonomaus::lua::get_autonomaus(L);
	auto result = autonomaus.get_userdata<mapp::Game>();
	if (!result)
	{
		throw std::runtime_error("mapp::Game instance missing");
	}

	return *result;
}

static mapp::Atlas& atlas_from_lua(lua_State* L)
{
	auto& autonomaus = autonomaus::lua::get_autonomaus(L);
	auto result = autonomaus.get_userdata<mapp::Atlas>();
	if (!result)
	{
		throw std::runtime_error("mapp::Atlas instance missing");
	}

	return *result;
}

namespace twoflower
{
	class LuaProxyWizard : public Wizard
	{
	public:
		LuaProxyWizard(lua_State* L, int index);

		float cost(const Action& action, float quantity) const override;
		float distance(
			const Resource& from,
			const Resource& to,
			const Action& action) const override;
		void neighbors(
			const Resource& resource,
			std::vector<std::pair<Action, Resource>>& neighbors) const override;

		int steps(const Action& action, float quantity) const override;
		float reduce(const Action& action, float quantity) const override;
		bool has(const Resource& resource, float quantity) const override;
		float count(const Resource& resource) const override;

	private:
		lua_State* L;
		sol::table instance;
	};
}

twoflower::LuaProxyWizard::LuaProxyWizard(lua_State* L, int index) :
	L(L), instance(L, index)
{
	luaL_checktype(L, index, LUA_TTABLE);
}

float twoflower::LuaProxyWizard::cost(const Action& action, float quantity) const
{
	if (instance["cost"] != sol::nil)
	{
		float value = instance["cost"](
			instance,
			autonomaus::lua::to_lua(L, action),
			quantity);
		return value;
	}

	return HUGE_VALF;
}

float twoflower::LuaProxyWizard::distance(
	const Resource& from,
	const Resource& to,
	const Action& action) const
{
	if (instance["distance"] != sol::nil)
	{
		float value = instance["distance"](
			instance,
			autonomaus::lua::to_lua(L, from),
			autonomaus::lua::to_lua(L, to),
			autonomaus::lua::to_lua(L, action));
		return value;
	}

	return HUGE_VALF;
}

void twoflower::LuaProxyWizard::neighbors(
	const Resource& resource,
	std::vector<std::pair<Action, Resource>>& neighbors) const
{
	if (instance["neighbors"] != sol::nil)
	{
		sol::table value = instance["neighbors"](
			instance,
			autonomaus::lua::to_lua(L, resource));

		int count = value.size();

		auto& cache = cache_from_lua(L);
		auto resources = brochure_from_lua(L).resources();
		auto actions = brochure_from_lua(L).actions();
		for (int i = 1; i <= count; ++i)
		{
			sol::table neighbor = value[i];
			int action_id = neighbor[1]["id"];
			int resource_id = neighbor[2]["id"];

			twoflower::Action action;
			{
				auto action_iter = cache.actions.find(action_id);
				if (action_iter != cache.actions.end())
				{
					action = action_iter->second.first;
				}
				else if (actions.has(action_id))
				{
					action = actions.get(action_id);
				}
			}

			twoflower::Resource resource;
			{
				auto resource_iter = cache.resources.find(resource_id);
				if (resource_iter != cache.resources.end())
				{
					resource = resource_iter->second.first;
				}
				else if (resources.has(resource_id))
				{
					resource = resources.get(resource_id);
				}
			}

			if (action.get_id() && resource.get_id())
			{
				neighbors.emplace_back(action, resource);
			}
		}
	}
}

int twoflower::LuaProxyWizard::steps(const Action& action, float quantity) const
{
	if (instance["steps"] != sol::nil)
	{
		int value = instance["steps"](
			instance,
			autonomaus::lua::to_lua(L, action),
			quantity);
		return value;
	}

	return 1;
}

float twoflower::LuaProxyWizard::reduce(const Action& action, float quantity) const
{
	if (instance["reduce"] != sol::nil)
	{
		float value = instance["reduce"](
			instance,
			autonomaus::lua::to_lua(L, action),
			quantity);
		return value;
	}

	return quantity;
}

bool twoflower::LuaProxyWizard::has(const Resource& resource, float quantity) const
{
	if (instance["has"] != sol::nil)
	{
		bool value = instance["has"](
			instance,
			autonomaus::lua::to_lua(L, resource),
			quantity);
		return value;
	}

	return false;
}

float twoflower::LuaProxyWizard::count(const Resource& resource) const
{
	if (instance["count"] != sol::nil)
	{
		float value = instance["count"](
			instance,
			autonomaus::lua::to_lua(L, resource));
		return value;
	}

	return 0;
}

std::shared_ptr<twoflower::Wizard> create_wizard(sol::this_state S)
{
	lua_State* L = S;
	return std::make_shared<twoflower::LuaProxyWizard>(L, 1);
}

static sol::object get_resources(const std::string& name, const std::string& type, sol::this_state S)
{
	lua_State* L = S;
	auto& brochure = brochure_from_lua(L);
	auto resources = brochure.resources();

	sol::table result(L, sol::create);
	int index = 1;
	for (auto i = resources.by_name(name); i != resources.end(); ++i)
	{
		if (type.empty() || i->get_type() == type)
		{
			result[index++] = autonomaus::lua::to_lua(L, *i);
		}
	}

	return result;
}

static sol::object get_actions(const std::string& type, const std::string& name, sol::this_state S)
{
	lua_State* L = S;
	auto& brochure = brochure_from_lua(L);
	auto actions = brochure.actions();

	sol::table result(L, sol::create);
	int index = 1;
	for (auto i = actions.by_type(type, name); i != actions.end(); ++i)
	{
		result[index++] = autonomaus::lua::to_lua(L, *i);
	}

	return result;
}

static sol::object atlas_find(const std::string& address, sol::this_state S)
{
	lua_State* L = S;
	auto& atlas = atlas_from_lua(L);
	auto locations = atlas.find(address);

	sol::table result(L, sol::create);
	int index = 1;
	for (auto& location: locations)
	{
		result[index++] = autonomaus::lua::to_lua(L, location.resource);;
	}

	return result;
}

static sol::object atlas_locate(sol::this_state S)
{
	lua_State* L = S;

	thoreau::TileCoordinate tile_coordinate;
	if (sol::stack::check<sol::table>(L, 1))
	{
		auto t = sol::stack::get<sol::table>(L, 1);
		int x = t["x"];
		int y = t["y"];
		int layer = t["layer"];
		tile_coordinate = thoreau::tile_from_input(x, y, layer);
	}
	else
	{
		int x = sol::stack::get<int>(L, 1);
		int y = sol::stack::get<int>(L, 2);
		int layer = sol::stack::get<int>(L, 3);
		tile_coordinate = thoreau::tile_from_input(x, y, layer);
	}

	auto& atlas = atlas_from_lua(L);
	mapp::Location location;
	if (atlas.find(tile_coordinate, location))
	{
		return autonomaus::lua::to_lua(L, location.resource);
	}

	return sol::nil;
}

static sol::object plan_visit(sol::table f, sol::table t, const twoflower::Wizard& wizard, sol::this_state S)
{
	lua_State* L = S;
	auto& brochure = brochure_from_lua(L);
	auto resources = brochure.resources();

	int from_id = f["id"];
	int to_id = t["id"];

	if (!resources.has(from_id) || !resources.has(to_id))
	{
		return sol::nil;
	}

	auto from = resources.get(from_id);
	auto to = resources.get(to_id);

	auto plan = twoflower::visit(brochure, from, to, wizard);
	if (!plan)
	{
		return sol::nil;
	}

	sol::table result(L, sol::create);
	int index = 1;
	for (auto& bullet: plan)
	{
		sol::table r(L, sol::create);

		auto action = std::get<twoflower::Plan::ACTION>(bullet);
		r["action"] = autonomaus::lua::to_lua(L, action);

		auto count = std::get<twoflower::Plan::COUNT>(bullet);
		r["count"] = count;

		auto cost = std::get<twoflower::Plan::COST>(bullet);
		r["cost"] = cost;

		result[index++] = r;
	}

	return result;
}

static sol::object plan_goal(sol::table a, int count, sol::object step, sol::object max_depth, const twoflower::Wizard& wizard, sol::this_state S)
{
	lua_State* L = S;
	auto& brochure = brochure_from_lua(L);
	auto resources = brochure.resources();

	int id = a["id"];

	if (!resources.has(id))
	{
		return sol::nil;
	}

	auto resource = resources.get(id);

	bool s = false;
	if (step != sol::nil)
	{
		s = step.as<bool>();
	}

	int d = twoflower::PLAN_DEFAULT_DEPTH;
	if (max_depth != sol::nil)
	{
		d = max_depth.as<int>();
	}

	auto plan = twoflower::goal(brochure, resource, count, wizard, s, d);
	if (!plan)
	{
		return sol::nil;
	}

	sol::table result(L, sol::create);
	int index = 1;
	for (auto& bullet: plan)
	{
		sol::table r(L, sol::create);

		auto action = std::get<twoflower::Plan::ACTION>(bullet);
		r["action"] = autonomaus::lua::to_lua(L, action);

		auto count = std::get<twoflower::Plan::COUNT>(bullet);
		r["count"] = count;

		auto cost = std::get<twoflower::Plan::COST>(bullet);
		r["cost"] = cost;

		result[index++] = r;
	}

	return result;
}

sol::table autonomaus::lua::register_mapp(lua_State* L, Autonomaus& autonomaus)
{
	sol::table result(L, sol::create);
	result["Resources"] = sol::table(L, sol::create);
	result["Resources"]["find"] = get_resources;
	result["Actions"] = sol::table(L, sol::create);
	result["Actions"]["find"] = get_actions;
	result["Atlas"] = sol::table(L, sol::create);
	result["Atlas"]["find"] = atlas_find;
	result["Atlas"]["locate"] = atlas_locate;
	result["Wizard"] = create_wizard;
	result["Plan"] = sol::table(L, sol::create);
	result["Plan"]["visit"] = plan_visit;
	result["Plan"]["goal"] = plan_goal;
	return result;
}

sol::object autonomaus::lua::to_lua(lua_State* L, const twoflower::Resource& resource)
{
	auto& cache = cache_from_lua(L);
	{
		auto iter = cache.resources.find(resource.get_id());
		if (iter != cache.resources.end())
		{
			return iter->second.second;
		}
	}

	sol::table table(L, sol::create);
	table["id"] = resource.get_id();
	table["name"] = resource.get_name();
	table["type"] = resource.get_type();

	auto& brochure = brochure_from_lua(L);
	table["actions"] = [&brochure, resource](sol::this_state S)
	{
		lua_State* L = S;

		auto& cache = cache_from_lua(L);
		{
			auto iter = cache.resource_actions.find(resource.get_id());
			if (iter != cache.resource_actions.end())
			{
				return iter->second;
			}
		}

		sol::table result(L, sol::create);
		{
			auto actions = brochure.actions(resource);
			int index = 1;
			for (auto action: actions)
			{
				result[index++] = to_lua(L, action);
			}
		}

		cache.resource_actions[resource.get_id()] = result;
		return result;
	};

	auto& atlas = atlas_from_lua(L);
	auto& game = game_from_lua(L);
	if (resource.get_type() == "location")
	{
		mapp::Location location;
		if (atlas.location(resource, location))
		{
			table["address"] = atlas.address(location);
			table["parent"] = [&atlas, location](sol::this_state S)
			{
				lua_State* L = S;
				return to_lua(L, atlas.parent(location).resource);
			};
			table["children"] = [&atlas, location](sol::this_state S)
			{
				lua_State* L = S;
				sol::table result(L, sol::create);

				auto children = atlas.children(location);
				int index = 1;
				for (auto& child: children)
				{
					result[index++] = to_lua(L, child.resource);
				}

				return result;
			};
			table["inside"] = [&atlas, location](sol::this_state S)
			{
				lua_State* L = S;

				thoreau::TileCoordinate tile_coordinate;
				if (sol::stack::check<sol::table>(L, 1))
				{
					auto t = sol::stack::get<sol::table>(L, 1);
					int x = t["x"];
					int y = t["y"];
					int layer = t["layer"];
					tile_coordinate = thoreau::tile_from_input(x, y, layer);
				}
				else
				{
					int x = sol::stack::get<int>(L, 1);
					int y = sol::stack::get<int>(L, 2);
					int layer = sol::stack::get<int>(L, 3);
					tile_coordinate = thoreau::tile_from_input(x, y, layer);
				}

				return location.shape.inside(tile_coordinate);
			};

			table["anchor"] = sol::table(L, sol::create);
			table["anchor"]["x"] = location.anchor.x;
			table["anchor"]["y"] = location.anchor.y;
			table["anchor"]["layer"] = location.anchor.layer;
		}
	}
	else if (resource.get_type() == "amenity")
	{
		table["parent"] = [&atlas, resource](sol::this_state S)
		{
			lua_State* L = S;
			mapp::Location parent;
			atlas.parent(resource, parent);
			return to_lua(L, parent.resource);
		};
	}
	else if (resource.get_type() == "item")
	{
		mapp::Item item;
		if (game.item(resource, item))
		{
			table["value"] = item.value;
			table["object_id"] = item.object_id;
		}
	}

	cache.resources[resource.get_id()] = std::make_pair(resource, table);
	return table;
}

sol::object autonomaus::lua::to_lua(lua_State* L, const twoflower::Action& action)
{
	auto& cache = cache_from_lua(L);
	if (action.get_id())
	{
		auto iter = cache.actions.find(action.get_id());
		if (iter != cache.actions.end())
		{
			return iter->second.second;
		}
	}

	sol::table table(L, sol::create);
	table["name"] = action.get_name();
	table["type"] = action.get_type();
	table["is_getter"] = action.is_getter();

	if (action.has_cost_multiplier())
	{
		table["cost"] = action.get_cost_multiplier();
	}
	else
	{
		table["cost"] = 1.0f;
	}

	if (!action.get_task().empty())
	{
		table["task"] = action.get_task();
	}

	if (action.get_id())
	{
		table["id"] = action.get_id();

		auto& brochure = brochure_from_lua(L);
		table["resource"] = to_lua(L, brochure.resource(action));

		table["requirements"] = [&brochure, action](sol::this_state S)
		{
			lua_State* L = S;

			auto& cache = cache_from_lua(L);
			{
				auto iter = cache.requirements.find(action.get_id());
				if (iter != cache.requirements.end())
				{
					return iter->second;
				}
			}

			sol::table table(L, sol::create);

			auto requirements = brochure.requirements(action);
			int index = 1;
			for (auto requirement: requirements)
			{
				table[index++] = to_lua(L, requirement);
			}

			cache.requirements[action.get_id()] = table;
			return table;
		};
	}

	if (action.get_id())
	{
		cache.actions[action.get_id()] = std::make_pair(action, table);
	}

	return table;
}

sol::object autonomaus::lua::to_lua(lua_State* L, const twoflower::Requirement& requirement)
{
	sol::table table(L, sol::create);
	table["is_input"] = requirement.is_input();
	table["is_output"] = requirement.is_output();
	table["count"] = requirement.get_count();
	table["action"] = to_lua(L, requirement.get_action());
	table["resource"] = to_lua(L, requirement.get_resource());

	return table;
}
