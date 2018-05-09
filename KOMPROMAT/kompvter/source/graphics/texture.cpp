// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/graphics/texture.hpp"

const int kompvter::Texture::HASH_SPACE_SIZE = 32;

kompvter::Texture::Texture(
	int name,
	int width, int height,
	int pixel_format, int pixel_type, bool is_compressed)
{
	this->data->name = name;
	this->data->width = width;
	this->data->height = height;
	this->data->pixel_format = pixel_format;
	this->data->pixel_type = pixel_type;
	this->data->is_compressed = is_compressed;
}

kompvter::Texture::Texture(const std::shared_ptr<Data>& data) :
	data(data)
{
	// Nothing.
}

int kompvter::Texture::get_name() const
{
	return this->data->name;
}

int kompvter::Texture::get_width() const
{
	return this->data->width;
}

int kompvter::Texture::get_height() const
{
	return this->data->height;
}

int kompvter::Texture::get_pixel_format() const
{
	return this->data->pixel_format;
}

int kompvter::Texture::get_pixel_type() const
{
	return this->data->pixel_type;
}

bool kompvter::Texture::get_is_compressed() const
{
	return this->data->is_compressed;
}

kompvter::Texture::TextureRegionName kompvter::Texture::add_region(
	int x, int y,
	int width, int height,
	int pixel_format, int pixel_type, bool is_compressed,
	const std::uint8_t* pixels_data, std::size_t pixels_size)
{
	assert(x >= 0);
	assert(y >= 0);
	assert(width >= 1);
	assert(height >= 1);
	assert(x + width <= this->data->width);
	assert(y + height <= this->data->height);

	int new_region_name = ++this->data->current_name;
	TextureRegion new_region(
		new_region_name,
		x, y,
		width, height,
		pixel_format, pixel_type, is_compressed,
		pixels_data, pixels_size);

	// Find overlapping regions and invalidate.
	auto iter = this->data->regions->begin();
	while (iter != this->data->regions->end())
	{
		auto& region = iter->second;

		bool overlap_x = !(x >= region.get_x() + region.get_width() || region.get_x() >= x + width);
		bool overlap_y = !(y >= region.get_y() + region.get_height() || region.get_y() >= y + height);

		if (!overlap_x || !overlap_y)
		{
			++iter;
		}
		else
		{
			remove_from_hash_space(iter->second);
			iter = this->data->regions->erase(iter);
		}
	}

	add_to_hash_space(new_region);
	this->data->regions->emplace(new_region_name, std::move(new_region));

	return new_region_name;
}

const kompvter::TextureRegion* kompvter::Texture::get_region_from_name(
	TextureRegionName name) const
{
	auto iter = this->data->regions->find(name);
	if (iter != this->data->regions->end())
	{
		return &iter->second;
	}

	return nullptr;
}

const kompvter::TextureRegion* kompvter::Texture::get_region_from_pixel(
	int x, int y) const
{
	int cell_x = x / HASH_SPACE_SIZE;
	int cell_y = y / HASH_SPACE_SIZE;
	auto cell_key = std::make_pair(cell_x, cell_y);
	auto iter = this->data->hash_space->find(cell_key);
	if (iter != this->data->hash_space->end())
	{
		for (auto name: iter->second)
		{
			assert(this->data->regions->count(name) != 0);
			auto& region = this->data->regions->find(name)->second;
			if (x >= region.get_x() && y >= region.get_y() &&
				x < region.get_x() + region.get_width() &&
				y < region.get_y() + region.get_height())
			{
				return &region;
			}
		}
	}

	return nullptr;
}

const kompvter::TextureRegion* kompvter::Texture::get_region_from_texture_coordinate(
	float s, float t) const
{
	int x = s * this->data->width;
	int y = t * this->data->height;

	return get_region_from_pixel(x, y);
}

kompvter::Texture::const_iterator kompvter::Texture::begin() const
{
	return const_iterator(this->data->regions->begin());
}

kompvter::Texture::const_iterator kompvter::Texture::end() const
{
	return const_iterator(this->data->regions->end());
}

void kompvter::Texture::add_to_hash_space(TextureRegion& region)
{
	int min_x = region.get_x() / HASH_SPACE_SIZE;
	int min_y = region.get_y() / HASH_SPACE_SIZE;

	// a little bit inefficient, since there's a good chance it won't overlap
	// the next cell, but I don't care
	int max_x = (region.get_x() + region.get_width()) / HASH_SPACE_SIZE + 1;
	int max_y = (region.get_y() + region.get_height()) / HASH_SPACE_SIZE + 1;

	for (int i = min_x; i <= max_x; ++i)
	{
		for (int j = min_y; j <= max_y; ++j)
		{
			auto cell_key = std::make_pair(i, j);
			(*this->data->hash_space)[cell_key].insert(region.get_name());
		}
	}
}

void kompvter::Texture::remove_from_hash_space(TextureRegion& region)
{
	int min_x = region.get_x() / HASH_SPACE_SIZE;
	int min_y = region.get_y() / HASH_SPACE_SIZE;
	int max_x = (region.get_x() + region.get_width()) / HASH_SPACE_SIZE + 1;
	int max_y = (region.get_y() + region.get_height()) / HASH_SPACE_SIZE + 1;

	for (int i = min_x; i <= max_x; ++i)
	{
		for (int j = min_y; j <= max_y; ++j)
		{
			auto cell_key = std::make_pair(i, j);
			(*this->data->hash_space)[cell_key].erase(region.get_name());
		}
	}
}

kompvter::Texture::Data::Data(
	int name,
	int width, int height,
	int pixel_format, int pixel_type, bool is_compressed)
{
	this->name = name;
	this->width = width;
	this->height = height;
	this->pixel_format = pixel_format;
	this->pixel_type = pixel_type;
	this->is_compressed = is_compressed;
}

kompvter::Texture::const_iterator::const_iterator(
	TextureRegionMap::const_iterator source)
	: TextureRegionMap::const_iterator(std::move(source))
{
	// Nothing.
}

const kompvter::Texture::const_iterator::value_type& kompvter::Texture::const_iterator::operator *() const
{
	auto& value = TextureRegionMap::const_iterator::operator *();
	return value.second;
}

const kompvter::Texture::const_iterator::value_type& kompvter::Texture::const_iterator::operator ->() const
{
	auto& value = TextureRegionMap::const_iterator::operator *();
	return value.second;
}
