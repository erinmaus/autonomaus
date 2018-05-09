// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include "glooper/map/detail/wallRasterizer.hpp"

const int glooper::detail::WallRasterizer::NUM_SAMPLES;

glooper::detail::WallRasterizer::WallRasterizer(int width, int height)
{
	this->tiles.resize(width * height);
	this->width = width;
	this->height = height;
}

const glm::vec4& glooper::detail::WallRasterizer::get_wall_color() const
{
	return this->wall_color;
}

void glooper::detail::WallRasterizer::set_wall_color(const glm::vec4& value)
{
	this->wall_color = value;
}

const glm::vec4& glooper::detail::WallRasterizer::get_door_color() const
{
	return this->door_color;
}

void glooper::detail::WallRasterizer::set_door_color(const glm::vec4& value)
{
	this->door_color = value;
}

void glooper::detail::WallRasterizer::finalize(
	thoreau::Map& map,
	const glm::ivec2& offset)
{
	extract_walls();

	auto iter = this->tiles.begin();
	for (int y = 0; y < this->height; ++y)
	{
		for (int x = 0; x < this->height; ++x)
		{
			auto& tile = map.add_tile(x + offset.x, y + offset.y, 0);
			tile.append_flags(iter->flags);

			++iter;
		}
	}
}

bool glooper::detail::WallRasterizer::sign(
	const glm::vec2& a, const glm::vec2& b, const glm::vec2& c)
{
	return (a.x - c.x) * (b.y - c.y) - (b.x - c.x) * (a.y - c.y) < 0.0f;
}

void glooper::detail::WallRasterizer::rasterize_triangle(
	const glm::vec2& a, const glm::vec2& b, const glm::vec2& c,
	const glm::vec4& color)
{
	glm::vec2 s = a * (float)NUM_SAMPLES;
	glm::vec2 t = b * (float)NUM_SAMPLES;
	glm::vec2 p = c * (float)NUM_SAMPLES;

	float min_x = std::min({ s.x, t.x, p.x });
	float max_x = std::max({ s.x, t.x, p.x });
	float min_y = std::min({ s.y, t.y, p.y });
	float max_y = std::max({ s.y, t.y, p.y });

	for (float y = min_y; y <= max_y; y += 1.0f)
	{
		for (float x = min_x; x <= max_x; x += 1.0f)
		{
			glm::vec2 point(x, y);

			auto i = sign(point, s, t);
			auto j = sign(point, t, p);
			auto k = sign(point, p, s);

			if (i == j && i == k)
			{
				set_sample(glm::ivec2(x, y), color);
			}
		}
	}
}

void glooper::detail::WallRasterizer::set_sample(
	const glm::ivec2& pixel_space, const glm::vec4& color)
{
	int tile_x = pixel_space.x / NUM_SAMPLES;
	int tile_y = pixel_space.y / NUM_SAMPLES;

	if (tile_x >= this->width || tile_y >= this->height)
	{
		return;
	}

	auto& tile = get(tile_x, tile_y);
	int sample_x = pixel_space.x - tile_x * NUM_SAMPLES;
	int sample_y = pixel_space.y - tile_y * NUM_SAMPLES;
	tile.samples[sample_x][sample_y] = color;
}

void glooper::detail::WallRasterizer::extract_walls()
{
	auto iter = this->tiles.begin();
	for (int y = 0; y < this->height; ++y)
	{
		for (int x = 0; x < this->width; ++x)
		{
			bool is_vertical_wall = false;
			if (is_left_door(*iter))
			{
				iter->flags |= thoreau::Tile::flag_door_left;
				is_vertical_wall = true;
			}
			else if (is_left_wall(*iter))
			{
				iter->flags |= thoreau::Tile::flag_wall_left;
				is_vertical_wall = true;
			}

			if (is_right_door(*iter))
			{
				iter->flags |= thoreau::Tile::flag_door_right;
				is_vertical_wall = true;
			}
			else if (is_right_wall(*iter))
			{
				iter->flags |= thoreau::Tile::flag_wall_right;
				is_vertical_wall = true;
			}

			bool is_horizontal_wall = false;
			if (is_top_door(*iter))
			{
				iter->flags |= thoreau::Tile::flag_door_top;
				is_horizontal_wall = true;
			}
			else if (is_top_wall(*iter))
			{
				iter->flags |= thoreau::Tile::flag_wall_top;
				is_horizontal_wall = true;
			}

			if (is_bottom_door(*iter))
			{
				iter->flags |= thoreau::Tile::flag_door_bottom;
				is_horizontal_wall = true;
			}
			else if (is_bottom_wall(*iter))
			{
				iter->flags |= thoreau::Tile::flag_wall_bottom;
				is_horizontal_wall = true;
			}

			if (!is_vertical_wall && !is_horizontal_wall)
			{
				bool is_left = is_diagonal_left(*iter);
				bool is_right = is_diagonal_right(*iter);
				if (is_left || is_right)
				{
					iter->flags |= thoreau::Tile::flag_impassable;
					iter->flags |= thoreau::Tile::flag_wall_diagonal;

					if (is_left)
					{
						iter->flags |= thoreau::Tile::flag_wall_diagonal_left;
					}
					else if (is_right)
					{
						iter->flags |= thoreau::Tile::flag_wall_diagonal_right;
					}
				}
			}

			++iter;
		}
	}
}

bool glooper::detail::WallRasterizer::is_left_wall(const Tile& tile)
{
	for (int y = 0; y < NUM_SAMPLES; ++y)
	{
		if (tile.samples[0][y].a != 1.0f)
		{
			return false;
		}
	}

	return true;
}

bool glooper::detail::WallRasterizer::is_right_wall(const Tile& tile)
{
	for (int y = 0; y < NUM_SAMPLES; ++y)
	{
		if (tile.samples[NUM_SAMPLES - 1][y].a != 1.0f)
		{
			return false;
		}
	}

	return true;
}

bool glooper::detail::WallRasterizer::is_top_wall(const Tile& tile)
{
	for (int x = 0; x < NUM_SAMPLES; ++x)
	{
		if (tile.samples[x][NUM_SAMPLES - 1].a != 1.0f)
		{
			return false;
		}
	}

	return true;
}

bool glooper::detail::WallRasterizer::is_bottom_wall(const Tile& tile)
{
	for (int x = 0; x < NUM_SAMPLES; ++x)
	{
		if (tile.samples[x][0].a != 1.0f)
		{
			return false;
		}
	}

	return true;
}

bool glooper::detail::WallRasterizer::is_left_door(const Tile& tile)
{
	for (int y = 1; y < NUM_SAMPLES - 1; ++y)
	{
		if (tile.samples[0][y] == this->door_color)
		{
			return true;
		}
	}

	return false;
}

bool glooper::detail::WallRasterizer::is_right_door(const Tile& tile)
{
	for (int y = 1; y < NUM_SAMPLES - 1; ++y)
	{
		if (tile.samples[NUM_SAMPLES - 1][y] == this->door_color)
		{
			return true;
		}
	}

	return false;
}

bool glooper::detail::WallRasterizer::is_top_door(const Tile& tile)
{
	for (int x = 1; x < NUM_SAMPLES - 1; ++x)
	{
		if (tile.samples[x][NUM_SAMPLES - 1] == this->door_color)
		{
			return true;
		}
	}

	return false;
}

bool glooper::detail::WallRasterizer::is_bottom_door(const Tile& tile)
{
	for (int x = 1; x < NUM_SAMPLES - 1; ++x)
	{
		if (tile.samples[x][0] == this->door_color)
		{
			return true;
		}
	}

	return false;
}

bool glooper::detail::WallRasterizer::is_diagonal_left(const Tile& tile)
{
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		if (tile.samples[i][i].a == 0.0f)
		{
			return false;
		}
	}

	return true;
}

bool glooper::detail::WallRasterizer::is_diagonal_right(const Tile& tile)
{
	for (int i = 0; i < NUM_SAMPLES; ++i)
	{
		if (tile.samples[i][NUM_SAMPLES - i - 1].a == 0.0f)
		{
			return false;
		}
	}

	return true;
}

glooper::detail::WallRasterizer::Tile&
glooper::detail::WallRasterizer::get(int x, int y)
{
	assert(x >= 0);
	assert(x < this->width);
	assert(y >= 0);
	assert(y < this->height);

	return this->tiles.at(y * this->width + x);
}

const glooper::detail::WallRasterizer::Tile&
glooper::detail::WallRasterizer::get(int x, int y) const
{
	assert(x >= 0);
	assert(x < this->width);
	assert(y >= 0);
	assert(y < this->height);

	return this->tiles.at(y * this->height + x);
}
