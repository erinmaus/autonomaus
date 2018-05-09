// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "thoreau/thoreau.hpp"

const glm::uvec3 chomsky::TileSpacePointCloud::GRID_SIZE = glm::uvec3(15, 255, 15);
const glm::uvec3 chomsky::TileSpacePointCloud::COLOR_SIZE = glm::uvec3(31, 31, 31);
const glm::uvec3 chomsky::TileSpacePointCloud::GRID_INPUT_COMPONENT_BITS = glm::uvec3(9, 16, 9);
const glm::uvec3 chomsky::TileSpacePointCloud::COLOR_INPUT_COMPONENT_BITS = glm::uvec3(8, 8, 8);
const glm::uvec3 chomsky::TileSpacePointCloud::GRID_COMPONENT_BITS = glm::uvec3(4, 8, 4);
const glm::uvec3 chomsky::TileSpacePointCloud::GRID_COMPONENT_MASKS = glm::uvec3(0xf, 0xff, 0xf);
const glm::uvec3 chomsky::TileSpacePointCloud::COLOR_COMPONENT_BITS = glm::uvec3(5, 5, 5);
const glm::uvec3 chomsky::TileSpacePointCloud::COLOR_COMPONENT_MASKS = glm::uvec3(0x1f, 0x1f, 0x1f);
const std::uint32_t chomsky::TileSpacePointCloud::POSITION_BITS = 16;
const std::uint32_t chomsky::TileSpacePointCloud::COLOR_BITS = 15;

bool chomsky::TileSpacePointCloud::add(
	const glm::vec3& position, const glm::vec3& color, Tag tag)
{
	PackedPoint point;
	if (try_pack(position, color, point))
	{
		this->points.insert(point);

		if (tag != TAG_NONE)
		{
			auto& point_tags = this->tags[point];
			point_tags.insert(tag);
		}

		return true;
	}

	return false;
}

void chomsky::TileSpacePointCloud::remove_all(Tag tag)
{
	for (auto i = this->points.begin(); i != this->points.end(); /* Nil */)
	{
		auto point = *i;
		auto point_tags = this->tags.find(point);

		bool remove = false;
		if (point_tags != this->tags.end())
		{
			if (point_tags->second.count(tag) != 0)
			{
				point_tags->second.erase(tag);
				if (point_tags->second.empty())
				{
					remove = true;
				}
			}
		}

		if (remove)
		{
			i = this->points.erase(i);
			this->tags.erase(point);
		}
		else
		{
			++i;
		}
	}
}

bool chomsky::TileSpacePointCloud::has(
	const glm::vec3& position, const glm::vec3& color) const
{
	PackedPoint point;
	if (!try_pack(position, color, point))
	{
		return false;
	}

	return this->points.find(point) != this->points.end();
}

bool chomsky::TileSpacePointCloud::contains(
	const TileSpacePointCloud& other) const
{
	for (auto point: other.points)
	{
		if (this->points.count(point) == 0)
		{
			return false;
		}
	}

	return true;
}

boost::container::flat_set<chomsky::TileSpacePointCloud::Tag>
chomsky::TileSpacePointCloud::match(const TileSpacePointCloud& other) const
{
	boost::container::flat_set<Tag> matches;
	for (auto point: other.points)
	{
		auto tags = this->tags.find(point);
		if (tags != this->tags.end())
		{
			for (auto tag: tags->second)
			{
				matches.insert(tag);
			}
		}
	}

	return matches;
}

void chomsky::TileSpacePointCloud::merge(const TileSpacePointCloud& other)
{
	this->points.insert(other.points.begin(), other.points.end());
	this->tags.insert(other.tags.begin(), other.tags.end());
}

std::size_t chomsky::TileSpacePointCloud::count() const
{
	return this->points.size();
}

std::size_t chomsky::TileSpacePointCloud::count(
	const TileSpacePointCloud& other) const
{
	std::size_t num_matches = 0;
	for (auto point: other.points)
	{
		if (this->points.count(point) != 0)
		{
			++num_matches;
		}
	}

	return num_matches;
}

chomsky::TileSpacePointCloud::const_iterator
chomsky::TileSpacePointCloud::begin(Tag tag) const
{
	return const_iterator(this->points.begin(), this->points.end(), this, tag);
}
chomsky::TileSpacePointCloud::const_iterator
chomsky::TileSpacePointCloud::end() const
{
	return const_iterator(this->points.end(), this->points.end(), this, TAG_NONE);
}

bool chomsky::TileSpacePointCloud::try_pack(
	const glm::vec3& position, const glm::vec3& color,
	PackedPoint& result)
{
	auto grid_position = glm::uvec3(position);
	grid_position >>= GRID_INPUT_COMPONENT_BITS - GRID_COMPONENT_BITS;

#ifndef NDEBUG
	if (grid_position.x > GRID_SIZE.x ||
		grid_position.y > GRID_SIZE.y ||
		grid_position.z > GRID_SIZE.z)
	{
		return false;
	}
#endif

	auto scaled_color = glm::uvec3(color * glm::vec3(COLOR_SIZE));

#ifndef NDEBUG
	if (scaled_color.r < 0 || scaled_color.r > COLOR_SIZE.r ||
		scaled_color.g < 0 || scaled_color.g > COLOR_SIZE.g ||
		scaled_color.b < 0 || scaled_color.b > COLOR_SIZE.b)
	{
		return false;
	}
#endif

	PackedPoint packed_position =
		grid_position.x |
		(grid_position.y << GRID_COMPONENT_BITS.x) |
		(grid_position.z << GRID_COMPONENT_BITS.x << GRID_COMPONENT_BITS.y);
	PackedPoint packed_color =
		scaled_color.r |
		(scaled_color.g << COLOR_COMPONENT_BITS.r) |
		(scaled_color.b << COLOR_COMPONENT_BITS.r << COLOR_COMPONENT_BITS.g);

	result = packed_position | (packed_color << POSITION_BITS);
	return true;
}

void chomsky::TileSpacePointCloud::unpack(
	PackedPoint point, glm::vec3& result_position, glm::vec3& result_color)
{
	glm::uvec3 unpacked_position;
	unpacked_position.x = point & GRID_COMPONENT_MASKS.x;
	unpacked_position.y = (point >> GRID_COMPONENT_BITS.x) & GRID_COMPONENT_MASKS.y;
	unpacked_position.z = (point >> GRID_COMPONENT_BITS.x >> GRID_COMPONENT_BITS.y) & GRID_COMPONENT_MASKS.z;
	
	// TODO: Don't hard-code the quantification shifts.
	glm::uvec3 shifted_position = unpacked_position;
	shifted_position.x = (unpacked_position.x << 5)
		| (unpacked_position.x << 1)
		| (unpacked_position.x >> 3);
	shifted_position.y = (unpacked_position.y << 8) | unpacked_position.y;
	shifted_position.z = (unpacked_position.z << 5)
		| (unpacked_position.z << 1)
		| (unpacked_position.z >> 3);

	point >>= POSITION_BITS;
	glm::uvec3 unpacked_color;
	unpacked_color.x = point & COLOR_COMPONENT_MASKS.x;
	unpacked_color.y = (point >> COLOR_COMPONENT_BITS.x) & COLOR_COMPONENT_MASKS.y;
	unpacked_color.z = (point >> COLOR_COMPONENT_BITS.x >> COLOR_COMPONENT_BITS.y) & GRID_COMPONENT_MASKS.z;

	result_position = shifted_position;
	result_color = unpacked_color;
	result_color /= COLOR_SIZE;
}

chomsky::TileSpacePointCloud::const_iterator&
chomsky::TileSpacePointCloud::const_iterator::operator ++()
{
	while (this->current != this->end)
	{
		++this->current;

		if (this->tag == TAG_NONE)
		{
			break;
		}
		else
		{
			auto tags = this->point_cloud->tags.find(*this->current);
			if (tags->second.find(this->tag) != tags->second.end())
			{
				break;
			}
		}
	}

	return *this;
}

chomsky::TileSpacePointCloud::const_iterator
chomsky::TileSpacePointCloud::const_iterator::operator ++(int)
{
	auto previous = *this;

	++(*this);

	return previous;
}

bool chomsky::TileSpacePointCloud::const_iterator::operator ==(
	const const_iterator& other) const
{
	return this->current == other.current &&
		this->end == other.end &&
		this->point_cloud == other.point_cloud &&
		(this->tag == other.tag || this->tag == TAG_NONE || other.tag == TAG_NONE);
}

bool chomsky::TileSpacePointCloud::const_iterator::operator !=(
	const const_iterator& other) const
{
	return !(*this == other);
}

chomsky::TileSpacePointCloud::const_iterator::value_type
chomsky::TileSpacePointCloud::const_iterator::operator *() const
{
	assert(this->current != this->end);

	Point result;
	unpack(*this->current, result.position, result.color);

	return result;
}

chomsky::TileSpacePointCloud::const_iterator::const_iterator(
	const PointCollection::const_iterator& begin,
	const PointCollection::const_iterator& end,
	const TileSpacePointCloud* point_cloud,
	Tag tag)
{
	this->current = begin;
	this->end = end;
	this->point_cloud = point_cloud;
	this->tag = tag;
}
