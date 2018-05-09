// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#include <algorithm>
#include "mapp/game/game.hpp"

mapp::Game::Game(twoflower::Brochure& brochure) :
	brochure(&brochure),
	atlas(brochure)
{
	ensure_action_definition("buy", "ge", true);
	ensure_action_definition("buy", "shop", true);
	ensure_action_definition("loot", "monster", true);
	ensure_action_definition("loot", "object", true);
	ensure_action_definition("loot", "npc", true);
	ensure_action_definition("combat", "attack", true);
	ensure_action_definition("combat", "strength", true);
	ensure_action_definition("combat", "defence", true);
	ensure_action_definition("combat", "ranged", true);
	ensure_action_definition("combat", "magic", true);
	ensure_action_definition("combat", "constitution", true);
	ensure_action_definition("combat", "slayer", true);
	ensure_action_definition("prayer", "altar", true);
	ensure_action_definition("prayer", "bury", true);
	ensure_action_definition("prayer", "default", true);
	ensure_action_definition("crafting", "spinning_wheel", true);
	ensure_action_definition("crafting", "potters_wheel", true);
	ensure_action_definition("crafting", "kiln", true);
	ensure_action_definition("crafting", "needle", true);
	ensure_action_definition("crafting", "furnace", true);
	ensure_action_definition("crafting", "default", true);
	ensure_action_definition("mining", "default", true);
	ensure_action_definition("smithing", "furnace", true);
	ensure_action_definition("smithing", "anvil", true);
	ensure_action_definition("smithing", "default", true);
	ensure_action_definition("fishing", "default", true);
	ensure_action_definition("cooking", "range", true);
	ensure_action_definition("cooking", "default", true);
	ensure_action_definition("woodcutting", "default", true);
	ensure_action_definition("firemaking", "bonfire", true);
	ensure_action_definition("firemaking", "default", true);
	ensure_action_definition("runecrafting", "default", true);
	ensure_action_definition("dungoneering", "default", true);
	ensure_action_definition("agility", "shortcut", true);
	ensure_action_definition("agility", "course", true);
	ensure_action_definition("agility", "default", true);
	ensure_action_definition("herblore", "pestle_and_mortar", true);
	ensure_action_definition("herblore", "clean", true);
	ensure_action_definition("herblore", "potion", true);
	ensure_action_definition("herblore", "default", true);
	ensure_action_definition("thieving", "blackjack", true);
	ensure_action_definition("thieving", "stall", true);
	ensure_action_definition("thieving", "chest", true);
	ensure_action_definition("thieving", "door", true);
	ensure_action_definition("thieving", "pickpocket", true);
	ensure_action_definition("thieving", "default", true);
	ensure_action_definition("fletching", "bow", true);
	ensure_action_definition("fletching", "stringing", true);
	ensure_action_definition("fletching", "arrow", true);
	ensure_action_definition("fletching", "bolt", true);
	ensure_action_definition("slayer", "default", true);
	ensure_action_definition("farming", "allotment", true);
	ensure_action_definition("farming", "hop", true);
	ensure_action_definition("farming", "bush", true);
	ensure_action_definition("farming", "tree", true);
	ensure_action_definition("farming", "fruit_tree", true);
	ensure_action_definition("farming", "cactus", true);
	ensure_action_definition("farming", "mushroom", true);
	ensure_action_definition("farming", "default", true);
	ensure_action_definition("construction", "flatpack", true);
	ensure_action_definition("construction", "default", true);
	ensure_action_definition("hunter", "track", true);
	ensure_action_definition("hunter", "snare", true);
	ensure_action_definition("hunter", "butterfly_net", true);
	ensure_action_definition("hunter", "deadfall", true);
	ensure_action_definition("hunter", "box", true);
	ensure_action_definition("hunter", "net", true);
	ensure_action_definition("hunter", "pitfall", true);
	ensure_action_definition("hunter", "falconry", true);
	ensure_action_definition("hunter", "magic_box", true);
	ensure_action_definition("hunter", "marasamaw", true);
	ensure_action_definition("hunter", "yaktwee", true);
	ensure_action_definition("hunter", "barehand", true);
	ensure_action_definition("hunter", "jar", true);
	ensure_action_definition("hunter", "default", true);
	ensure_action_definition("summoning", "pouch", true);
	ensure_action_definition("summoning", "scroll", true);
	ensure_action_definition("summoning", "default", true);
	ensure_action_definition("divination", "weave", true);
	ensure_action_definition("divination", "harvest", true);
	ensure_action_definition("divination", "default", true);
	ensure_action_definition("ability", "unlock", true);
	ensure_action_definition("ability", "use", true);
	ensure_action_definition("quest", "complete", true);

	ensure_resource_type("item");
	ensure_resource_type("skill");
	ensure_resource_type("currency");
	ensure_resource_type("quest");
	ensure_resource_type("ability");
	ensure_resource_type("shop");
	ensure_resource_type("shop_inventory");
}

mapp::Item mapp::Game::add(const ItemDefinition& item)
{
	Item result;
	result.resource.builder().set_type("item");
	result.resource.builder().set_name(item.name);
	result.resource = brochure->builder().add_resource(result.resource);

	result.value = (item.value < 0) ? 0 : item.value;
	brochure->builder().set_userdata(
		result.resource,
		"mapp-item-value",
		result.value);

	if (item.object_id != Item::NULL_OBJECT_ID)
	{
		brochure->builder().set_userdata(
			result.resource,
			"mapp-item-object-id",
			item.object_id);
	}
	result.object_id = item.object_id;

	return result;
}

void mapp::Game::remove(const Item& item)
{
	auto s = shops(item);
	for (auto& i: s)
	{
		remove(i, item);
	}

	remove(item.resource);
}

mapp::Item mapp::Game::update(const Item& item, const ItemDefinition& definition)
{
	auto resource = item.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);

	int value = (definition.value < 0) ? 0 : item.value;
	brochure->builder().set_userdata(
		resource,
		"mapp-item-value",
		value);

	if (definition.object_id == Item::NULL_OBJECT_ID)
	{
		brochure->builder().unset_userdata(resource, "mapp-item-object-id");
	}
	else
	{
		brochure->builder().set_userdata(
			resource,
			"mapp-item-object-id",
			definition.object_id);
	}

	invalidate(item.resource);

	Item result = item;
	result.object_id = definition.object_id;
	result.value = value;
	return result;
}

bool mapp::Game::item(const twoflower::Resource& resource, Item& result) const
{
	if (resource.get_type() != "item")
	{
		return false;
	}

	int value;
	if (!brochure->get_userdata(resource, "mapp-item-value", value))
	{
		return false;
	}

	if (value < 0)
	{
		return false;
	}

	int object_id;
	brochure->get_userdata(resource, "mapp-item-object-id", object_id);

	result.resource = resource;
	result.value = value;
	result.object_id = object_id;
	return true;
}

mapp::Currency mapp::Game::add(const CurrencyDefinition& currency)
{
	Currency result;
	result.resource.builder().set_type("currency");
	result.resource.builder().set_name(currency.name);

	result.resource = brochure->builder().add_resource(result.resource);

	return result;
}

void mapp::Game::remove(const Currency& currency)
{
	remove(currency.resource);
}

mapp::Currency mapp::Game::update(const Currency& currency, const CurrencyDefinition& definition)
{
	auto resource = currency.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);

	invalidate(resource);

	return currency;
}

bool mapp::Game::currency(const twoflower::Resource& resource, Currency& result) const
{
	if (resource.get_type() != "currency")
	{
		return false;
	}

	result.resource = resource;
	return true;
}

mapp::Currency mapp::Game::currency(const std::string& name) const
{
	auto resources = brochure->resources();
	for (auto i = resources.by_name(name); i != resources.end(); ++i)
	{
		if (i->get_type() == "currency")
		{
			mapp::Currency result;
			if (currency(*i, result))
			{
				return result;
			}
		}
	}

	return mapp::Currency();
}

mapp::Skill mapp::Game::add(const SkillDefinition& skill)
{
	Skill result;
	result.resource.builder().set_type("skill");
	result.resource.builder().set_name(skill.name);

	result.resource = brochure->builder().add_resource(result.resource);

	return result;
}

void mapp::Game::remove(const Skill& skill)
{
	remove(skill.resource);
}

mapp::Skill mapp::Game::update(const Skill& skill, const SkillDefinition& definition)
{
	auto resource = skill.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);

	invalidate(resource);

	return skill;
}

bool mapp::Game::skill(const twoflower::Resource& resource, Skill& result) const
{
	if (resource.get_type() != "skill")
	{
		return false;
	}

	result.resource = resource;
	return true;
}

mapp::Skill mapp::Game::skill(const std::string& name) const
{
	auto resources = brochure->resources();
	for (auto i = resources.by_name(name); i != resources.end(); ++i)
	{
		if (i->get_type() == "skill")
		{
			mapp::Skill result;
			if (skill(*i, result))
			{
				return result;
			}
		}
	}

	return mapp::Skill();
}

mapp::Ability mapp::Game::add(const AbilityDefinition& ability)
{
	Ability result;
	result.resource.builder().set_type("ability");
	result.resource.builder().set_name(ability.name);

	result.resource = brochure->builder().add_resource(result.resource);

	return result;
}

void mapp::Game::remove(const Ability& ability)
{
	remove(ability.resource);
}

mapp::Ability mapp::Game::update(const Ability& ability, const AbilityDefinition& definition)
{
	auto resource = ability.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);

	invalidate(resource);

	return ability;
}

bool mapp::Game::ability(const twoflower::Resource& resource, Ability& result) const
{
	if (resource.get_type() != "ability")
	{
		return false;
	}

	result.resource = resource;
	return true;
}

mapp::Ability mapp::Game::ability(const std::string& name) const
{
	auto resources = brochure->resources();
	for (auto i = resources.by_name(name); i != resources.end(); ++i)
	{
		if (i->get_type() == "ability")
		{
			mapp::Ability result;
			if (ability(*i, result))
			{
				return result;
			}
		}
	}

	return mapp::Ability();
}

mapp::Quest mapp::Game::add(const QuestDefinition& quest)
{
	Quest result;
	result.resource.builder().set_type("quest");
	result.resource.builder().set_name(quest.name);

	result.resource = brochure->builder().add_resource(result.resource);

	return result;
}

void mapp::Game::remove(const Quest& quest)
{
	remove(quest.resource);
}

mapp::Quest mapp::Game::update(const Quest& quest, const QuestDefinition& definition)
{
	auto resource = quest.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);

	invalidate(resource);

	return quest;
}

bool mapp::Game::quest(const twoflower::Resource& resource, Quest& result) const
{
	if (resource.get_type() != "quest")
	{
		return false;
	}

	result.resource = resource;
	return true;
}

mapp::Quest mapp::Game::quest(const std::string& name) const
{
	auto resources = brochure->resources();
	for (auto i = resources.by_name(name); i != resources.end(); ++i)
	{
		if (i->get_type() == "quest")
		{
			mapp::Quest result;
			if (quest(*i, result))
			{
				return result;
			}
		}
	}

	return mapp::Quest();
}

mapp::Shop mapp::Game::add(const ShopDefinition& shop)
{
	Shop result;
	result.resource.builder().set_type("shop");
	result.resource.builder().set_name(shop.name);

	result.resource = brochure->builder().add_resource(result.resource);

	if (shop.location.resource.get_id())
	{
		twoflower::Action parent_link;
		parent_link.builder().set_type("travel");
		parent_link.builder().set_name("parent");
		parent_link = brochure->builder().connect(parent_link, result.resource);
		brochure->builder().connect(
			twoflower::Requirement(),
			parent_link,
			shop.location.resource);

		twoflower::Action child_link;
		child_link.builder().set_type("travel");
		child_link.builder().set_name("child");
		child_link = brochure->builder().connect(child_link, shop.location.resource);
		brochure->builder().connect(
			twoflower::Requirement(),
			child_link,
			result.resource);
		
		result.location = shop.location;
	}


	return result;
}

void mapp::Game::remove(const Shop& shop)
{
	auto items = inventory(shop);
	for (auto& item: items)
	{
		remove(shop, item);
	}

	remove(shop.resource);
}

mapp::Shop mapp::Game::update(const Shop& shop, const ShopDefinition& definition)
{
	auto resource = shop.resource;
	resource.builder().set_name(definition.name);
	brochure->builder().update_resource(resource);

	mapp::Location parent;
	while (atlas.parent(shop.resource, parent))
	{
		auto actions = brochure->actions(parent.resource);
		auto child = actions.by_type("travel", "child");
		auto end = actions.end();
		while (child != end)
		{
			mapp::Location destination;
			if (atlas.destination(*child, destination) &&
				destination.resource.get_id() == shop.location.resource.get_id())
			{
				brochure->builder().remove_action(*child);
			}

			++child;
		}
	}

	auto actions = brochure->actions(shop.resource);
	for (auto i = actions.by_type("travel", "parent"); i != actions.end(); ++i)
	{
		brochure->builder().remove_action(*i);
	}

	if (definition.location.resource.get_id())
	{
		twoflower::Action parent_link;
		parent_link.builder().set_type("travel");
		parent_link.builder().set_name("parent");
		parent_link = brochure->builder().connect(parent_link, resource);
		brochure->builder().connect(
			twoflower::Requirement(),
			parent_link,
			definition.location.resource);

		twoflower::Action child_link;
		child_link.builder().set_type("travel");
		child_link.builder().set_name("child");
		child_link = brochure->builder().connect(child_link, definition.location.resource);
		brochure->builder().connect(
			twoflower::Requirement(),
			child_link,
			resource);
	}

	invalidate(shop.resource);

	return shop;
}

bool mapp::Game::shop(const twoflower::Resource& resource, Shop& result) const
{
	if (resource.get_type() != "shop")
	{
		return false;
	}

	mapp::Location location;
	atlas.parent(resource, location);

	result.resource = resource;
	result.location = location;
	return true;
}

mapp::Shops mapp::Game::shops(const Item& item) const
{
	auto iter = cache_item_shops.find(item.resource.get_id());
	if (iter != cache_item_shops.end())
	{
		return iter->second;
	}

	Shops results;
	auto actions = brochure->actions(item.resource);
	auto current = actions.by_type("buy", "shop");
	auto end = actions.end();
	while (current != end)
	{
		auto requirements = brochure->requirements(*current);
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_type() == "shop")
			{
				Shop result;
				if (shop(requirement.get_resource(), result))
				{
					results.push_back(result);
				}
			}
		}

		++current;
	}

	cache_item_shops[item.resource.get_id()] = results;
	return results;
}

mapp::Items mapp::Game::inventory(const Shop& shop) const
{
	auto iter = cache_shop_inventory.find(shop.resource.get_id());
	if (iter != cache_shop_inventory.end())
	{
		return iter->second;
	}

	Items results;
	auto actions = brochure->actions(shop.resource);
	auto current = actions.by_type("buy", "shop");
	auto end = actions.end();
	while (current != end)
	{
		auto requirements = brochure->requirements(*current);
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_type() == "item")
			{
				Item result;
				if (item(requirement.get_resource(), result))
				{
					results.push_back(result);
				}
			}
		}

		++current;
	}

	cache_shop_inventory[shop.resource.get_id()] = results;
	return results;
}

void mapp::Game::add(const Shop& shop, const Item& item, int quantity)
{
	twoflower::Action shop_buy_action, item_buy_action;
	twoflower::Resource inventory_resource;
	if (get_buy_actions(shop, item, shop_buy_action, item_buy_action, inventory_resource))
	{
		auto actions = brochure->actions(inventory_resource);
		for (auto i = actions.by_type("buy", "shop"); i != actions.end(); ++i)
		{
			auto requirements = brochure->requirements(*i);
			for (auto requirement: requirements)
			{
				if (requirement.get_resource().get_id() == item.resource.get_id())
				{
					requirement.builder().set_count(quantity);
					brochure->builder().update_requirement(requirement);
				}
			}
		}
	}
	else
	{
		twoflower::Resource inventory_resource;
		inventory_resource.builder().set_type("shop_inventory");
		inventory_resource.builder().set_name(
			shop.resource.get_name() + "::" + item.resource.get_name());
		inventory_resource = brochure->builder().add_resource(inventory_resource);

		twoflower::Action buy_action;
		buy_action.builder().set_type("buy");
		buy_action.builder().set_name("shop");

		twoflower::Requirement quantity_requirement;
		quantity_requirement.builder().set_count(quantity);

		twoflower::Requirement item_requirement;
		item_requirement.builder().set_is_output(true);
		item_requirement.builder().set_count(1);

		twoflower::Requirement inventory_requirement;
		inventory_requirement.builder().set_is_input(true);

		twoflower::Requirement shop_requirement;

		auto inventory_quantity_action = brochure->builder().connect(buy_action, inventory_resource);
		brochure->builder().connect(quantity_requirement, inventory_quantity_action, item.resource);

		auto shop_buy_action = brochure->builder().connect(buy_action, shop.resource);
		brochure->builder().connect(inventory_requirement, shop_buy_action, inventory_resource);
		brochure->builder().connect(item_requirement, shop_buy_action, item.resource);

		auto item_buy_action = brochure->builder().connect(buy_action, item.resource);
		brochure->builder().connect(inventory_requirement, item_buy_action, inventory_resource);
		brochure->builder().connect(shop_requirement, item_buy_action, shop.resource);
		brochure->builder().connect(item_requirement, item_buy_action, item.resource);
	}

	invalidate(shop.resource);
	invalidate(item.resource);
}

void mapp::Game::add(
	const Shop& shop,
	const Item& item,
	const Currency& currency,
	int price)
{
	remove(shop, item, currency);

	twoflower::Requirement currency_requirement;
	currency_requirement.builder().set_is_input(true);
	currency_requirement.builder().set_count(price);

	twoflower::Action shop_buy_action, item_buy_action;
	twoflower::Resource unused_inventory_resource;
	if (!get_buy_actions(shop, item, shop_buy_action, item_buy_action, unused_inventory_resource))
	{
		throw std::runtime_error("shop does not have item in inventory");
	}

	brochure->builder().connect(currency_requirement, shop_buy_action, currency.resource);
	brochure->builder().connect(currency_requirement, item_buy_action, currency.resource);

	invalidate(shop.resource);
	invalidate(item.resource);
	invalidate(currency.resource);
}

mapp::ShopPrice mapp::Game::price(const Shop& shop, const Item& item)
{
	auto key = std::make_tuple(shop.resource.get_id(), item.resource.get_id());
	auto iter = cache_prices.find(key);
	if (iter != cache_prices.end())
	{
		return iter->second;
	}

	ShopPrice results;
	auto actions = brochure->actions(shop.resource);
	auto current = actions.by_type("buy", "shop");
	auto end = actions.end();
	while (current != end)
	{
		auto requirements = brochure->requirements(*current);
		++current;

		bool has_item = false;
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_id() == item.resource.get_id())
			{
				has_item = true;
			}
		}

		if (!has_item)
		{
			continue;
		}

		for (auto requirement: requirements)
		{
			Price result;
			if (requirement.get_resource().get_type() == "currency" &&
				currency(requirement.get_resource(), result.second))
			{
				result.first = requirement.get_count();
				results.push_back(result);
			}
		}
	}

	std::sort(
		results.begin(), results.end(),
		[](auto& a, auto& b)
		{
			return a.second.resource.get_name() < b.second.resource.get_name();
		});

	cache_prices[key] = results;
	return results;
}

int mapp::Game::quantity(const Shop& shop, const Item& item)
{
	auto key = std::make_tuple(shop.resource.get_id(), item.resource.get_id());
	auto iter = cache_quantity.find(key);
	if (iter != cache_quantity.end())
	{
		return iter->second;
	}

	twoflower::Action shop_buy_action, item_buy_action;
	twoflower::Resource inventory_resource;
	if (get_buy_actions(shop, item, shop_buy_action, item_buy_action, inventory_resource))
	{
		auto actions = brochure->actions(inventory_resource);
		for (auto i = actions.by_type("buy", "shop"); i != actions.end(); ++i)
		{
			auto requirements = brochure->requirements(*i);
			for (auto requirement: requirements)
			{
				if (requirement.get_resource().get_id() == item.resource.get_id())
				{
					cache_quantity[key] = requirement.get_count();
					return requirement.get_count();
				}
			}
		}
	}

	cache_quantity[key] = 0;
	return 0;
}

void mapp::Game::remove(const Shop& shop, const Item& item)
{
	twoflower::Action shop_buy_action, item_buy_action;
	twoflower::Resource inventory_resource;
	if (get_buy_actions(shop, item, shop_buy_action, item_buy_action, inventory_resource))
	{
		brochure->builder().remove_action(shop_buy_action);
		brochure->builder().remove_action(item_buy_action);
		remove(inventory_resource);
	}

	invalidate(shop.resource);
	invalidate(item.resource);
}

static void remove_currency(
	twoflower::Brochure& brochure,
	const twoflower::Resource& a,
	const twoflower::Resource& b,
	const mapp::Currency& currency)
{
	auto actions = brochure.actions(a);
	auto current = actions.by_type("buy", "shop");
	auto end = actions.end();

	while (current != end)
	{
		auto requirements = brochure.requirements(*current);
		++current;

		bool has_item = false;
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_id() == b.get_id())
			{
				has_item = true;
			}
		}

		if (!has_item)
		{
			continue;
		}

		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_id() == currency.resource.get_id())
			{
				brochure.builder().remove_requirement(requirement);
			}
		}
	}
}

void mapp::Game::remove(const Shop& shop, const Item& item, const Currency& currency)
{
	remove_currency(*brochure, shop.resource, item.resource, currency);
	remove_currency(*brochure, item.resource, shop.resource, currency);

	invalidate(shop.resource);
	invalidate(item.resource);
	invalidate(currency.resource);
}

void mapp::Game::ensure_action_definition(
	const std::string& type,
	const std::string& name,
	bool getter)
{
	if (!brochure->has_action_definition(type, name))
	{
		twoflower::Action action;
		action.builder().set_type(type);
		action.builder().set_name(name);
		action.builder().set_is_getter(getter);

		brochure->builder().add_action_definition(action);
	}
}

void mapp::Game::ensure_resource_type(const std::string& type)
{
	if (!brochure->has_resource_type(type))
	{
		brochure->builder().add_resource_type(type);
	}
}

bool mapp::Game::get_buy_actions(
	const Shop& shop,
	const Item& item,
	twoflower::Action& shop_buy_action,
	twoflower::Action& item_buy_action,
	twoflower::Resource& inventory_resource)
{
	twoflower::Resource pending_inventory_resource;
	twoflower::Action pending_shop_buy_action;

	auto shop_actions = brochure->actions(shop.resource);
	for (auto i = shop_actions.by_type("buy", "shop"); i != shop_actions.end(); ++i)
	{
		auto requirements = brochure->requirements(*i);

		bool has_item = false;
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_id() == item.resource.get_id())
			{
				has_item = true;
			}
		}

		if (!has_item)
		{
			continue;
		}

		bool got_inventory_resource = false;
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_type() == "shop_inventory")
			{
				pending_inventory_resource = requirement.get_resource();
				pending_shop_buy_action = *i;
				got_inventory_resource = true;
				break;
			}
		}

		if (got_inventory_resource)
		{
			break;
		}
	}

	if (!pending_shop_buy_action.get_id())
	{
		return false;
	}

	auto item_actions = brochure->actions(item.resource);
	for (auto i = item_actions.by_type("buy", "shop"); i != item_actions.end(); ++i)
	{
		auto requirements = brochure->requirements(*i);

		bool has_item = false;
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_id() == item.resource.get_id())
			{
				has_item = true;
			}
		}

		if (!has_item)
		{
			continue;
		}
		for (auto requirement: requirements)
		{
			if (requirement.get_resource().get_type() == "shop_inventory" &&
				requirement.get_resource().get_id() == pending_inventory_resource.get_id())
			{
				shop_buy_action = pending_shop_buy_action;
				item_buy_action = *i;
				inventory_resource = pending_inventory_resource;
				return true;
			}
		}
	}

	return false;
}

void mapp::Game::remove(const twoflower::Resource& resource)
{
	brochure->builder().remove_resource(resource);
	invalidate(resource);
}

void mapp::Game::invalidate(const twoflower::Resource& resource)
{
	// TODO: Only clear affected caches.
	cache_shop_inventory.clear();
	cache_item_shops.clear();
	cache_prices.clear();
	cache_quantity.clear();
}
