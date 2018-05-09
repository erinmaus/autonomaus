// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_GRAPHICS_MESH_BUFFER_HPP
#define KVNCER_GRAPHICS_MESH_BUFFER_HPP

#include "kompvter/graphics/buffer.hpp"
#include "kvncer/render/gl.hpp"

namespace kvncer
{
	class MeshBuffer
	{
	public:
		MeshBuffer();
		~MeshBuffer();

		const kompvter::Buffer& get_buffer() const;
		void set_buffer(const kompvter::Buffer& buffer);
		void update(GLenum target);

		void bind(GLenum target);

		bool get_is_dirty() const;

	private:
		GLuint buffer;
		kompvter::Buffer data;
		bool is_dirty = false;
	};
}

#endif
