// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kvre/draw/rangeVertexIterator.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "continent1/drawSnapshot.hpp"

int continent1::DrawSnapshot::get_type() const
{
	return this->type;
}

const kompvter::Buffer& continent1::DrawSnapshot::get_vertices() const
{
	return this->vertices;
}

int continent1::DrawSnapshot::get_tag() const
{
	return this->tag;
}

void continent1::DrawSnapshot::set_tag(int value)
{
	this->tag = value;
}

continent1::DrawSnapshot continent1::DrawSnapshot::extract_model(
	const kvlc::Bounds& bounds,
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper,
	const glm::mat4& model_matrix)
{
	auto get_position = [model, model_matrix](std::size_t index)
	{
		auto position = glm::vec4(model->get_position(index), 1.0f);
		auto transformed_position = model_matrix * position;
		return glm::vec3(transformed_position);
	};
	auto get_color = [model](std::size_t index)
	{
		return model->get_color(index);
	};
	auto get_normal = [model](std::size_t index)
	{
		return model->get_normal(index);
	};

	auto begin = kvre::IndexedVertexIterator::begin(index_mapper);
	auto end = kvre::IndexedVertexIterator::end(index_mapper);
	auto vertices = clip(
		begin, end,
		bounds,
		get_position, get_normal, get_color);

	DrawSnapshot result;
	result.type = type_model;
	if (vertices.size() > 0)
	{
		result.vertices.allocate(vertices.size() * sizeof(Vertex));
		result.vertices.update(
			0,
			(const std::uint8_t*)&vertices[0],
			result.vertices.get_data_size());
	}

	return result;
}

continent1::DrawSnapshot continent1::DrawSnapshot::extract_terrain(
	const kvlc::Bounds& bounds,
	const kvre::Terrain* terrain,
	const glm::mat4& model_matrix)
{
	auto get_position = [terrain, model_matrix](std::size_t index)
	{
		auto position = glm::vec4(terrain->get_position(index), 1.0f);
		auto transformed_position = model_matrix * position;
		return glm::vec3(transformed_position);
	};
	auto get_color = [terrain](std::size_t index)
	{
		return terrain->get_color(index);
	};
	auto get_normal = [terrain](std::size_t index)
	{
		return glm::vec3(0.0f, 1.0f, 0.0f);
	};

	auto begin = kvre::RangeVertexIterator(0);
	auto end = kvre::RangeVertexIterator(terrain->get_num_vertices());
	auto vertices = clip_terrain(
		begin, end,
		bounds,
		get_position, get_normal, get_color);

	DrawSnapshot result;
	result.type = type_terrain;
	if (vertices.size() > 0)
	{
		result.vertices.allocate(vertices.size() * sizeof(Vertex));
		result.vertices.update(
			0,
			(const std::uint8_t*)&vertices[0],
			result.vertices.get_data_size());
	}
	return result;
}

continent1::DrawSnapshot continent1::DrawSnapshot::extract_water(
	const kvlc::Bounds& bounds,
	const kvre::Terrain* water,
	const glm::mat4& model_matrix)
{
	auto get_position = [water, model_matrix](std::size_t index)
	{
		auto position = glm::vec4(water->get_position(index), 1.0f);
		auto transformed_position = model_matrix * position;
		return glm::vec3(transformed_position);
	};
	auto get_color = [water](std::size_t index)
	{
		return glm::vec4(0.0f, 0.0f, 1.0f, 0.5f);
	};
	auto get_normal = [water](std::size_t index)
	{
		return glm::vec3(0.0f, 1.0f, 0.0f);
	};

	auto begin = kvre::RangeVertexIterator(0);
	auto end = kvre::RangeVertexIterator(water->get_num_vertices());
auto 	vertices = clip_terrain(
		begin, end,
		bounds,
		get_position, get_normal, get_color);

	DrawSnapshot result;
	result.type = type_terrain;
	if (vertices.size() > 0)
	{
		result.vertices.allocate(vertices.size() * sizeof(Vertex));
		result.vertices.update(
			0,
			(const std::uint8_t*)&vertices[0],
			result.vertices.get_data_size());
	}
	return result;
}

continent1::DrawSnapshot::Vertex continent1::DrawSnapshot::split(
	const Vertex& a, const Vertex& b, float delta)
{
	Vertex result;
	result.position = glm::mix(a.position, b.position, delta);
	result.normal = glm::mix(a.normal, b.normal, delta);
	result.color = glm::mix(a.color, b.color, delta);
	return result;
}

std::vector<continent1::DrawSnapshot::Vertex> continent1::DrawSnapshot::clip(
	const Vertex triangle[],
	const glm::vec4& plane)
{
	auto normal = glm::vec3(plane);
	float d = plane.w;

	std::vector<Vertex> output_list;
	int previous = 2;
	for (int i = 0; i < 3; ++i)
	{
		auto& a = triangle[i];
		auto& b = triangle[previous];
		float distance_a = glm::dot(a.position, normal) - d;
		float distance_b = glm::dot(b.position, normal) - d;
		float delta = distance_a / (distance_a - distance_b);

		if (distance_a <= 1)
		{
			if (distance_b > 1)
			{
				output_list.push_back(split(a, b, delta));
			}

			output_list.push_back(a);
		}
		else if (distance_b <= 1)
		{
			output_list.push_back(split(a, b, delta));
		}

		previous = i;
	}

	return output_list;
}
