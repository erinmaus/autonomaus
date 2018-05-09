// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/graphics/vertexFetcher.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvncer/render/gl.hpp"

GLenum kvncer::gl::marshal_vertex_fetcher_format_from_kompvter(int format)
{
	switch(format)
	{
		case kompvter::VertexFetcher::format_byte:
			return GL_BYTE;
		case kompvter::VertexFetcher::format_unsigned_byte:
			return GL_UNSIGNED_BYTE;
		case kompvter::VertexFetcher::format_short:
			return GL_SHORT;
		case kompvter::VertexFetcher::format_unsigned_short:
			return GL_UNSIGNED_SHORT;
		case kompvter::VertexFetcher::format_integer:
			return GL_INT;
		case kompvter::VertexFetcher::format_unsigned_integer:
			return GL_UNSIGNED_INT;
		case kompvter::VertexFetcher::format_float:
			return GL_FLOAT;
		default:
			assert(false);
			return GL_NONE;
	}
}

GLenum kvncer::gl::marshal_index_mapper_format_from_kompvter(int format)
{
	switch (format)
	{
		case kompvter::VertexIndexMapper::format_unsigned_byte:
			return GL_UNSIGNED_BYTE;
		case kompvter::VertexIndexMapper::format_unsigned_short:
			return GL_UNSIGNED_SHORT;
		case kompvter::VertexIndexMapper::format_unsigned_integer:
			return GL_UNSIGNED_INT;
		default:
			assert(false);
			return GL_NONE;
	}
}

std::size_t kvncer::gl::get_type_size(GLenum type)
{
	switch (type)
	{
		case GL_BYTE:
			return sizeof(std::int8_t);
		case GL_UNSIGNED_BYTE:
			return sizeof(std::uint8_t);
		case GL_SHORT:
			return sizeof(std::int16_t);
		case GL_UNSIGNED_SHORT:
			return sizeof(std::uint16_t);
		case GL_INT:
			return sizeof(std::int32_t);
		case GL_UNSIGNED_INT:
			return sizeof(std::uint32_t);
		case GL_FLOAT:
			return sizeof(float);
		default:
			assert(false);
			return 0;
	}
}
