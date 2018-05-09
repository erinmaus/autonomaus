// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/graphics/shapes.hpp"

void autonomaus::make_box(
	Mesh& mesh,
	const glm::vec3& size,
	const glm::vec4& color)
{
	glm::vec3 half_size = size / 2.0f;
	glm::vec3 positions[] =
	{
		// front
		glm::vec3(-half_size.x, -half_size.y, half_size.z),  // top left
		glm::vec3(half_size.x, -half_size.y, half_size.z),   // top right
		glm::vec3(half_size.x, half_size.y, half_size.z),    // bottom right,
		glm::vec3(-half_size.x, half_size.y, half_size.z),   // bottom left

		// back
		glm::vec3(-half_size.x, -half_size.y, -half_size.z), // top left
		glm::vec3(half_size.x, -half_size.y, -half_size.z),  // top right
		glm::vec3(half_size.x, half_size.y, -half_size.z),   // bottom right,
		glm::vec3(-half_size.x, half_size.y, -half_size.z),  // bottom left

		// left
		glm::vec3(-half_size.x, -half_size.y, -half_size.z), // top left
		glm::vec3(-half_size.x, half_size.y, -half_size.z),  // top right
		glm::vec3(-half_size.x, half_size.y, half_size.z),   // bottom right
		glm::vec3(-half_size.x, -half_size.y, half_size.z),  // bottom left

		// right
		glm::vec3(half_size.x, -half_size.y, -half_size.z), // top left
		glm::vec3(half_size.x, half_size.y, -half_size.z),  // top right
		glm::vec3(half_size.x, half_size.y, half_size.z),   // bottom right
		glm::vec3(half_size.x, -half_size.y, half_size.z),  // bottom left

		// top
		glm::vec3(-half_size.x, half_size.y, -half_size.z), // top left
		glm::vec3(half_size.x, half_size.y, -half_size.z),  // top right
		glm::vec3(half_size.x, half_size.y, half_size.z),   // bottom right
		glm::vec3(-half_size.x, half_size.y, half_size.z),  // bottom left

		// bottom
		glm::vec3(-half_size.x, -half_size.y, -half_size.z), // top left
		glm::vec3(half_size.x, -half_size.y, -half_size.z),  // top right
		glm::vec3(half_size.x, -half_size.y, half_size.z),   // bottom right
		glm::vec3(-half_size.x, -half_size.y, half_size.z),  // bottom left
	};

	glm::vec4 colors[] =
	{
		color, color, color, color,
		color, color, color, color,
		color, color, color, color,
		color, color, color, color,
		color, color, color, color,
		color, color, color, color
	};

	glm::vec2 textures[] =
	{
		glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0),
		glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0),
		glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0),
		glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0),
		glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0),
		glm::vec2(0, 1), glm::vec2(1, 1), glm::vec2(1, 0), glm::vec2(0, 0)
	};

	std::uint32_t indices[] =
	{
		0, 1, 2, 0, 2, 3,
		4, 5, 6, 4, 6, 7,
		8, 9, 10, 8, 10, 11,
		12, 13, 14, 12, 14, 15,
		16, 17, 18, 16, 18, 19,
		20, 21, 22, 20, 22, 23
	};

	static const std::size_t NUM_VERTICES = 24;
	static const std::size_t NUM_INDICES = 36;

	mesh.set_num_vertices(NUM_VERTICES);
	mesh.set_positions(positions);
	mesh.set_textures(textures);
	mesh.set_colors(colors);
	mesh.set_indices(indices, NUM_INDICES);
}

void autonomaus::make_box_outline(
	Mesh& mesh,
	const glm::vec3& size,
	const glm::vec4& color)
{
	glm::vec3 half_size = size / 2.0f;
	glm::vec3 positions[] =
	{
		// top
		glm::vec3(-half_size.x, -half_size.y, -half_size.z), // top left
		glm::vec3(half_size.x, -half_size.y, -half_size.z),  // top right
		glm::vec3(half_size.x, -half_size.y, half_size.z),   // bottom right
		glm::vec3(-half_size.x, -half_size.y, half_size.z),  // bottom left

		// bottom
		glm::vec3(-half_size.x, half_size.y, -half_size.z),  // top left
		glm::vec3(half_size.x, half_size.y, -half_size.z),   // top right
		glm::vec3(half_size.x, half_size.y, half_size.z),    // bottom right
		glm::vec3(-half_size.x, half_size.y, half_size.z),   // bottom left
	};

	glm::vec4 colors[] =
	{
		color, color, color, color,
		color, color, color, color
	};

	std::uint32_t indices[] =
	{
		// top
		0, 1, 1, 2, 2, 3, 3, 0,

		// bottom,
		4, 5, 5, 6, 6, 7, 7, 4,

		// sides
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	static const std::size_t NUM_VERTICES = 8;
	static const std::size_t NUM_INDICES = 24;

	mesh.set_num_vertices(NUM_VERTICES);
	mesh.set_positions(positions);
	mesh.set_colors(colors);
	mesh.set_textures(nullptr);
	mesh.set_indices(indices, NUM_INDICES);
}

void autonomaus::make_sphere(Mesh& mesh, float radius, const glm::vec4& color)
{
	// Adapted from https://gist.github.com/zwzmzd/0195733fa1210346b00d

	static const int NUM_STEPS = 16;
	static const int NUM_VERTICES = (NUM_STEPS + 1) * (NUM_STEPS + 1) * 2;
	static const int NUM_INDICES = NUM_STEPS * (NUM_STEPS + 1) * 6;
	glm::vec3 positions[NUM_VERTICES];
	glm::vec4 colors[NUM_VERTICES];
	std::uint32_t indices[NUM_INDICES];

	std::uint32_t vertex_offset = 0;
	std::uint32_t index_offset = 0;
	std::uint32_t current_index = 0;
	for (int i = 0; i <= NUM_STEPS; ++i)
	{
		float lat0 = M_PI * (-0.5 + (float)(i - 1) / NUM_STEPS);
		float z0 = std::sin(lat0);
		float zr0 = std::cos(lat0);

		float lat1 = M_PI * (-0.5 + (float)i / NUM_STEPS);
		float z1 = std::sin(lat1);
		float zr1 = std::cos(lat1);

		for (int j = 0; j <= NUM_STEPS; ++j)
		{
			auto lng = 2 * M_PI * (float)(j - 1) / NUM_STEPS;
			float x = std::cos(lng);
			float y = std::sin(lng);

			colors[vertex_offset] = color;
			positions[vertex_offset++] = glm::vec3(x * zr0, z0, y * zr0);
			colors[vertex_offset] = color;
			positions[vertex_offset++] = glm::vec3(x * zr1, z1, y * zr1);
		}

		indices[index_offset++] = current_index + 0;
		indices[index_offset++] = current_index + 1;
		indices[index_offset++] = current_index + 2;
		current_index += 3;
		for (int j = 0; j <= (NUM_STEPS - 1) * 2; ++j)
		{
			indices[index_offset++] = current_index + j - 2;
			indices[index_offset++] = current_index + j - 1;
			indices[index_offset++] = current_index + j - 0;
		}
		current_index += NUM_STEPS * 2 - 1;
	}

	mesh.set_num_vertices(NUM_VERTICES);
	mesh.set_positions(positions);
	mesh.set_colors(colors);
	mesh.set_textures(nullptr);
	mesh.set_indices(indices, NUM_INDICES);
}
