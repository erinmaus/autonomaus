// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/chomsky.hpp"
#include "chomsky/resources/fontExtentManager.hpp"
#include "kompvter/events/textureEvent.hpp"

bool chomsky::FontExtentManager::has_extents(const std::string& key) const
{
	return this->extents.count(key) != 0;
}

bool chomsky::FontExtentManager::generate_extents(
	const std::string& key, const kompvter::TextureRegion& region)
{
	if (region.get_pixel_format() != kompvter::TextureEvent::component_format_rgba)
	{
		return false;
	}

	if (region.get_pixel_type() != kompvter::TextureEvent::component_type_unsigned_byte)
	{
		return false;
	}

	if (region.get_width() % chomsky::config::FONT_NUM_COLUMNS != 0)
	{
		return false;
	}

	if (region.get_height() % chomsky::config::FONT_NUM_ROWS != 0)
	{
		return false;
	}

	int cell_width = region.get_width() / chomsky::config::FONT_NUM_COLUMNS;
	int cell_height = region.get_height() / chomsky::config::FONT_NUM_ROWS;

	auto& map = extents[key];
	auto pixels = region.get_pixels_data();

	std::uint32_t character = 0;
	std::size_t offset_y = 0;
	for (int j = 0; j < chomsky::config::FONT_NUM_ROWS; ++j)
	{
		std::size_t offset_x = 0;
		for (int i = 0; i < chomsky::config::FONT_NUM_COLUMNS; ++i)
		{
			int min_x = cell_width, max_x = 0;
			int min_y = cell_height, max_y = 0;
			bool is_empty = true;

			for (int y = 0; y < cell_height; ++y)
			{
				for (int x = 0; x < cell_width; ++x)
				{
					std::size_t pixel_x = offset_x + x;
					std::size_t pixel_y = offset_y + y;
					std::size_t component_x = pixel_x * 4;
					std::size_t component_y = pixel_y * 4;
					auto p = pixels + (component_y * region.get_height() + component_x);

					if (p[3] != 0)
					{
						min_x = glm::min(min_x, x);
						min_y = glm::min(min_y, y);
						max_x = glm::max(max_x, x);
						max_y = glm::max(max_y, y);
						is_empty = false;
					}
				}
			}

			std::uint32_t c = character++;
			glm::vec2 position(min_x, min_y);
			glm::vec2 size(max_x - min_x, max_y - min_y);
			map[c] = { position, size };

			offset_x += cell_width;
		}

		offset_y += cell_height;
	}

	return true;
}

glm::vec2 chomsky::FontExtentManager::get_size(
	const std::string& key, std::uint32_t character) const
{
	auto iter = this->extents.find(key);
	if (iter != this->extents.end())
	{
		auto& map = iter->second;
		auto extent = map.find(character);
		if (extent != map.end())
		{
			return extent->second.size;
		}
	}

	return glm::vec2(0.0f);
}

glm::vec2 chomsky::FontExtentManager::get_offset(
	const std::string& key, std::uint32_t character) const
{
	auto iter = this->extents.find(key);
	if (iter != this->extents.end())
	{
		auto& map = iter->second;
		auto extent = map.find(character);
		if (extent != map.end())
		{
			return extent->second.position;
		}
	}

	return glm::vec2(0.0f);
}
