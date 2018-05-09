// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_RENDER_GL_HPP
#define KVNCER_RENDER_GL_HPP

#include <cstddef>
#include <GL/glew.h>

namespace kvncer { namespace gl
{
	GLenum marshal_vertex_fetcher_format_from_kompvter(int format);
	GLenum marshal_index_mapper_format_from_kompvter(int format);
	std::size_t get_type_size(GLenum type);
} }

#endif
