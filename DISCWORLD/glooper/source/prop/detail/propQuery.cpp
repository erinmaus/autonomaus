// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "mantorok/io/datReader.hpp"
#include "glooper/prop/detail/propQuery.hpp"
#include "glooper/resources/resourceManager.hpp"
#include "chomsky/io/world/query/materialSetMatch.hpp"
#include "chomsky/io/world/query/tileSpacePointCloudMatch.hpp"

glooper::PropQuery::PropQuery(const std::string& name, ResourceManager& resource_manager)
{
	this->name = name;
	this->resource_manager = &resource_manager;
	this->resource_manager->add_listener(this);

	on_reload_resources();
}

glooper::PropQuery::~PropQuery()
{
	this->resource_manager->remove_listener(this);
}

const std::string& glooper::PropQuery::get_name() const
{
	return this->name;
}

bool glooper::PropQuery::match(
	const thoreau::TileCoordinate& location,
	const chomsky::StaticTileSpacePointCloudMatchExecutor& executor) const
{
	for (auto& i: this->model_matches)
	{
		if (executor.execute(location, i.second))
		{
			return true;
		}
	}

	return false;
}

bool glooper::PropQuery::match(
	const thoreau::TileCoordinate& location,
	const chomsky::StaticMaterialSetMatchExecutor& executor) const
{
	for (auto& i: this->material_matches)
	{
		if (executor.execute(location, i.second))
		{
			return true;
		}
	}

	return false;
}

void glooper::PropQuery::on_reload_resources()
{
	this->model_matches.clear();

	auto root = get_root_path();
	load_models(root);
	load_materials(root);
}

void glooper::PropQuery::load_models(const boost::filesystem::path& root)
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

void glooper::PropQuery::load_materials(const boost::filesystem::path& root)
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

boost::filesystem::path glooper::PropQuery::get_root_path() const
{
	auto base = this->resource_manager->get_config_string(
		"prop", "queries", "queries/props");

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
