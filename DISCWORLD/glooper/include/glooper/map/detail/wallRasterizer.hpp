// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_MAP_DETAIL_WALL_RASTERIZER_HPP
#define GLOOPER_MAP_DETAIL_WALL_RASTERIZER_HPP

#include <cassert>
#include <cstring>
#include <functional>
#include <vector>
#include <glm/glm.hpp>
#include "kvre/ui/flatDrawBuffer.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tile.hpp"

namespace glooper { namespace detail
{
	class WallRasterizer
	{
	public:
		static const int NUM_SAMPLES = 4;

		WallRasterizer(int width, int height);
		~WallRasterizer() = default;

		const glm::vec4& get_wall_color() const;
		void set_wall_color(const glm::vec4& value);
		const glm::vec4& get_door_color() const;
		void set_door_color(const glm::vec4& value);

		template <typename Iter>
		void rasterize(
			Iter current, Iter end,
			const kvre::FlatDrawBuffer& buffer,
			const glm::vec2& scale);

		void finalize(
			thoreau::Map& result,
			const glm::ivec2& offset);

	private:
		static bool sign(
			const glm::vec2& a, const glm::vec2& b, const glm::vec2& c);
		void rasterize_triangle(
			const glm::vec2& a, const glm::vec2& b, const glm::vec2& c,
			const glm::vec4& color);
		void set_sample(
			const glm::ivec2& pixel_space,
			const glm::vec4& color);

		void extract_walls();

		struct Tile
		{
			glm::vec4 samples[NUM_SAMPLES][NUM_SAMPLES] = {};
			int flags = 0;
		};
		std::vector<Tile> tiles;
		int width, height;

		glm::vec4 door_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
		glm::vec4 wall_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		bool is_left_wall(const Tile& tile);
		bool is_right_wall(const Tile& tile);
		bool is_top_wall(const Tile& tile);
		bool is_bottom_wall(const Tile& tile);
		bool is_left_door(const Tile& tile);
		bool is_right_door(const Tile& tile);
		bool is_top_door(const Tile& tile);
		bool is_bottom_door(const Tile& tile);
		bool is_diagonal_left(const Tile& tile);
		bool is_diagonal_right(const Tile& tile);
		bool is_door(const Tile& tile);

		Tile& get(int x, int y);
		const Tile& get(int x, int y) const;
	};
} }

template <typename Iter>
void glooper::detail::WallRasterizer::rasterize(
	Iter current, Iter end,
	const kvre::FlatDrawBuffer& buffer,
	const glm::vec2& scale)
{
	assert((end - current) % 3 == 0);

	while (current != end)
	{
		if (buffer.is_textured(*current))
		{
			++current;
			continue;
		}

		glm::vec2 positions[3];
		glm::vec4 color = buffer.get_color(*current);

		for (int i = 0; i < 3; ++i)
		{
			assert(!buffer.is_textured(*current));
			positions[i] = buffer.get_position(*current);
			positions[i] *= scale;
			++current;
		}

		rasterize_triangle(positions[1], positions[0], positions[2], color);
	}
}

#endif
