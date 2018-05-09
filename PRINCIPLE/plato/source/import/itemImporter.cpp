// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mapp/game/game.hpp"
#include "plato/import/itemImporter.hpp"

int plato::ItemImporter::import(
	const Json::Value& value,
	twoflower::Brochure& brochure)
{
	int count = 0;
	mapp::Game game(brochure);

	auto items = value.get("item", Json::Value());
	for (auto& i: items)
	{
		for (auto& j: i)
		{
			std::string name = j.get("name", std::string()).asString();
			if (name.empty())
			{
				continue;
			}

			int object_value = std::atoi(j.get("value", "0").asString().c_str());
			int object_id = std::atoi(j.get("id", std::to_string(mapp::Item::NULL_OBJECT_ID)).asString().c_str());

			mapp::Item item;
			bool has_item = false;
			auto resources = brochure.resources();
			for (auto k = resources.by_name(name); k != resources.end(); ++k)
			{
				if (k->get_type() != "item")
				{
					continue;
				}

				if (game.item(*k, item) && item.object_id == object_id)
				{
					has_item = true;
					break;
				}
			}

			if (!has_item)
			{
				mapp::ItemDefinition definition;
				definition.name = name;
				definition.object_id = object_id;
				definition.value = object_value;

				item = game.add(definition);
				++count;
			}

			brochure.builder().set_userdata(
				item.resource,
				"mapp-rs-wiki-article-title",
				value.get("_title", std::string()).asString());
		}
	}

	return count;
}
