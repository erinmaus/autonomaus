// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/regex.hpp>
#include "mapp/game/game.hpp"
#include "plato/import/spellImporter.hpp"

int plato::SpellImporter::import(
	const Json::Value& value,
	twoflower::Brochure& brochure)
{
	int count = 0;
	mapp::Game game(brochure);

	auto spells = value.get("infobox spell", Json::Value());
	for (auto& i: spells)
	{
		std::string name = i.get("name", std::string()).asString();
		if (name.empty())
		{
			continue;
		}

		float xp = std::atof(i.get("exp", "0.0").asString().c_str());
		int level = std::atoi(i.get("level", "0").asString().c_str());

		std::string cost = i.get("cost", std::string()).asString();
		std::map<std::string, int> runes;
		{
			boost::regex e("([A-Za-z]*)=([0-9]*)");
			boost::sregex_iterator begin(cost.begin(), cost.end(), e);
			boost::sregex_iterator end;

			while (begin != end)
			{
				auto& m = *begin;
				for (std::size_t i = 1; i < m.size(); i += 2)
				{
					std::string rune = m[i];
					std::string count = m[i + 1];
					runes.emplace(rune + " rune", std::atoi(count.c_str()));
				}

				++begin;
			}
		}
		if (runes.empty())
		{
			boost::regex e("([0-9]*)\\s*\\[\\[([^\\|]*)");
			boost::sregex_iterator begin(cost.begin(), cost.end(), e);
			boost::sregex_iterator end;

			while (begin != end)
			{
				auto& m = *begin;
				for (std::size_t i = 1; i < m.size(); i += 2)
				{
					std::string rune = m[i + 1];
					std::string count = m[i];
					runes.emplace(rune, std::atoi(count.c_str()));
				}

				++begin;
			}

			if (runes.empty())
			{
				printf("no match: %s\n", name.c_str());
				printf("\t- cost: '%s'\n", cost.c_str());
			}
		}

		mapp::Ability spell;
		bool has_spell = false;
		auto resources = brochure.resources();
		for (auto k = resources.by_name(name); k != resources.end(); ++k)
		{
			if (k->get_type() != "ability")
			{
				continue;
			}

			if (game.ability(*k, spell))
			{
				has_spell = true;
			}
		}

		if (!has_spell)
		{
			mapp::AbilityDefinition definition;
			definition.name = name;

			spell = game.add(definition);

			twoflower::Action use_action;
			use_action.builder().set_type("ability");
			use_action.builder().set_name("use");
			use_action = brochure.builder().connect(use_action, spell.resource);

			auto resources = brochure.resources();
			for (auto& i: runes)
			{
				twoflower::Resource rune;
				int object_id = std::numeric_limits<int>::max();

				auto begin = resources.by_name(i.first);
				auto end = resources.end();
				while (begin != end)
				{
					if (begin->get_type() == "item")
					{
						mapp::Item r;
						if (game.item(*begin, r))
						{
							if (r.object_id != mapp::Item::NULL_OBJECT_ID &&
								r.object_id < object_id)
							{
								rune = r.resource;
								object_id = r.object_id;
							}
						}
					}
					++begin;
				}

				if (rune.get_id())
				{
					twoflower::Requirement requirement;
					requirement.builder().set_count(i.second);
					requirement.builder().set_is_input(true);

					brochure.builder().connect(requirement, use_action, rune);
				}
			}

			twoflower::Resource magic_skill;
			{
				for (auto i = resources.by_name("Magic"); i != resources.end(); ++i)
				{
					if (i->get_type() == "skill")
					{
						magic_skill = *i;
						break;
					}
				}
			}

			if (!magic_skill.get_id())
			{
				continue;
			}

			if (xp != 0.0f)
			{
				twoflower::Requirement x;
				x.builder().set_count(xp);
				x.builder().set_is_output(true);

				brochure.builder().connect(x, use_action, magic_skill);
			}

			if (level != 0)
			{
				twoflower::Requirement l;
				l.builder().set_count(mapp::xp_for_level(level));

				brochure.builder().connect(l, use_action, magic_skill);
			}

			++count;
		}

		brochure.builder().set_userdata(
			spell.resource,
			"mapp-rs-wiki-article-title",
			value.get("_title", std::string()).asString());
	}

	return count;
}
