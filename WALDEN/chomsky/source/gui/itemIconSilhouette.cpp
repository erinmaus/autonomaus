// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/resources/modelVertexCloud.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/thoreau.hpp"

const int chomsky::ItemIconSilhouette::WIDTH;
const int chomsky::ItemIconSilhouette::HEIGHT;
const int chomsky::ItemIconSilhouette::DEPTH;

chomsky::ItemIconSilhouette::ItemIconSilhouette() :
	weight(WIDTH, HEIGHT, DEPTH),
	red(WIDTH, HEIGHT, DEPTH),
	green(WIDTH, HEIGHT, DEPTH),
	blue(WIDTH, HEIGHT, DEPTH)
{
	// Nothing.
}

int chomsky::ItemIconSilhouette::get_width() const
{
	return WIDTH;
}

int chomsky::ItemIconSilhouette::get_height() const
{
	return HEIGHT;
}

int chomsky::ItemIconSilhouette::get_depth() const
{
	return DEPTH;
}

const chomsky::Silhouette& chomsky::ItemIconSilhouette::get_weight() const
{
	return this->weight;
}

const chomsky::Silhouette& chomsky::ItemIconSilhouette::get_red() const
{
	return this->red;
}

const chomsky::Silhouette& chomsky::ItemIconSilhouette::get_green() const
{
	return this->green;
}

const chomsky::Silhouette& chomsky::ItemIconSilhouette::get_blue() const
{
	return this->blue;
}

const chomsky::TileSpacePointCloudMatch& chomsky::ItemIconSilhouette::get_point_cloud_match() const
{
	return this->point_cloud_match;
}

void chomsky::ItemIconSilhouette::project(const ModelVertexCloud& cloud)
{
	clear();

	cloud.project_weight(this->weight, glm::vec3(0.0f), cloud.get_bounds());
	cloud.project_color_component(
		this->red,
		glm::vec3(0.0f), cloud.get_bounds(),
		ModelVertexCloud::color_component_red);
	cloud.project_color_component(
		this->green,
		glm::vec3(0.0f), cloud.get_bounds(),
		ModelVertexCloud::color_component_green);
	cloud.project_color_component(
		this->blue,
		glm::vec3(0.0f), cloud.get_bounds(),
		ModelVertexCloud::color_component_blue);

	normalize_colors();
}

void chomsky::ItemIconSilhouette::merge(
	const chomsky::ItemIconSilhouette& other)
{
	for (int z = 0; z < DEPTH; ++z)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			for (int x = 0; x < WIDTH; ++x)
			{
				auto a_weight = this->weight.get_fragment(x, y, z);
				auto b_weight = other.weight.get_fragment(x, y, z);
				auto sum_weight = a_weight + b_weight;

				if (sum_weight < 1.0f)
				{
					continue;
				}

				auto relative_a_weight = 1.0f - (a_weight / sum_weight);
				this->weight.set_fragment(x, y, z, sum_weight);

				auto a_red = this->red.get_fragment(x, y, z);
				auto b_red = other.red.get_fragment(x, y, z);
				this->red.set_fragment(
					x, y, z,
					glm::mix(a_red, b_red, relative_a_weight));
				auto a_green = this->green.get_fragment(x, y, z);
				auto b_green = other.green.get_fragment(x, y, z);
				this->green.set_fragment(
					x, y, z,
					glm::mix(a_green, b_green, relative_a_weight));
				auto a_blue = this->blue.get_fragment(x, y, z);
				auto b_blue = other.blue.get_fragment(x, y, z);
				this->blue.set_fragment(
					x, y, z,
					glm::mix(a_blue, b_blue, relative_a_weight));
			}
		}
	}
}

std::string chomsky::ItemIconSilhouette::to_string() const
{
	std::ostringstream result;
	for (int z = 0; z < DEPTH; ++z)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			for (int x = 0; x < WIDTH; ++x)
			{
				result << "/";

				int weight = this->weight.get_fragment(x, y, z);
				result << weight << ":";

				int red = this->red.get_fragment(x, y, z) * 255;
				result << red << ":";

				int green = this->green.get_fragment(x, y, z) * 255;
				result << green << ":";

				int blue = this->blue.get_fragment(x, y, z) * 255;
				result << blue;
			}
		}
	}

	return result.str();
}

bool chomsky::ItemIconSilhouette::match(
	const ItemIconSilhouette& a, const ItemIconSilhouette& b)
{
	for (int z = 0; z < DEPTH; ++z)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			for (int x = 0; x < WIDTH; ++x)
			{
				int a_weight = a.weight.get_fragment(x, y, z);
				int b_weight = b.weight.get_fragment(x, y, z);
				if (a_weight != b_weight)
				{
					return false;
				}

				int a_red = a.red.get_fragment(x, y, z) * 255;
				int b_red = b.red.get_fragment(x, y, z) * 255;
				if (a_red != b_red)
				{
					return false;
				}

				int a_green = a.green.get_fragment(x, y, z) * 255;
				int b_green = b.green.get_fragment(x, y, z) * 255;
				if (a_green != b_green)
				{
					return false;
				}

				int a_blue = a.blue.get_fragment(x, y, z) * 255;
				int b_blue = b.blue.get_fragment(x, y, z) * 255;
				if (a_blue != b_blue)
				{
					return false;
				}
			}
		}
	}

	return true;
}

void chomsky::ItemIconSilhouette::clear()
{
	this->point_cloud_match = TileSpacePointCloudMatch();
	this->weight.clear();
	this->red.clear();
	this->green.clear();
	this->blue.clear();
}

void chomsky::ItemIconSilhouette::normalize_colors()
{
	for (int z = 0; z < DEPTH; ++z)
	{
		for (int y = 0; y < HEIGHT; ++y)
		{
			for (int x = 0; x < WIDTH; ++x)
			{
				float weight = this->weight.get_fragment(x, y, z);
				if (weight < 1.0f)
				{
					continue;
				}

				float inverse_weight = 1.0f / this->weight.get_fragment(x, y, z);
				this->red.set_fragment(
					x, y, z,
					inverse_weight * this->red.get_fragment(x, y, z));
				this->green.set_fragment(
					x, y, z,
					inverse_weight * this->green.get_fragment(x, y, z));
				this->blue.set_fragment(
					x, y, z,
					inverse_weight * this->blue.get_fragment(x, y, z));
			}
		}
	}
}
