// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <algorithm>
#include <sstream>
#include <set>
#include "thoreau/shape.hpp"

thoreau::Shape::Shape(int layer)
{
	this->layer = layer;
}

int thoreau::Shape::get_layer() const
{
	return this->layer;
}

std::size_t thoreau::Shape::get_num_points() const
{
	return this->points.size();
}

thoreau::TileCoordinate thoreau::Shape::get_point(std::size_t index) const
{
	if (index < this->points.size())
	{
		return this->points[index];
	}

	return TileCoordinate(0, 0);
}

thoreau::TileCoordinate thoreau::Shape::get_min() const
{
	return this->min;
}

thoreau::TileCoordinate thoreau::Shape::get_max() const
{
	return this->max;
}

void thoreau::Shape::add_point(const TileCoordinate& point)
{
	auto new_point = point;
	new_point.layer = this->layer;
	this->points.push_back(new_point);

	if (this->points.size() == 1)
	{
		this->min = new_point;
		this->max = new_point;
	}
	else
	{
		this->min.x = std::min(this->min.x, new_point.x);
		this->min.y = std::min(this->min.y, new_point.y);
		this->max.x = std::max(this->max.x, new_point.x);
		this->max.y = std::max(this->max.y, new_point.y);
	}
}

bool thoreau::Shape::inside(const TileCoordinate& point) const
{
	if (point.layer != this->layer)
	{
		return false;
	}

	if (this->points.size() == 1)
	{
		return this->points[0] == point;
	}

	int crosses = 0;
	for (std::size_t i = 0; i < this->points.size(); ++i)
	{
		auto a = this->points[i];
		auto b = this->points[(i + 1) % this->points.size()];

		if ((a.y <= point.y && b.y > point.y) ||
			(a.y > point.y && b.y <= point.y))
		{
			float v = (float)(point.y - a.y) / (float)(b.y - a.y);
			float u = a.x + v * (b.x - a.x);
			if (point.x < u)
			{
				++crosses;
			}
		}
	}

	return (crosses % 2) == 1;
}

thoreau::Shape thoreau::Shape::make_rectangle(
	const TileCoordinate& a, const TileCoordinate& b)
{
	if (a.layer != b.layer)
	{
		return Shape(0);
	}

	Shape shape(a.layer);
	int min_x = std::min(a.x, b.x);
	int max_x = std::max(a.x, b.x);
	int min_y = std::min(a.y, b.y);
	int max_y = std::max(a.y, b.y);

	shape.add_point(thoreau::TileCoordinate(min_x, min_y));
	shape.add_point(thoreau::TileCoordinate(max_x, min_y));
	shape.add_point(thoreau::TileCoordinate(max_x, max_y));
	shape.add_point(thoreau::TileCoordinate(min_x, max_y));

	return shape;
}

thoreau::Shape thoreau::Shape::make_circle(
	const TileCoordinate& center, int radius)
{
	std::set<TileCoordinate> tiles;

	Shape shape(center.layer);

	int x = radius - 1, y = 0;
	int dx = 1, dy = 1;
	int error = dx - (radius << 1);

	auto put_tile = [&](int offset_x, int offset_y)
	{
		auto tile_coordinate = center;
		tile_coordinate.x += offset_x;
		tile_coordinate.y += offset_y;

		if (tiles.count(tile_coordinate) == 0)
		{
			tiles.insert(tile_coordinate);
			shape.add_point(tile_coordinate);
		}
	};

	while (x >= y)
	{
		put_tile(+x, +y);
		put_tile(+y, +x);
		put_tile(-y, +x);
		put_tile(-x, +y);
		put_tile(-x, -y);
		put_tile(-y, -x);
		put_tile(+y, -x);
		put_tile(+x, -y);

		if (error <= 0)
		{
			++y;
			error += dy;
			dy += 2;
		}

		if (error > 0)
		{
			--x;
			dx += 2;
			error += (-radius << 1) + dx;
		}
	}

	std::sort(
		shape.points.begin(), shape.points.end(),
		[center](auto a, auto b)
		{
			float a_x = center.x - a.x;
			float a_y = center.y - a.y;
			float a_angle = std::atan2(a_y, a_x);
			if (a_angle < 0.0f)
			{
				a_angle += M_PI * 2.0f;;
			}

			float b_x = center.x - b.x;
			float b_y = center.y - b.y;
			float b_angle = std::atan2(b_y, b_x);
			if (b_angle < 0.0f)
			{
				b_angle += M_PI * 2.0f;;
			}

			return a_angle < b_angle;
		});

	return shape;
}

std::vector<std::uint8_t> thoreau::Shape::serialize() const
{
	std::stringstream stream;

	std::uint32_t c = this->points.size();
	stream.write((const char*)&c, sizeof(std::uint32_t));

	std::int32_t l = this->layer;
	stream.write((const char*)&l, sizeof(std::int32_t));

	for (auto point: points)
	{
		std::int32_t x = point.x;
		stream.write((const char*)&x, sizeof(std::int32_t));

		std::int32_t y = point.y;
		stream.write((const char*)&y, sizeof(std::int32_t));
	}

	auto size = stream.tellp();
	std::vector<std::uint8_t> result;
	result.resize(size);

	stream.seekg(0);
	stream.read((char*)&result[0], size);

	return result;
}

bool thoreau::Shape::deserialize(const std::vector<std::uint8_t>& input)
{
	Shape shape;

	std::stringstream stream;
	stream.write((const char*)&input[0], input.size());
	stream.seekg(0);

	std::uint32_t c;
	stream.read((char*)&c, sizeof(std::uint32_t));

	if (stream.fail())
	{
		return false;
	}

	std::int32_t l;
	stream.read((char*)&l, sizeof(std::int32_t));
	if (stream.fail() || l < TileCoordinate::MIN_LAYER || l > TileCoordinate::MAX_LAYER)
	{
		return false;
	}
	shape.layer = l;

	for (std::uint32_t i = 0; i < c; ++i)
	{
		if (stream.eof())
		{
			return false;
		}

		std::int32_t x;
		stream.read((char*)&x, sizeof(std::int32_t));
		if (x < TileCoordinate::MIN_COORDINATE || x > TileCoordinate::MAX_COORDINATE)
		{
			return false;
		}

		std::int32_t y;
		stream.read((char*)&y, sizeof(std::int32_t));
		if (y < TileCoordinate::MIN_COORDINATE || y > TileCoordinate::MAX_COORDINATE)
		{
			return false;
		}

		if (stream.bad())
		{
			return false;
		}

		shape.add_point(TileCoordinate(x, y));
	}

	*this = shape;

	return true;
}
