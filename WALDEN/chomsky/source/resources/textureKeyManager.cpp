// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/resources/textureKeyManager.hpp"

chomsky::TextureKeyManager::TextureKeyManager(
	const kompvter::TextureManager& texture_manager,
	const TextureFingerprintManager& texture_fingerprint_manager,
	const TextureDictionary& texture_dictionary)
{
	this->texture_manager = &texture_manager;
	this->texture_fingerprint_manager = &texture_fingerprint_manager;
	this->texture_dictionary = &texture_dictionary;
}

float chomsky::TextureKeyManager::get_max_difference() const
{
	return this->max_difference;
}

void chomsky::TextureKeyManager::set_max_difference(float value)
{
	this->max_difference = value;
}

float chomsky::TextureKeyManager::get_max_dissimilarity() const
{
	return this->max_dissimilarity;
}

void chomsky::TextureKeyManager::set_max_dissimilarity(float value)
{
	this->max_dissimilarity = value;
}

void chomsky::TextureKeyManager::on_texture_delete(
	const kompvter::TextureEvent& event)
{
	this->updates.erase(event.get_name());
}

void chomsky::TextureKeyManager::on_texture_update(
	const kompvter::TextureEvent& event)
{
	int texture_name = event.get_name();
	auto texture = this->texture_manager->get_texture(texture_name);
	auto region = texture.get_region_from_pixel(event.get_x(), event.get_y());

	assert(region != nullptr);
	add_pending(
		event.get_destination_name(),
		event.get_destination_x(), event.get_destination_y(),
		event.get_width(), event.get_height());
}

void chomsky::TextureKeyManager::on_texture_copy(
	const kompvter::TextureEvent& event)
{
	remove_any(
		event.get_destination_name(),
		event.get_destination_x(), event.get_destination_y(),
		event.get_width(), event.get_height());
}

void chomsky::TextureKeyManager::reset()
{
	this->updates.clear();
}

std::string chomsky::TextureKeyManager::get(
	int texture_name, int region_name) const
{
	if (this->updates.count(texture_name) != 0)
	{
		auto texture_updates = this->updates.find(texture_name);
		assert(texture_updates != this->updates.end());

		auto region_update = texture_updates->second.find(region_name);
		if (region_update != texture_updates->second.end())
		{
			if (!region_update->second.ready)
			{
				if (!update_pending(
					texture_name, region_name,
					region_update->second.key))
				{
					return std::string();
				}

				region_update->second.ready = true;
			}

			return region_update->second.key;
		}
	}

	return std::string();
}

void chomsky::TextureKeyManager::add(
	int texture_name,
	int x, int y, int width, int height,
	const std::string& key)
{
	remove_any(texture_name, x, y, width, height);

	TextureUpdate update;
	update.x = x;
	update.y = y;
	update.width = width;
	update.height = height;
	update.ready = true;
	update.key = key;

	auto& texture_updates = this->updates[texture_name];
	auto texture = this->texture_manager->get_texture(texture_name);
	auto region = texture.get_region_from_pixel(x, y);

	assert(region != nullptr);
	assert(texture_updates.count(region->get_name()) == 0);

	texture_updates.emplace(region->get_name(), update);
}

void chomsky::TextureKeyManager::add(
	int texture_name, int region_name,
	const std::string& key)
{
	auto texture = this->texture_manager->get_texture(texture_name);
	auto region = texture.get_region_from_name(region_name);
	assert(region != nullptr);

	TextureUpdate update;
	update.x = region->get_x();
	update.y = region->get_y();
	update.width = region->get_width();
	update.height = region->get_height();
	update.ready = true;
	update.key = key;

	auto& texture_updates = this->updates[texture_name];
	texture_updates[region_name] = update;
}

void chomsky::TextureKeyManager::remove(const std::string& key)
{
	for (auto& i: updates)
	{
		auto& texture = i.second;
		for (auto j = texture.begin(); j != texture.end(); /* Nothing. */ )
		{
			if (j->second.key == key)
			{
				j = texture.erase(j);
			}
			else
			{
				++j;
			}
		}
	}
}

void chomsky::TextureKeyManager::add_pending(
	int texture_name,
	int x, int y, int width, int height)
{
	remove_any(texture_name, x, y, width, height);

	TextureUpdate update;
	update.x = x;
	update.y = y;
	update.width = width;
	update.height = height;

	auto& texture_updates = this->updates[texture_name];
	auto texture = this->texture_manager->get_texture(texture_name);
	auto region = texture.get_region_from_pixel(x, y);

	assert(region != nullptr);
	assert(texture_updates.count(region->get_name()) == 0);
	texture_updates[region->get_name()] = update;
}

bool chomsky::TextureKeyManager::update_pending(
	int texture_name, int region_name, std::string& result) const
{
	if (!this->texture_fingerprint_manager->ready(texture_name, region_name))
	{
		return false;
	}

	auto fingerprint = this->texture_fingerprint_manager->get(
		texture_name, region_name);
	std::string key;
	if (this->texture_dictionary->try_find(
		fingerprint, key, this->max_difference, this->max_dissimilarity))
	{
		result = key;
	}

	return true;
}

void chomsky::TextureKeyManager::remove_any(
	int texture_name, int x, int y, int width, int height)
{
	auto& texture_updates = this->updates[texture_name];
	auto texture = this->texture_manager->get_texture(texture_name);
	for (auto i = texture_updates.begin(); i != texture_updates.end(); /* Nothing. */)
	{
		auto name = i->first;
		auto& update = i->second;
		bool overlap_x = !(x >= update.x + update.width || update.x >= x + width);
		bool overlap_y = !(y >= update.y + update.height || update.y >= y + height);

		if (overlap_x && overlap_y)
		{
			assert(texture.get_region_from_name(name) == nullptr);
			i = texture_updates.erase(i);
		}
		else
		{
			++i;
		}
	}
}
