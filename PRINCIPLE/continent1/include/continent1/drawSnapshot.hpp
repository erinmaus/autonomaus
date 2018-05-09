// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CONTINENT1_DRAW_SNAPSHOT_HPP
#define CONTINENT1_DRAW_SNAPSHOT_HPP

#include <cstdint>
#include <functional>
#include <glm/glm.hpp>
#include "kompvter/graphics/buffer.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"

namespace continent1
{
	class DrawSnapshot
	{
	public:
		typedef std::uint32_t Index;
		struct Vertex
		{
			glm::vec3 position;
			glm::vec3 normal;
			glm::vec4 color;
		};

		DrawSnapshot() = default;
		~DrawSnapshot() = default;

		enum
		{
			type_none = 0,
			type_model = 1,
			type_terrain = 2,
			type_water = 3
		};
		int get_type() const;

		int get_tag() const;
		void set_tag(int value);

		const kompvter::Buffer& get_vertices() const;

		static DrawSnapshot extract_model(
			const kvlc::Bounds& bounds,
			const kvre::Model* model,
			const kompvter::VertexIndexMapper& index_mapper,
			const glm::mat4& model_matrix);
		static DrawSnapshot extract_terrain(
			const kvlc::Bounds& bounds,
			const kvre::Terrain* terrain,
			const glm::mat4& model_matrix);
		static DrawSnapshot extract_water(
			const kvlc::Bounds& bounds,
			const kvre::Terrain* terrain,
			const glm::mat4& model_matrix);

	private:
		static Vertex split(const Vertex& a, const Vertex& b, float delta);
		static std::vector<Vertex> clip(
			const Vertex triangle[],
			const glm::vec4& plane);

		template <typename Iter>
		static std::vector<Vertex> clip(
			Iter begin, Iter end,
			const kvlc::Bounds& bounds,
			const std::function<glm::vec3(std::size_t)>& get_position,
			const std::function<glm::vec3(std::size_t)>& get_normal,
			const std::function<glm::vec4(std::size_t)>& get_color);

		template <typename Iter>
		static std::vector<Vertex> clip_terrain(
			Iter begin, Iter end,
			const kvlc::Bounds& bounds,
			const std::function<glm::vec3(std::size_t)>& get_position,
			const std::function<glm::vec3(std::size_t)>& get_normal,
			const std::function<glm::vec4(std::size_t)>& get_color);

		int type = type_none;
		kompvter::Buffer vertices;

		int tag = 0;
	};
}

template <typename Iter>
std::vector<continent1::DrawSnapshot::Vertex> continent1::DrawSnapshot::clip(
	Iter begin, Iter end,
	const kvlc::Bounds& bounds,
	const std::function<glm::vec3(std::size_t)>& get_position,
	const std::function<glm::vec3(std::size_t)>& get_normal,
	const std::function<glm::vec4(std::size_t)>& get_color)
{
	glm::vec3 X = glm::vec3(1.0f, 0.0f, 0.0f);
	glm::vec3 Z = glm::vec3(0.0f, 0.0f, 1.0f);
	glm::vec4 plane[4];
	plane[0] = glm::vec4(-X, glm::dot(-X, bounds.min));
	plane[1] = glm::vec4(X, glm::dot(X, bounds.max));
	plane[2] = glm::vec4(-Z, glm::dot(-Z, bounds.min));
	plane[3] = glm::vec4(Z, glm::dot(Z, bounds.max));

	std::vector<Vertex> vertices;
	std::vector<Vertex> triangles;
	std::vector<Vertex> output;

	auto current = begin;
	while (current != end)
	{
		triangles.clear();
		for (int i = 0; i < 3; ++i)
		{
			auto index = *current;
			++current;

			Vertex vertex;
			vertex.position = get_position(index);
			vertex.normal = get_normal(index);
			vertex.color = get_color(index);

			triangles.push_back(vertex);
		}

		std::swap(triangles[0], triangles[1]);

		for (int i = 0; i < 4; ++i)
		{
			output.clear();
			for (int j = 0; j < triangles.size(); j += 3)
			{
				auto result = clip(&triangles[j], plane[i]);

				// - The entire triangle can be clipped (0 vertices)
				// - Two vertices are clipped (3 vertices)
				// - One vertex is clipped (4 vertices)
				assert(
					result.size() == 0 ||
					result.size() == 3 ||
					result.size() == 4);

				if (result.size() >= 3)
				{
					output.insert(output.end(), result.begin(), result.begin() + 3);
				}
				if (result.size() >= 4)
				{
					output.push_back(result[0]);
					output.push_back(result[2]);
					output.push_back(result[3]);
				}
			}

			triangles = output;
		}

		vertices.insert(vertices.end(), output.begin(), output.end());
		output.clear();
	}

	return vertices;
}

template <typename Iter>
std::vector<continent1::DrawSnapshot::Vertex> continent1::DrawSnapshot::clip_terrain(
	Iter begin, Iter end,
	const kvlc::Bounds& bounds,
	const std::function<glm::vec3(std::size_t)>& get_position,
	const std::function<glm::vec3(std::size_t)>& get_normal,
	const std::function<glm::vec4(std::size_t)>& get_color)
{
	std::vector<Vertex> vertices;
	auto current = begin;
	while (current != end)
	{
		Vertex triangle[3];
		bool in_bounds = false;

		auto previous = current;
		for (auto i = 0; i < 3; ++i)
		{
			auto index = *current;
			++current;

			triangle[i].position = get_position(index);
			triangle[i].color = get_color(index);
			triangle[i].normal = get_normal(index);

			if (triangle[i].position.x >= bounds.min.x &&
				triangle[i].position.x <= bounds.max.x &&
				triangle[i].position.z >= bounds.min.z &&
				triangle[i].position.z <= bounds.max.z)
			{
				in_bounds = true;
			}
		}

		if (in_bounds)
		{
			auto clipped_triangle = clip(
				previous, current,
				bounds,
				get_position, get_normal, get_color);
			vertices.insert(
				vertices.begin(),
				clipped_triangle.begin(), clipped_triangle.end());
		}
	}

	return vertices;
}

#endif
