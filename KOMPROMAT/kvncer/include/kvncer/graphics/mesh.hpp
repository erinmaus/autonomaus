// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_GRAPHICS_MESH_HPP
#define KVNCER_GRAPHICS_MESH_HPP

#include <cstddef>
#include <cstdint>
#include <unordered_map>
#include "kvncer/render/gl.hpp"

namespace kvncer
{
	class MeshBuffer;

	class Mesh
	{
	public:
		Mesh();
		virtual ~Mesh();

		void enable_vertex_attrib(
			GLuint index,
			MeshBuffer* buffer,
			GLint num_components, GLenum type, GLboolean normalized,
			std::size_t offset, std::size_t stride);

		void set_indices(MeshBuffer* buffer);
		void set_indices(
			MeshBuffer* buffer,
			GLenum format, std::size_t start, std::size_t stop);

		void draw(GLenum primitive = GL_TRIANGLES);
		void draw(
			GLenum format, std::size_t start, std::size_t stop,
			GLenum primitive = GL_TRIANGLES);

		GLuint get_vertex_array_object() const;

	private:
		GLuint vertex_array_object;

		std::unordered_map<GLuint, MeshBuffer*> vertex_buffers;
		MeshBuffer* index_buffer = nullptr;

		int index_format = GL_NONE;
		std::size_t index_offset_start = 0, index_offset_stop = 0;
	};
}

#endif
