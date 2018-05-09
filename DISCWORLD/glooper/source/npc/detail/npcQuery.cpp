// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "mantorok/io/datReader.hpp"
#include "glooper/npc/detail/npcQuery.hpp"
#include "glooper/resources/resourceManager.hpp"
#include "chomsky/io/world/query/materialSetMatch.hpp"
#include "chomsky/io/world/query/tileSpacePointCloudMatch.hpp"

glooper::NPCQuery::NPCQuery(const std::string& name, ResourceManager& resource_manager)
{
	this->name = name;
	this->resource_manager = &resource_manager;
	this->resource_manager->add_listener(this);

	on_reload_resources();
}

glooper::NPCQuery::~NPCQuery()
{
	this->resource_manager->remove_listener(this);
}

bool glooper::NPCQuery::match(
	const thoreau::WorldCoordinate& location,
	const chomsky::ActorTileSpacePointCloudMatchExecutor& executor,
	std::string& result) const
{
	for (auto& i: this->model_matches)
	{
		if (executor.execute(location, i.second))
		{
			result = i.first;
			return true;
		}
	}

	return false;
}

bool glooper::NPCQuery::match(
	const thoreau::WorldCoordinate& location,
	const chomsky::ActorMaterialSetMatchExecutor& executor,
	std::string& result) const
{
	for (auto& i: this->material_matches)
	{
		if (executor.execute(location, i.second))
		{
			result = i.first;
			return true;
		}
	}

	return false;
}

bool glooper::NPCQuery::match(
	const chomsky::ActorDraw& draw,
	std::vector<std::string>& results) const
{
	bool success = false;
	chomsky::ActorAnimation animation;
	animation.extract(draw);

	for (auto& i: this->animation_matches)
	{
		if (i.second.match(animation))
		{
			results.emplace_back(i.first);
			success = true;
		}
	}

	return success;
}

bool glooper::NPCQuery::match(
	const thoreau::WorldCoordinate& location,
	const chomsky::ActorTileSpacePointCloudMatchExecutor& executor,
	std::vector<std::string>& results) const
{
	bool success = false;
	for (auto& i: this->effect_matches)
	{
		if (executor.execute(location, i.second))
		{
			results.emplace_back(i.first);
			success = true;
		}
	}

	return success;
}

void glooper::NPCQuery::on_reload_resources()
{
	this->model_matches.clear();
	this->material_matches.clear();
	this->effect_matches.clear();
	this->animation_matches.clear();

	auto root = get_root_path();
	load_models(root);
	load_materials(root);
	load_animations(root);
}

void glooper::NPCQuery::load_models(const boost::filesystem::path& root)
{
	auto models_path = root;
	models_path.append("models");

	this->resource_manager->for_each_file(
		models_path.string(),
		[this, &models_path, &root](auto& file)
		{
			boost::filesystem::path file_path(file);
			if (file_path != models_path)
			{
				auto model = file_path.stem().string();
				chomsky::TileSpacePointCloudMatch match;

				std::ifstream stream(file, std::ios::binary);
				mantorok::DATReader reader(stream);
				mantorok::DATDeserializer<chomsky::TileSpacePointCloudMatch>::deserialize(match, reader);

				if (!stream.bad() && !match.empty())
				{
					this->model_matches.emplace(model, match);
				}
			}
		});
}

void glooper::NPCQuery::load_materials(const boost::filesystem::path& root)
{
	auto models_path = root;
	models_path.append("materials");

	this->resource_manager->for_each_file(
		models_path.string(),
		[this, &models_path, &root](auto& file)
		{
			boost::filesystem::path file_path(file);
			if (file_path != models_path)
			{
				auto model = file_path.stem().string();
				chomsky::MaterialSetMatch match;
				
				std::ifstream stream(file, std::ios::binary);
				mantorok::DATReader reader(stream);
				mantorok::DATDeserializer<chomsky::MaterialSetMatch>::deserialize(match, reader);

				if (!stream.bad() && !match.empty())
				{
					this->material_matches.emplace(model, match);
				}
			}
		});
}

void glooper::NPCQuery::load_animations(const boost::filesystem::path& root)
{
	auto animations_path = root;
	animations_path.append("animations");

	this->resource_manager->for_each_directory(
		animations_path.string(),
		false,
		[this](auto& directory)
		{
			boost::filesystem::path path = directory;
			{
				auto p = path;
				p.append("effects");
				this->load_effect(path.stem().string(), p);
			}
			{
				auto p = path;
				p.append("animations");
				this->load_animation(path.stem().string(), p);
			}
		});
}

void glooper::NPCQuery::load_animation(
	const std::string& name, const boost::filesystem::path& root)
{
	this->resource_manager->for_each_file(
		root.string(),
		[this, &name](auto& filename)
		{
			boost::filesystem::path path(filename);
			std::ifstream stream(filename, std::ios::binary);
			mantorok::DATReader reader(stream);

			chomsky::ActorAnimation animation;
			mantorok::DATDeserializer<chomsky::ActorAnimation>::deserialize(
				animation, reader);

			if (!stream.bad() && animation.get_num_frames() != 0)
			{
				this->animation_matches.emplace(name, animation);
			}
		});
}

void glooper::NPCQuery::load_effect(
	const std::string& name, const boost::filesystem::path& root)
{
	this->resource_manager->for_each_file(
		root.string(),
		[this, &name](auto& filename)
		{
			chomsky::TileSpacePointCloudMatch match;

			std::ifstream stream(filename, std::ios::binary);
			mantorok::DATReader reader(stream);
			mantorok::DATDeserializer<chomsky::TileSpacePointCloudMatch>::deserialize(
				match, reader);

			if (!stream.bad() && !match.empty())
			{
				this->effect_matches.emplace(name, match);
			}
		});
}

boost::filesystem::path glooper::NPCQuery::get_root_path() const
{
	auto base = this->resource_manager->get_config_string(
		"npc", "queries", "queries/npcs");

	boost::filesystem::path root_path(base);
	if (!root_path.is_absolute())
	{
		boost::filesystem::path base_path(this->resource_manager->get_path());
		base_path.append(base);

		root_path = base_path;
	}

	std::size_t current = 0;
	while (current != std::string::npos)
	{
		std::size_t next = this->name.find(".", current);
		if (next == std::string::npos)
		{
			root_path.append(this->name.substr(current, this->name.length()));
			current = next;
		}
		else
		{
			root_path.append(this->name.substr(current, next - current));
			current = next + 1;
		}
	}

	return root_path;
}
