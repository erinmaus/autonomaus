// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/events/textureEvent.hpp"
#include "kompvter/graphics/textureManager.hpp"

bool kompvter::TextureManager::has_texture(int name) const
{
	return this->textures.count(name) != 0;
}

kompvter::Texture kompvter::TextureManager::get_texture(int name) const
{
	auto iter = this->textures.find(name);
	if (iter == this->textures.end())
	{
		return Texture();
	}

	return iter->second;
}

void kompvter::TextureManager::reset()
{
	this->textures.clear();
}

kompvter::TextureManager::const_iterator kompvter::TextureManager::begin() const
{
	return const_iterator(this->textures.begin());
}

kompvter::TextureManager::const_iterator kompvter::TextureManager::end() const
{
	return const_iterator(this->textures.end());
}

void kompvter::TextureManager::on_texture_create(const TextureEvent& event)
{
	// Nothing.
}

void kompvter::TextureManager::on_texture_delete(const TextureEvent& event)
{
	auto iter = this->textures.find(event.get_name());
	if (iter != this->textures.end())
	{
		this->textures.erase(iter);
	}
}

void kompvter::TextureManager::on_texture_allocate(const TextureEvent& event)
{
	int pixel_format;
	if (event.get_is_compressed())
	{
		pixel_format = event.get_compression();
	}
	else
	{
		pixel_format = event.get_component_format();
	}

	this->textures[event.get_name()] = std::make_shared<Texture::Data>(
		event.get_name(),
		event.get_width(), event.get_height(),
		pixel_format, event.get_component_type(), event.get_is_compressed());
}

void kompvter::TextureManager::on_texture_update(const TextureEvent& event)
{
	if (!has_texture(event.get_name()))
	{
		return;
	}

	int pixel_format, pixel_type;
	if (event.get_is_compressed())
	{
		pixel_format = event.get_compression();
		pixel_type = 0;
	}
	else
	{
		pixel_format = event.get_component_format();
		pixel_type = event.get_component_type();
	}

	auto iter = this->textures.find(event.get_name());
	if (iter->second.use_count() != 1)
	{
		iter->second = std::make_shared<Texture::Data>(*iter->second);
	}

	Texture texture(iter->second);
	texture.add_region(
		event.get_x(), event.get_y(),
		event.get_width(), event.get_height(),
		pixel_format, pixel_type, event.get_is_compressed(),
		event.get_pixels_data(), event.get_pixels_data_size());
}

void kompvter::TextureManager::on_texture_copy(const TextureEvent& event)
{
	if (!has_texture(event.get_name()))
	{
		return;
	}

	auto iter = this->textures.find(event.get_name());
	if (iter->second.use_count() != 1)
	{
		iter->second = std::make_shared<Texture::Data>(*iter->second);
	}

	Texture texture(iter->second);
	texture.add_region(
		event.get_destination_x(), event.get_destination_y(),
		event.get_width(), event.get_height(),
		TextureEvent::component_format_none,
		TextureEvent::component_type_none,
		false,
		nullptr, 0);
}

kompvter::TextureManager::const_iterator::const_iterator(const TextureDataMap::const_iterator& other) :
	TextureDataMap::const_iterator(other), current(0, Texture())
{
	// Nothing.
}

kompvter::TextureManager::const_iterator::value_type*
kompvter::TextureManager::const_iterator::operator ->() const
{
	auto& result = operator *();
	return &result;
}

kompvter::TextureManager::const_iterator::value_type&
kompvter::TextureManager::const_iterator::operator *() const
{
	auto& result = TextureDataMap::const_iterator::operator *();
	if (result.first != current.first)
	{
		current = std::make_pair(result.first, Texture(result.second));
	}

	return current;
}
