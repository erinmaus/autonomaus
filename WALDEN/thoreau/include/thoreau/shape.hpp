// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef THOREAU_SHAPE_HPP
#define THOREAU_SHAPE_HPP

#include <cstdint>
#include <vector>
#include "thoreau/tileCoordinate.hpp"

namespace thoreau
{
	class Shape
	{
	public:
		Shape(int layer = 0);
		~Shape() = default;

		int get_layer() const;

		std::size_t get_num_points() const;
		TileCoordinate get_point(std::size_t index) const;
		void add_point(const TileCoordinate& point);

		TileCoordinate get_min() const;
		TileCoordinate get_max() const;

		bool inside(const TileCoordinate& point) const;

		static Shape make_rectangle(const TileCoordinate& a, const TileCoordinate& b);
		static Shape make_circle(const TileCoordinate& center, int radius);

		std::vector<std::uint8_t> serialize() const;
		bool deserialize(const std::vector<std::uint8_t>& input);

	private:
		int layer;
		std::vector<thoreau::TileCoordinate> points;
		TileCoordinate min, max;
	};
}

#endif
