// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvncer/render/gl.hpp"
#include "principle/resources/textureManager.hpp"

void principle::TextureManager::reset()
{
	this->textures.clear();
}

void principle::TextureManager::on_texture_delete(
	const kompvter::TextureEvent& event)
{
	this->textures.erase(event.get_name());
}

void principle::TextureManager::on_texture_allocate(
	const kompvter::TextureEvent& event)
{
	this->textures[event.get_name()] = std::make_unique<kvncer::Texture>(
		event.get_component_format(), event.get_is_compressed(),
		event.get_width(), event.get_height());
}

void principle::TextureManager::on_texture_update(
	const kompvter::TextureEvent& event)
{
	auto iter = this->textures.find(event.get_name());
	if (iter != this->textures.end())
	{
		if (event.get_is_compressed())
		{
			iter->second->compressed_update(
				event.get_x(), event.get_y(),
				event.get_width(), event.get_height(),
				event.get_compression(),
				event.get_pixels_data(), event.get_pixels_data_size());
		}
		else
		{
			iter->second->update(
				event.get_x(), event.get_y(),
				event.get_width(), event.get_height(),
				event.get_component_format(), event.get_component_type(),
				event.get_pixels_data());
		}
	}
}

void principle::TextureManager::on_texture_copy(const kompvter::TextureEvent& event)
{
	auto source_texture = this->textures.find(event.get_source_name());
	auto destination_texture = this->textures.find(event.get_destination_name());

	if (source_texture == this->textures.end() ||
		destination_texture == this->textures.end())
	{
		return;
	}

	GLuint source_name = source_texture->second->get_texture();
	GLuint destination_name = destination_texture->second->get_texture();

	glCopyImageSubData(
		source_name, GL_TEXTURE_2D, 0,
		event.get_source_x(), event.get_source_y(), 0,
		destination_name, GL_TEXTURE_2D, 0,
		event.get_destination_x(), event.get_destination_y(), 0,
		event.get_width(), event.get_height(), 1);
}

kvncer::Texture* principle::TextureManager::get_texture(int name) const
{
	auto iter = this->textures.find(name);
	if (iter != this->textures.end())
	{
		return iter->second.get();
	}

	return nullptr;
}
