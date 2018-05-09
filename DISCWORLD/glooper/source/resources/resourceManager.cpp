// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cerrno>
#include <cstdlib>
#include <fstream>
#include <boost/filesystem.hpp>
#include "glooper/deps/inih/ini.h"
#include "glooper/resources/resourceListener.hpp"
#include "glooper/resources/resourceManager.hpp"
#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/io/gui/itemIconSilhouette.hpp"
#include "chomsky/io/resources/textureDictionary.hpp"

glooper::ResourceManager::ResourceManager(const std::string& path) :
	texture_fingerprint_manager(this->texture_manager),
	texture_key_manager(this->texture_manager, this->texture_fingerprint_manager, this->texture_dictionary),
	item_icon_manager(this->texture_manager, this->texture_key_manager, this->texture_dictionary)
{
	this->path = path;
}

const std::string& glooper::ResourceManager::get_path() const
{
	return this->path;
}

void glooper::ResourceManager::set_path(const std::string& value)
{
	this->path = value;
	reload();
}

void glooper::ResourceManager::for_each_directory(
	const std::string& path,
	bool recursive,
	const std::function<void(const std::string&)>& callback) const
{
	boost::filesystem::path search_path(path);
	if (!search_path.is_absolute())
	{
		boost::filesystem::path root_path(this->path);
		root_path.append(path);

		search_path = root_path;
	}

	if (!boost::filesystem::is_directory(search_path))
	{
		return;
	}

	boost::filesystem::directory_iterator current(search_path);
	boost::filesystem::directory_iterator end;

	while (current != end)
	{
		if (boost::filesystem::is_directory(current->path()))
		{
			callback(current->path().string());

			if (recursive)
			{
				for_each_directory(current->path().string(), true, callback);
			}
		}

		++current;
	}
}

void glooper::ResourceManager::for_each_file(
	const std::string& path,
	const std::function<void(const std::string&)>& callback) const
{
	boost::filesystem::path search_path(path);
	if (!search_path.is_absolute())
	{
		boost::filesystem::path root_path(this->path);
		root_path.append(path);

		search_path = root_path;
	}

	if (boost::filesystem::is_regular_file(search_path))
	{
		callback(search_path.string());
	}
	else if (boost::filesystem::is_directory(search_path))
	{
		boost::filesystem::directory_iterator current(search_path);
		boost::filesystem::directory_iterator end;

		while (current != end)
		{
			for_each_file(current->path().string(), callback);
			++current;
		}
	}
}

kompvter::BufferManager&
glooper::ResourceManager::get_buffer_manager()
{
	return this->buffer_manager;
}

const kompvter::BufferManager&
glooper::ResourceManager::get_buffer_manager() const
{
	return this->buffer_manager;
}

kompvter::TextureManager&
glooper::ResourceManager::get_texture_manager()
{
	return this->texture_manager;
}

const kompvter::TextureManager&
glooper::ResourceManager::get_texture_manager() const
{
	return this->texture_manager;
}

chomsky::TextureFingerprintManager&
glooper::ResourceManager::get_texture_fingerprint_manager()
{
	return this->texture_fingerprint_manager;
}

const chomsky::TextureFingerprintManager&
glooper::ResourceManager::get_texture_fingerprint_manager() const
{
	return this->texture_fingerprint_manager;
}

chomsky::TextureKeyManager&
glooper::ResourceManager::get_texture_key_manager()
{
	return this->texture_key_manager;
}

const chomsky::TextureKeyManager&
glooper::ResourceManager::get_texture_key_manager() const
{
	return this->texture_key_manager;
}

chomsky::ItemIconManager&
glooper::ResourceManager::get_item_icon_manager()
{
	return this->item_icon_manager;
}

const chomsky::ItemIconManager&
glooper::ResourceManager::get_item_icon_manager() const
{
	return this->item_icon_manager;
}

chomsky::TextureDictionary&
glooper::ResourceManager::get_texture_dictionary()
{
	return this->texture_dictionary;
}

const chomsky::TextureDictionary&
glooper::ResourceManager::get_texture_dictionary() const
{
	return this->texture_dictionary;
}

thoreau::Map& glooper::ResourceManager::get_map()
{
	return this->map;
}

const thoreau::Map& glooper::ResourceManager::get_map() const
{
	return this->map;
}

thoreau::Map& glooper::ResourceManager::get_runtime_map()
{
	return this->runtime_map;
}

const thoreau::Map& glooper::ResourceManager::get_runtime_map() const
{
	return this->runtime_map;
}

void glooper::ResourceManager::load_config(const std::string& filename)
{
	auto previous_config = std::move(this->config);

	if (ini_parse(filename.c_str(), &load_ini, this) == 0)
	{
		this->path = get_config_string("resources", "path", this->path);
		reload();
	}
	else
	{
		this->config = std::move(previous_config);
	}
}

void glooper::ResourceManager::save_config(const std::string& filename) const
{
	std::ofstream stream(filename);

	for (auto& section: this->config)
	{
		if (!section.first.empty())
		{
			stream << "[" << section.first << "]" << std::endl;
		}

		for (auto& entry: section.second)
		{
			stream << entry.first << " = " << entry.second << std::endl;
		}
	}
}

float glooper::ResourceManager::get_config_float(
	const std::string& scope, const std::string& key,
	float default_value) const
{
	auto section_iter = this->config.find(scope);
	if (section_iter == this->config.end())
	{
		return default_value;
	}

	auto value_iter = section_iter->second.find(key);
	if (value_iter == section_iter->second.end())
	{
		return default_value;
	}

	errno = 0;
	int result = std::strtof(value_iter->second.c_str(), nullptr);
	if (errno != 0)
	{
		return default_value;
	}

	return result;
}

void glooper::ResourceManager::set_config_float(
	const std::string& scope, const std::string& key,
	float value)
{
	auto& section = this->config[scope];
	section[key] = std::to_string(value);
}

int glooper::ResourceManager::get_config_int(
	const std::string& scope, const std::string& key,
	int default_value) const
{
	auto section_iter = this->config.find(scope);
	if (section_iter == this->config.end())
	{
		return default_value;
	}

	auto value_iter = section_iter->second.find(key);
	if (value_iter == section_iter->second.end())
	{
		return default_value;
	}

	errno = 0;
	int result = std::strtol(value_iter->second.c_str(), nullptr, 0);
	if (errno != 0)
	{
		return default_value;
	}

	return result;
}

void glooper::ResourceManager::set_config_int(
	const std::string& scope, const std::string& key,
	int value)
{
	auto& section = this->config[scope];
	section[key] = std::to_string(value);
}

bool glooper::ResourceManager::get_config_bool(
	const std::string& scope, const std::string& key,
	bool default_value) const
{
	auto section_iter = this->config.find(scope);
	if (section_iter == this->config.end())
	{
		return default_value;
	}

	auto value_iter = section_iter->second.find(key);
	if (value_iter == section_iter->second.end())
	{
		return default_value;
	}

	if (is_true_value(value_iter->second))
	{
		return true;
	}
	else if (is_false_value(value_iter->second))
	{
		return false;
	}
	else
	{
		return default_value;
	}
}

void glooper::ResourceManager::set_config_bool(
	const std::string& scope, const std::string& key,
	bool value)
{
	auto& section = this->config[scope];

	if (value == true)
	{
		section[key] = "true";
	}
	else
	{
		section[key] = "false";
	}
}

void glooper::ResourceManager::set_config_bool(
	const std::string& scope, const std::string& key,
	const std::string& value)
{
	if (is_true_value(value) || is_false_value(value))
	{
		auto& section = this->config[scope];
		section[key] = value;
	}
	else
	{
		assert(false);
	}
}

std::string glooper::ResourceManager::get_config_string(
	const std::string& scope, const std::string& key,
	const std::string& default_value) const
{
	auto section_iter = this->config.find(scope);
	if (section_iter == this->config.end())
	{
		return default_value;
	}

	auto value_iter = section_iter->second.find(key);
	if (value_iter == section_iter->second.end())
	{
		return default_value;
	}

	return value_iter->second;
}

void glooper::ResourceManager::set_config_string(
	const std::string& scope, const std::string& key,
	const std::string& value)
{
	auto& section = this->config[scope];
	section[key] = value;
}

void glooper::ResourceManager::unset_config_entry(
	const std::string& scope, const std::string& key)
{
	auto section_iter = this->config.find(scope);
	if (section_iter != this->config.end())
	{
		section_iter->second.erase(key);
	}
}

int glooper::ResourceManager::load_ini(
	void* user,
	const char* section, const char* name,
	const char* value)
{
	auto resource_manager = (ResourceManager*)user;
	resource_manager->config[section].emplace(name, value);

	return 1;
}

void glooper::ResourceManager::reload()
{
	if (!this->path.empty())
	{
		load_texture_dictionary();
		load_map();
		load_special_items();
	}

	for (auto listener: this->listeners)
	{
		listener->on_reload_resources();
	}
}

void glooper::ResourceManager::add_listener(ResourceListener* resource_listener)
{
	this->listeners.push_back(resource_listener);
}

void glooper::ResourceManager::remove_listener(ResourceListener* resource_listener)
{
	for (auto i = this->listeners.begin(); i != this->listeners.end(); /* Nothing/ */)
	{
		if (*i == resource_listener)
		{
			i = this->listeners.erase(i);
		}
		else
		{
			++i;
		}
	}
}

bool glooper::ResourceManager::is_true_value(const std::string& value)
{
	return value == "true" ||
		value == "yes" ||
		value == "y" ||
		value == "on" ||
		value == "enable" ||
		value == "enabled" ||
		value == "1";
}

bool glooper::ResourceManager::is_false_value(const std::string& value)
{
	return value == "false" ||
		value == "no" ||
		value == "n" ||
		value == "off" ||
		value == "disable" ||
		value == "disabled" ||
		value == "0";
}

void glooper::ResourceManager::load_texture_dictionary()
{
	auto end_path = get_config_string(
		"resources", "texture_dictionary", "textures.dat");

	boost::filesystem::path texture_dictionary_path(this->path);;
	texture_dictionary_path.append(end_path);

	std::ifstream stream(texture_dictionary_path.string(), std::ios::binary);
	mantorok::DATReader reader(stream);
	mantorok::DATDeserializer<chomsky::TextureDictionary>::deserialize(
		this->texture_dictionary,
		reader);
}

void glooper::ResourceManager::load_map()
{
	auto end_path = get_config_string(
		"resources", "map", "map");

	boost::filesystem::path map_path(this->path);;
	map_path.append(end_path);

	std::ifstream stream(map_path.string());
	this->map.clear();
	this->map.load(stream);

	this->runtime_map.clear();
	this->runtime_map.merge(this->map);
}

void glooper::ResourceManager::load_special_items()
{
	auto items_path = get_config_string("resources", "items", "items");

	boost::filesystem::path path(this->path);
	path.append(items_path);

	for_each_file(
		path.string(),
		[this](auto& filename)
		{
			std::ifstream stream(filename, std::ios::binary);
			mantorok::DATReader reader(stream);

			chomsky::ItemIconSilhouette silhouette;
			mantorok::DATDeserializer<chomsky::ItemIconSilhouette>::deserialize(
				silhouette, reader);

			if (!stream.bad())
			{
				this->item_icon_manager.add_special(silhouette.to_string(), silhouette);
			}
		});
}
