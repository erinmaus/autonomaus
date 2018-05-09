// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_SIZE_HPP
#define TOUCHSTONE_GL_SIZE_HPP

#include <cstdint>
#include <cstddef>
#include <vector>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/dispatch.hpp"

namespace touchstone { namespace size
{
	namespace TexParameter
	{
		std::size_t num_components(Dispatch& gl, GLenum pname);
	}

	namespace Get
	{
		std::size_t num_components(Dispatch& gl, GLenum pname);
	}

	namespace TexParameterfv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace TexParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace TexParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace Tex
	{
		std::size_t num_components(Dispatch& gl, GLenum format, GLenum type);
		std::size_t component_size(Dispatch& gl, GLenum type);
	}

	namespace TexImage1D
	{
		std::size_t pixels(Dispatch& gl, GLenum format, GLenum type, GLint width);
	}

	namespace TexImage2D
	{
		std::size_t pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height);
	}

	namespace ReadPixels
	{
		std::size_t pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height);
	}

	namespace GetBooleanv
	{
		std::size_t data(Dispatch& gl, GLenum pname);
	}

	namespace GetDoublev
	{
		std::size_t data(Dispatch& gl, GLenum pname);
	}

	namespace GetFloatv
	{
		std::size_t data(Dispatch& gl, GLenum pname);
	}

	namespace GetIntegerv
	{
		std::size_t data(Dispatch& gl, GLenum pname);
	}

	namespace GetTexImage
	{
		std::size_t pixels(Dispatch& gl, GLenum target, int level, GLenum format, GLenum type);
	}

	namespace GetTexParameterfv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetTexParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetTexLevelParameter
	{
		std::size_t num_components(Dispatch& gl, GLenum pname);
	}

	namespace GetTexLevelParameterfv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetTexLevelParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace DrawElements
	{
		std::size_t indices(Dispatch& gl, GLsizei count, GLenum type);
	}

	namespace TexSubImage1D
	{
		std::size_t pixels(Dispatch& gl, GLenum format, GLenum type, GLint width);
	}

	namespace TexSubImage2D
	{
		std::size_t pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height);
	}

	namespace DrawRangeElements
	{
		std::size_t indices(Dispatch& gl, GLsizei count, GLenum type);
	}

	namespace TexImage3D
	{
		std::size_t pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height, GLint depth);
	}

	namespace TexSubImage3D
	{
		std::size_t pixels(Dispatch& gl, GLenum format, GLenum type, GLint width, GLint height, GLint depth);
	}

	namespace GetCompressedTexImage
	{
		std::size_t img(Dispatch& gl, GLenum target, GLint level);
	}

	namespace MultiDrawArrays
	{
		std::size_t first(Dispatch& gl, GLsizei drawcount);
	}

	namespace MultiDrawArrays
	{
		std::size_t count(Dispatch& gl, GLsizei drawcount);
	}

	namespace MultiDrawElements
	{
		std::size_t count(Dispatch& gl, GLsizei drawcount);
	}

	namespace MultiDrawElements
	{
		std::size_t indices(Dispatch& gl, GLsizei drawcount);
	}

	namespace PointParameter
	{
		std::size_t num_components(Dispatch& gl, GLenum pname);
	}

	namespace PointParameterfv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace PointParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetQueryiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetQueryObjectiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetQueryObjectuiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetBufferParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace BindAttribLocation
	{
		std::size_t name(Dispatch& gl, const GLchar* name);
	}

	namespace GetAttribLocation
	{
		std::size_t name(Dispatch& gl, const GLchar* name);
	}

	namespace GetProgramiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetShaderiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetUniformLocation
	{
		std::size_t name(Dispatch& gl, const GLchar* name);
	}

	namespace GetUniform
	{
		std::size_t num_components(Dispatch& gl, GLuint program, GLint location);
		std::size_t component_size(Dispatch& gl, GLuint program, GLint location);
	}

	namespace GetUniformfv
	{
		std::size_t params(Dispatch& gl, GLuint program, GLint location);
	}

	namespace GetUniformiv
	{
		std::size_t params(Dispatch& gl, GLuint program, GLint location);
	}

	namespace ShaderSource
	{
		std::vector<std::size_t> string(Dispatch& gl, GLsizei count, const GLint* length, const GLchar* const* string);
	}

	namespace VertexAttribPointer
	{
		std::size_t pointer(Dispatch& gl, GLint size, GLenum type, GLsizei stride);
	}

	namespace GetBooleani_v
	{
		std::size_t data(Dispatch& gl, GLenum target);
	}

	namespace GetIntegeri_v
	{
		std::size_t data(Dispatch& gl, GLenum target);
	}

	namespace TransformFeedbackVaryings
	{
		std::vector<std::size_t> varyings(Dispatch& gl, GLsizei count, const GLchar* const* varyings);
	}

	namespace VertexAttribIPointer
	{
		std::size_t pointer(Dispatch& gl, GLint size, GLenum type, GLsizei stride);
	}

	namespace GetUniformuiv
	{
		std::size_t params(Dispatch& gl, GLuint program, GLint location);
	}

	namespace BindFragDataLocation
	{
		std::size_t name(Dispatch& gl, const char* name);
	}

	namespace GetFragDataLocation
	{
		std::size_t name(Dispatch& gl, const char* name);
	}

	namespace TexParameter
	{
		std::size_t num_components(Dispatch& gl, GLenum pname);
	}

	namespace TexParameterIiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace TexParameterIuiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetTexParameterIiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetTexParameterIuiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace ClearBuffer
	{
		std::size_t num_components(Dispatch& gl, GLenum buffer);
	}

	namespace ClearBufferiv
	{
		std::size_t value(Dispatch& gl, GLenum buffer);
	}

	namespace ClearBufferuiv
	{
		std::size_t value(Dispatch& gl, GLenum buffer);
	}

	namespace ClearBufferfv
	{
		std::size_t value(Dispatch& gl, GLenum buffer);
	}

	namespace GetRenderbufferParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetFramebufferAttachmentParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace DrawElementsInstanced
	{
		std::size_t indices(Dispatch& gl, GLsizei count, GLenum type);
	}

	namespace GetUniformIndices
	{
		std::vector<std::size_t> uniformNames(Dispatch& gl, GLsizei uniformCount, const GLchar* const* uniformNames);
	}

	namespace GetUniformIndices
	{
		std::size_t uniformIndices(Dispatch& gl, GLsizei uniformCount);
	}

	namespace GetActiveUniformsiv
	{
		std::size_t params(Dispatch& gl, GLsizei uniformCount, GLenum pname);
	}

	namespace GetUniformBlockIndex
	{
		std::size_t uniformBlockName(Dispatch& gl, const GLchar* uniformBlockName);
	}

	namespace GetActiveUniformBlockiv
	{
		std::size_t params(Dispatch& gl, GLuint program, GLuint uniformBlockIndex, GLenum pname);
	}

	namespace DrawElementsBaseVertex
	{
		std::size_t indices(Dispatch& gl, GLsizei count, GLenum type);
	}

	namespace DrawRangeElementsBaseVertex
	{
		std::size_t indices(Dispatch& gl, GLsizei count, GLenum type);
	}

	namespace DrawElementsInstancedBaseVertex
	{
		std::size_t indices(Dispatch& gl, GLsizei count, GLenum type);
	}

	namespace MultiDrawElementsBaseVertex
	{
		std::size_t count(Dispatch& gl, GLsizei drawcount);
	}

	namespace MultiDrawElementsBaseVertex
	{
		std::size_t indices(Dispatch& gl, GLsizei drawcount);
	}

	namespace MultiDrawElementsBaseVertex
	{
		std::size_t basevertex(Dispatch& gl, GLsizei drawcount);
	}

	namespace GetInteger64v
	{
		std::size_t data(Dispatch& gl, GLenum pname);
	}

	namespace GetInteger64i_v
	{
		std::size_t data(Dispatch& gl, GLenum target);
	}

	namespace GetBufferParameteri64v
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace BindFragDataLocationIndexed
	{
		std::size_t name(Dispatch& gl, const GLchar* name);
	}

	namespace GetFragDataIndex
	{
		std::size_t name(Dispatch& gl, const GLchar* name);
	}

	namespace GetMultisamplefv
	{
		std::size_t val(Dispatch& gl, GLenum pname);
	}

	namespace SamplerParameteriv
	{
		std::size_t param(Dispatch& gl, GLenum pname);
	}

	namespace SamplerParameterfv
	{
		std::size_t param(Dispatch& gl, GLenum pname);
	}

	namespace SamplerParameterIiv
	{
		std::size_t param(Dispatch& gl, GLenum pname);
	}

	namespace SamplerParameterIuiv
	{
		std::size_t param(Dispatch& gl, GLenum pname);
	}

	namespace GetSamplerParameteriv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetSamplerParameterIiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetSamplerParameterfv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetSamplerParameterIuiv
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetQueryObjecti64v
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetQueryObjectui64v
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetRenderbufferParameterivEXT
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}

	namespace GetFramebufferAttachmentParameterivEXT
	{
		std::size_t params(Dispatch& gl, GLenum pname);
	}
} }

#endif
