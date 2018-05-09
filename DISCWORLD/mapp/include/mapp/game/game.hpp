// MAPP
//
// This Source Code Form is subject to the terms of the Mozilla Public
// License, v. 2.0. If a copy of the MPL was not distributed with this
// file, You can obtain one at http://mozilla.org/MPL/2.0/.
//
// Copyright [bk]door.maus

#ifndef MAPP_GAME_GAME_HPP
#define MAPP_GAME_GAME_HPP

#include <map>
#include <string>
#include <unordered_map>
#include <utility>
#include "mapp/atlas/atlas.hpp"
#include "mapp/game/ability.hpp"
#include "mapp/game/currency.hpp"
#include "mapp/game/item.hpp"
#include "mapp/game/quest.hpp"
#include "mapp/game/shop.hpp"
#include "mapp/game/skill.hpp"
#include "twoflower/brochure.hpp"

namespace mapp
{
	class Game
	{
	public:
		Game(twoflower::Brochure& brochure);
		~Game() = default;

		Item add(const ItemDefinition& item);
		void remove(const Item& item);
		Item update(const Item& item, const ItemDefinition& definition);
		bool item(const twoflower::Resource& resource, Item& result) const;

		Currency add(const CurrencyDefinition& currency);
		void remove(const Currency& currency);
		Currency update(const Currency& currency, const CurrencyDefinition& definition);
		bool currency(const twoflower::Resource& resource, Currency& result) const;
		Currency currency(const std::string& name) const;

		Skill add(const SkillDefinition& skill);
		void remove(const Skill& skill);
		Skill update(const Skill& skill, const SkillDefinition& definition);
		bool skill(const twoflower::Resource& resource, Skill& result) const;
		Skill skill(const std::string& name) const;

		Ability add(const AbilityDefinition& ability);
		void remove(const Ability& ability);
		Ability update(const Ability& ability, const AbilityDefinition& definition);
		bool ability(const twoflower::Resource& resource, Ability& result) const;
		Ability ability(const std::string& name) const;

		Quest add(const QuestDefinition& quest);
		void remove(const Quest& quest);
		Quest update(const Quest& quest, const QuestDefinition& definition);
		bool quest(const twoflower::Resource& resource, Quest& result) const;
		Quest quest(const std::string& name) const;

		Shop add(const ShopDefinition& shop);
		void remove(const Shop& shop);
		Shop update(const Shop& shop, const ShopDefinition& definition);
		bool shop(const twoflower::Resource& resource, Shop& result) const;

		Shops shops(const Item& item) const;
		Items inventory(const Shop& shop) const;
		void add(const Shop& shop, const Item& item, int quantity);
		void add(const Shop& shop, const Item& item, const Currency& currency, int price);
		ShopPrice price(const Shop& shop, const Item& item);
		int quantity(const Shop& shop, const Item& item);
		void remove(const Shop& shop, const Item& item);
		void remove(const Shop& shop, const Item& item, const Currency& currency);

	private:
		void ensure_action_definition(
			const std::string& type,
			const std::string& name,
			bool getter);
		void ensure_resource_type(const std::string& type);

		bool get_buy_actions(
			const Shop& shop,
			const Item& item,
			twoflower::Action& shop_buy_action,
			twoflower::Action& item_buy_action,
			twoflower::Resource& inventory_resource);

		mutable std::unordered_map<int, Items> cache_shop_inventory;
		mutable std::unordered_map<int, Shops> cache_item_shops;

		typedef std::tuple<int, int> ShopItemKey;
		mutable std::map<ShopItemKey, ShopPrice> cache_prices;
		mutable std::map<ShopItemKey, int> cache_quantity;

		void remove(const twoflower::Resource& resource);
		void invalidate(const twoflower::Resource& resource);

		twoflower::Brochure* brochure;
		mapp::Atlas atlas;
	};
}

#endif
