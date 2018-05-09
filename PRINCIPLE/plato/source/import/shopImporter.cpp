// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mapp/game/game.hpp"
#include "mapp/atlas/atlas.hpp"
#include "plato/import/shopImporter.hpp"

int plato::ShopImporter::import(
	const Json::Value& value,
	twoflower::Brochure& brochure)
{
	int count = 0;
	mapp::Game game(brochure);
	mapp::Atlas atlas(brochure);

	auto shops = value.get("infobox shop", Json::Value());
	for (auto& i: shops)
	{
		std::string name = i.get("name", std::string()).asString();
		if (name.empty())
		{
			continue;
		}

		mapp::Shop shop;
		bool has_shop = false;
		auto resources = brochure.resources();
		for (auto j = resources.by_name(name); j != resources.end(); ++j)
		{
			if (j->get_type() != "shop")
			{
				continue;
			}

			if (game.shop(*j, shop))
			{
				has_shop = true;
				break;
			}
		}

		if (has_shop)
		{
			if (!shop.location.resource.get_id())
			{
				mapp::ShopDefinition definition = shop;
				atlas.find(name, definition.location);

				shop = game.update(shop, definition);
			}
		}
		else
		{
			mapp::ShopDefinition definition;
			definition.name = name;

			shop = game.add(definition);
			++count;
		}

		auto items = value.get("store", Json::Value());
		for (auto& j: items)
		{
			std::string name = j.get("name", std::string()).asString();
			if (name.empty())
			{
				continue;
			}

			std::string display_name = j.get("displayname", std::string()).asString();
			if (!display_name.empty())
			{
				continue;
			}

			mapp::Item item;
			bool got_item = false;
			auto resources = brochure.resources();
			for (auto i = resources.by_name(name); i != resources.end(); ++i)
			{
				if (i->get_type() == "item")
				{
					if (game.item(*i, item))
					{
						got_item = true;
						break;
					}
				}
			}

			if (!got_item)
			{
				for (auto i = resources.by_type("item"); i != resources.end(); ++i)
				{
					std::string article_title;
					if (brochure.get_userdata(*i, "mapp-rs-wiki-article-title", article_title) &&
						article_title == name)
					{
						if (game.item(*i, item))
						{
							got_item = true;
							break;
						}
					}
				}
			}

			if (!got_item)
			{
				continue;
			}

			int quantity = std::atoi(j.get("stock", "0").asString().c_str());
			int price = std::atoi(j.get("sell", "0").asString().c_str());
			std::string currency_name = j.get("currency", "Coins").asString();

			mapp::Currency currency = game.currency(currency_name);
			if (!currency.resource.get_id())
			{
				continue;
			}

			game.add(shop, item, quantity);
			game.add(shop, item, currency, price);
		}
	}

	return count;
}
