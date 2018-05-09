// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "chomsky/resources/silhouette.hpp"

chomsky::Silhouette::Silhouette(int width, int height, int depth)
{
	assert(width >= 1);
	assert(height >= 1);
	assert(depth >= 1);

	this->width = width;
	this->height = height;
	this->depth = depth;
	this->fragments = std::make_unique<Fragment[]>(width * height * depth);
}

chomsky::Silhouette::Silhouette(const Silhouette& other)
{
	*this = other;
}

chomsky::Silhouette::Silhouette(Silhouette&& other)
{
	*this = other;
}

int chomsky::Silhouette::get_width() const
{
	return this->width;
}

int chomsky::Silhouette::get_height() const
{
	return this->height;
}

int chomsky::Silhouette::get_depth() const
{
	return this->depth;
}

chomsky::Silhouette::Fragment chomsky::Silhouette::get_fragment(
	int x, int y, int z) const
{
	assert(x >= 0);
	assert(x < this->width);
	assert(y >= 0);
	assert(y < this->height);
	assert(z >= 0);
	assert(z < this->depth);

	auto p = this->fragments.get();
	p += (this->width * this->height) * z;
	return p[y * this->width + x];
}

void chomsky::Silhouette::set_fragment(
	int x, int y, int z, Fragment value)
{
	assert(x >= 0);
	assert(x < this->width);
	assert(y >= 0);
	assert(y < this->height);
	assert(z >= 0);
	assert(z < this->depth);

	auto p = this->fragments.get();
	p += (this->width * this->height) * z;
	p[y * this->width + x] = value;
}

void chomsky::Silhouette::merge(const chomsky::Silhouette& other)
{
	if (this->width != other.width)
	{
		return;
	}
	if (this->height != other.height)
	{
		return;
	}
	if (this->depth != other.depth)
	{
		return;
	}

	for (int z = 0; z < this->depth; ++z)
	{
		auto plane_a = &this->fragments[(this->width * this->height) * z];
		auto plane_b = &other.fragments[(other.width * other.height) * z];

		for (int y = 0; y < this->height; ++y)
		{
			auto row_a = plane_a + y * this->width;
			auto row_b = plane_b + y * other.width;

			for (int x = 0; x < this->depth; ++x)
			{
				row_a[x] += row_b[x];
			}
		}
	}
}

void chomsky::Silhouette::clear(Fragment value)
{
	for (int z = 0; z < this->depth; ++z)
	{
		auto plane = &this->fragments[(this->width * this->height) * z];

		for (int y = 0; y < this->height; ++y)
		{
			auto row = plane + y * this->width;

			for (int x = 0; x < this->depth; ++x)
			{
				row[x] = value;
			}
		}
	}
}

bool chomsky::Silhouette::match(
	const Silhouette& a, int a_x, int a_y, int a_z,
	const Silhouette& b, int b_x, int b_y, int b_z,
	int width, int height, int depth,
	const CompareFunction& compare)
{
	assert(a_x >= 0);
	assert(a_x < a.width);
	assert(a_y >= 0);
	assert(a_y < a.width);
	assert(a_z >= 0);
	assert(a_z < a.depth);
	assert(b_x >= 0);
	assert(b_x < b.width);
	assert(b_y >= 0);
	assert(b_y < b.width);
	assert(b_z >= 0);
	assert(b_z < b.depth);
	assert(width > 0);
	assert(a_x + width < a.width);
	assert(b_x + width < b.width);
	assert(height > 0);
	assert(a_y + height < a.height);
	assert(b_y + height < b.height);
	assert(depth > 0);
	assert(a_z + depth < a.depth);
	assert(b_z + depth < b.depth);

	for (int z = 0; z < depth; ++z)
	{
		auto plane_a = &a.fragments[(a.width * a.height) * (z + a_z)];
		auto plane_b = &b.fragments[(b.width * b.height) * (z + b_z)];

		for (int y = 0; y < height; ++y)
		{
			auto row_a = plane_a + (y + a_y) * a.width;
			auto row_b = plane_b + (y + b_y) * b.width;

			for (int x = 0; x < width; ++x)
			{
				auto fragment_a = row_a[x + a_x];
				auto fragment_b = row_b[x + b_x];

				if (!compare(fragment_a, fragment_b, x, y, z))
				{
					return false;
				}
			}
		}
	}

	return true;
}

bool chomsky::Silhouette::match(
	const Silhouette& a, const Silhouette& b,
	const CompareFunction& compare)
{
	if (a.width != b.width)
	{
		return false;
	}

	if (a.height != b.height)
	{
		return false;
	}

	if (a.depth != b.depth)
	{
		return false;
	}

	return match(a, 0, 0, 0, b, 0, 0, 0, a.width, a.height, a.depth, compare);
}

chomsky::Silhouette& chomsky::Silhouette::operator =(const Silhouette& other)
{
	assert(other.width >= 0);
	assert(other.height >= 0);
	assert(other.depth >= 0);

	this->width = other.width;
	this->height = other.height;
	this->depth = other.depth;

	if (other.width > 0 && other.height > 0)
	{
		auto num_fragments = this->width * this->height * this->depth;
		this->fragments = std::make_unique<Fragment[]>(num_fragments);
		for (int i = 0; i < num_fragments; ++i)
		{
			this->fragments[i] = other.fragments[i];
		}
	}
	else
	{
		this->fragments.reset();
	}

	return *this;
}

chomsky::Silhouette& chomsky::Silhouette::operator =(Silhouette&& other)
{
	assert(other.width >= 0);
	assert(other.height >= 0);
	assert(other.depth >= 0);

	this->width = other.width;
	this->height = other.height;
	this->depth = other.depth;

	other.width = 0;
	other.height = 0;
	other.depth = 0;

	this->fragments = std::move(other.fragments);

	return *this;
}
