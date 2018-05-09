// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_MESH_HPP
#define AUTONOMAUS_GRAPHICS_MESH_HPP

#include <glm/glm.hpp>
#include "autonomaus/graphics/renderer.hpp"

namespace autonomaus
{
	class Mesh
	{
	public:
		Mesh(Renderer& renderer);
		~Mesh();

		std::size_t get_num_vertices() const;
		void set_num_vertices(std::size_t count);

		void set_positions(const glm::vec3 value[]);
		void set_textures(const glm::vec2 value[]);
		void set_colors(const glm::vec4 value[]);

		std::size_t get_num_indices() const;
		void set_indices(const std::uint32_t value[], std::size_t count);

		enum
		{
			primitive_triangles = 0,
			primitive_lines,
			primitive_points
		};
		void draw(int primitive);
		void draw(int primitive, std::size_t start, std::size_t count);

	private:
		std::size_t num_vertices = 0;
		GLuint position_buffer, texture_buffer, color_buffer;
		GLuint index_buffer;
		std::size_t num_indices = 0;
		GLuint vertex_array_object;

		Renderer* renderer;
		touchstone::ImmediateDispatch* gl;
	};
}

#endif
