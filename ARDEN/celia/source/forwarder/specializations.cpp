// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstdio>
#include <cstdlib>
#include "ganymede/ganymede.hpp"
#include "ganymede/functionLoader.hpp"
#include "celia/common/buffer.hpp"
#include "celia/common/utility.hpp"
#include "celia/forwarder/bufferMap.hpp"
#include "celia/forwarder/forwarder.hpp"
#include "celia/forwarder/inputQueue.hpp"

#define not_yet_implemented() \
	do { \
		std::fprintf(stderr, "%s not yet implemented\n", __FUNCTION__); \
		std::abort(); \
	} \
	while (false);

extern "C"
void glTexParameterfv(GLenum target, GLenum pname, const GLfloat* params)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(pname);
	_buffer.push_data(params, sizeof(GLfloat) * 4);
	celia::Forwarder::forward("glTexParameterfv", _buffer);
}

extern "C"
void glTexParameteriv(GLenum target, GLenum pname, const GLint* params)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(pname);
	_buffer.push_data(params, sizeof(GLint) * 4);
	celia::Forwarder::forward("glTexParameteriv", _buffer);
}

extern "C"
void glTexImage1D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLint border, GLenum format, GLenum type, const void* pixels)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(internalformat);
	_buffer.push(width);
	_buffer.push(border);
	_buffer.push(format);
	_buffer.push(type);
	_buffer.push_data(pixels, celia::utility::get_texture_pixel_data_size(format, type, width));
	celia::Forwarder::forward("glTexImage1D", _buffer);
}

extern "C"
void glTexImage2D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLint border, GLenum format, GLenum type, const void* pixels)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(internalformat);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(border);
	_buffer.push(format);
	_buffer.push(type);
	_buffer.push_data(pixels, celia::utility::get_texture_pixel_data_size(format, type, width, height));
	celia::Forwarder::forward("glTexImage2D", _buffer);
}

extern "C"
void glReadPixels(GLint x, GLint y, GLsizei width, GLsizei height, GLenum format, GLenum type, void * pixels)
{
	celia::Buffer _buffer;
	_buffer.push(x);
	_buffer.push(y);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(format);
	_buffer.push(type);

	celia::Forwarder::forward_with_reply("glReadPixels", _buffer);

	if (pixels != nullptr)
	{
		auto _size = celia::utility::get_texture_pixel_data_size(format, type, width, height);
		_buffer.copy(pixels, _size);
	}
}

extern "C"
void glGetBooleanv(GLenum pname, GLboolean * data)
{
	celia::Buffer _buffer;
	_buffer.push(pname);

	celia::Forwarder::forward_with_reply("glGetBooleanv", _buffer);

	auto _size = sizeof(GLboolean) * celia::utility::get_pname_count(pname);
	_buffer.copy(data, _size);
}

extern "C"
void glGetDoublev(GLenum pname, GLdouble * data)
{
	celia::Buffer _buffer;
	_buffer.push(pname);

	celia::Forwarder::forward_with_reply("glGetDoublev", _buffer);

	auto _size = sizeof(GLdouble) * celia::utility::get_pname_count(pname);
	_buffer.copy(data, _size);
}

extern "C"
GLenum glGetError()
{
	return GL_NO_ERROR;
}

extern "C"
void glGetFloatv(GLenum pname, GLfloat * data)
{
	celia::Buffer _buffer;
	_buffer.push(pname);

	celia::Forwarder::forward_with_reply("glGetFloatv", _buffer);

	auto _size = sizeof(GLfloat) * celia::utility::get_pname_count(pname);
	_buffer.copy(data, _size);
}

extern "C"
void glGetIntegerv(GLenum pname, GLint * data)
{
	celia::Buffer _buffer;
	_buffer.push(pname);

	celia::Forwarder::forward_with_reply("glGetIntegerv", _buffer);

	auto _size = sizeof(GLint) * celia::utility::get_pname_count(pname);
	_buffer.copy(data, _size);
}

extern "C"
const GLubyte* glGetString(GLenum name)
{
	static std::map<GLenum, std::string> _strings;
	{
		auto iter = _strings.find(name);
		if (iter != _strings.end())
		{
			return (const GLubyte*)iter->second.c_str();
		}
	}

	celia::Buffer _buffer;
	_buffer.push(name);
	celia::Forwarder::forward_with_reply("glGetString", _buffer);
	_strings[name] = std::string((const char*)_buffer.get_data(), _buffer.get_data_size());

	return (const GLubyte*)_strings[name].c_str();
}

extern "C"
void glGetTexImage(GLenum target, GLint level, GLenum format, GLenum type, void * pixels)
{
	not_yet_implemented();
}

extern "C"
void glGetTexParameterfv(GLenum target, GLenum pname, GLfloat * params)
{
	not_yet_implemented();
}

extern "C"
void glGetTexParameteriv(GLenum target, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetTexLevelParameteriv(GLenum target, GLint level, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetTexLevelParameterfv(GLenum target, GLint level, GLenum pname, GLfloat * params)
{
	not_yet_implemented();
}

extern "C"
GLboolean glIsEnabled(GLenum cap)
{
	celia::Buffer _buffer;
	_buffer.push(cap);
	celia::Forwarder::forward_with_reply("glIsEnabled", _buffer);

	return _buffer.pop<GLboolean>();
}

extern "C"
void glGetPointerv(GLenum pname, void ** params)
{
	not_yet_implemented();
}

extern "C"
void glDeleteTextures(GLsizei n, const GLuint * textures)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(textures, sizeof(GLuint) * n);

	celia::Forwarder::forward("glDeleteTextures", _buffer);
}

extern "C"
void glGenTextures(GLsizei n, GLuint * textures)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenTextures", _buffer);

	_buffer.copy(textures, sizeof(GLuint) * n);
}

extern "C"
GLboolean glIsTexture(GLuint texture)
{
	return GL_TRUE;
}

extern "C"
void glTexImage3D(GLenum target, GLint level, GLint internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLenum format, GLenum type, const void * pixels)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(internalformat);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(depth);
	_buffer.push(border);
	_buffer.push(format);
	_buffer.push(type);
	_buffer.push_data(pixels, celia::utility::get_texture_pixel_data_size(format, type, width, height, depth));
	celia::Forwarder::forward("glTexImage3D", _buffer);
}

extern "C"
void glTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLenum type, const void * pixels)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(xoffset);
	_buffer.push(width);
	_buffer.push(format);
	_buffer.push(type);
	_buffer.push_data(pixels, celia::utility::get_texture_pixel_data_size(format, type, width));
	celia::Forwarder::forward("glTexSubImage1D", _buffer);
}

extern "C"
void glTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLenum type, const void * pixels)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(xoffset);
	_buffer.push(yoffset);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(format);
	_buffer.push(type);
	_buffer.push_data(pixels, celia::utility::get_texture_pixel_data_size(format, type, width, height));
	celia::Forwarder::forward("glTexSubImage2D", _buffer);
}


extern "C"
void glTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLenum type, const void * pixels)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(xoffset);
	_buffer.push(yoffset);
	_buffer.push(zoffset);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(depth);
	_buffer.push(format);
	_buffer.push(type);
	_buffer.push_data(pixels, celia::utility::get_texture_pixel_data_size(format, type, width, height, depth));
	celia::Forwarder::forward("glTexSubImage3D", _buffer);
}

extern "C"
void glCompressedTexImage3D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLsizei depth, GLint border, GLsizei imageSize, const void * data)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(internalformat);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(depth);
	_buffer.push(border);
	_buffer.push(imageSize);
	_buffer.push_data(data, imageSize);
	celia::Forwarder::forward("glCompressedTexImage3D", _buffer);
}

extern "C"
void glCompressedTexImage2D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLsizei height, GLint border, GLsizei imageSize, const void * data)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(internalformat);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(border);
	_buffer.push(imageSize);
	_buffer.push_data(data, imageSize);
	celia::Forwarder::forward("glCompressedTexImage2D", _buffer);
}

extern "C"
void glCompressedTexImage1D(GLenum target, GLint level, GLenum internalformat, GLsizei width, GLint border, GLsizei imageSize, const void * data)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(internalformat);
	_buffer.push(width);
	_buffer.push(border);
	_buffer.push(imageSize);
	_buffer.push_data(data, imageSize);
	celia::Forwarder::forward("glCompressedTexImage1D", _buffer);
}

extern "C"
void glCompressedTexSubImage3D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLint zoffset, GLsizei width, GLsizei height, GLsizei depth, GLenum format, GLsizei imageSize, const void * data)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(xoffset);
	_buffer.push(yoffset);
	_buffer.push(zoffset);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(depth);
	_buffer.push(format);
	_buffer.push(imageSize);
	_buffer.push_data(data, imageSize);
	celia::Forwarder::forward("glCompressedTexSubImage3D", _buffer);
}

extern "C"
void glCompressedTexSubImage2D(GLenum target, GLint level, GLint xoffset, GLint yoffset, GLsizei width, GLsizei height, GLenum format, GLsizei imageSize, const void * data)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(xoffset);
	_buffer.push(yoffset);
	_buffer.push(width);
	_buffer.push(height);
	_buffer.push(format);
	_buffer.push(imageSize);
	_buffer.push_data(data, imageSize);
	celia::Forwarder::forward("glCompressedTexSubImage2D", _buffer);
}

extern "C"
void glCompressedTexSubImage1D(GLenum target, GLint level, GLint xoffset, GLsizei width, GLenum format, GLsizei imageSize, const void * data)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(level);
	_buffer.push(xoffset);
	_buffer.push(width);
	_buffer.push(format);
	_buffer.push(imageSize);
	_buffer.push_data(data, imageSize);
	celia::Forwarder::forward("glCompressedTexSubImage1D", _buffer);
}

extern "C"
void glGetCompressedTexImage(GLenum target, GLint level, void * img)
{
	not_yet_implemented();
}

extern "C"
void glMultiDrawArrays(GLenum mode, const GLint * first, const GLsizei * count, GLsizei drawcount)
{
	celia::Buffer _buffer;
	_buffer.push(mode);
	_buffer.push(drawcount);
	_buffer.push_data(first, sizeof(GLint) * drawcount);
	_buffer.push_data(count, sizeof(GLsizei) * drawcount);
	celia::Forwarder::forward("glMultiDrawArrays", _buffer);
}

extern "C"
void glMultiDrawElements(GLenum mode, const GLsizei * count, GLenum type, const void *const* indices, GLsizei drawcount)
{
	celia::Buffer _buffer;
	_buffer.push(mode);
	_buffer.push(drawcount);
	_buffer.push(type);
	_buffer.push_data(count, sizeof(GLsizei) * drawcount);
	_buffer.push_data(indices, sizeof(void*) * drawcount);
	celia::Forwarder::forward("glMultiDrawElements", _buffer);
}

extern "C"
void glPointParameterfv(GLenum pname, const GLfloat * params)
{
	not_yet_implemented();
}

extern "C"
void glPointParameteriv(GLenum pname, const GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGenQueries(GLsizei n, GLuint * ids)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenQueries", _buffer);

	_buffer.copy(ids, sizeof(GLuint) * n);
}

extern "C"
void glDeleteQueries(GLsizei n, const GLuint * ids)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(ids, sizeof(GLuint) * n);
	celia::Forwarder::forward("glDeleteQueries", _buffer);
}

extern "C"
GLboolean glIsQuery(GLuint id)
{
	return GL_TRUE;
}

extern "C"
void glGetQueryiv(GLenum target, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetQueryObjectiv(GLuint id, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetQueryObjectuiv(GLuint id, GLenum pname, GLuint * params)
{
	not_yet_implemented();
}

extern "C"
void glDeleteBuffers(GLsizei n, const GLuint * buffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(buffers, sizeof(GLuint) * n);
	celia::Forwarder::forward("glDeleteBuffers", _buffer);
}

extern "C"
void glBindBuffer(GLenum target, GLuint buffer)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(buffer);
	celia::Forwarder::forward("glBindBuffer", _buffer);

	celia::BufferMap::bind(target, buffer);
}

extern "C"
void glGenBuffers(GLsizei n, GLuint * buffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenBuffers", _buffer);
	_buffer.copy(buffers, sizeof(GLuint) * n);
}

extern "C"
GLboolean glIsBuffer(GLuint buffer)
{
	return GL_TRUE;
}


extern "C"
void glBufferData(GLenum target, GLsizeiptr size, const void * data, GLenum usage)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(size);
	_buffer.push_data(data, size);
	_buffer.push(usage);
	celia::Forwarder::forward("glBufferData", _buffer);
}

extern "C"
void glBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, const void * data)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(offset);
	_buffer.push(size);
	_buffer.push_data(data, size);
	celia::Forwarder::forward("glBufferSubData", _buffer);
}

extern "C"
void glGetBufferSubData(GLenum target, GLintptr offset, GLsizeiptr size, void * data)
{
	not_yet_implemented();
}

extern "C"
void* glMapBuffer(GLenum target, GLenum access)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	_buffer.push(access);

	celia::Forwarder::forward_with_reply("glMapBuffer", _buffer);

	bool _sync;
	if (access == GL_WRITE_ONLY || access == GL_READ_WRITE)
	{
		_sync = true;
	}
	else
	{
		_sync = false;
	}

	return celia::BufferMap::map(celia::BufferMap::get_current(target), _buffer, _sync);
}

extern "C"
void* glMapBufferRange(GLenum target, GLintptr offset, GLsizeiptr length, GLbitfield access)
{
	not_yet_implemented();
}

extern "C"
void glUnmapBuffer(GLenum target)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	celia::BufferMap::unmap(celia::BufferMap::get_current(target), _buffer);

	celia::Forwarder::forward("glUnmapBuffer", _buffer);
}

extern "C"
void glGetBufferParameteriv(GLenum target, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetBufferPointerv(GLenum target, GLenum pname, void ** params)
{
	not_yet_implemented();
}

extern "C"
void glDrawBuffers(GLsizei n, const GLenum * bufs)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(bufs, sizeof(GLenum) * n);
	celia::Forwarder::forward("glDrawBuffers", _buffer);
}

extern "C"
void glBindAttribLocation(GLuint program, GLuint index, const GLchar * name)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	_buffer.push(index);
	std::size_t _size = std::strlen(name);
	_buffer.push(_size);
	_buffer.push_data(name, _size);
	celia::Forwarder::forward("glBindAttribLocation", _buffer);
}

extern "C"
void glGetActiveUniform(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
	not_yet_implemented();
}

extern "C"
void glGetActiveAttrib(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLint * size, GLenum * type, GLchar * name)
{
	not_yet_implemented();
}

extern "C"
void glGetAttachedShaders(GLuint program, GLsizei maxCount, GLsizei * count, GLuint * shaders)
{
	not_yet_implemented();
}

extern "C"
void glGetAttribLocation(GLuint program, const GLchar * name)
{
	not_yet_implemented();
}

extern "C"
void glGetProgramiv(GLuint program, GLenum pname, GLint * params)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	_buffer.push(pname);
	celia::Forwarder::forward_with_reply("glGetProgramiv", _buffer);
	_buffer.copy(params, sizeof(GLint));
}

extern "C"
void glGetProgramInfoLog(GLuint program, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
	not_yet_implemented();
}

extern "C"
void glGetShaderiv(GLuint shader, GLenum pname, GLint * params)
{
	celia::Buffer _buffer;
	_buffer.push(shader);
	_buffer.push(pname);
	celia::Forwarder::forward_with_reply("glGetShaderiv", _buffer);
	_buffer.copy(params, sizeof(GLint));
}

extern "C"
void glGetShaderInfoLog(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * infoLog)
{
	not_yet_implemented();
}

extern "C"
void glGetShaderSource(GLuint shader, GLsizei bufSize, GLsizei * length, GLchar * source)
{
	not_yet_implemented();
}

extern "C"
GLint glGetUniformLocation(GLuint program, const GLchar * name)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	std::size_t _size = std::strlen(name);
	_buffer.push(_size);
	_buffer.push_data(name, _size);
	celia::Forwarder::forward_with_reply("glGetUniformLocation", _buffer);

	return _buffer.pop<GLint>();
}

extern "C"
void glGetUniformfv(GLuint program, GLint location, GLfloat * params)
{
	not_yet_implemented();
}

extern "C"
void glGetUniformiv(GLuint program, GLint location, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetVertexAttribdv(GLuint index, GLenum pname, GLdouble * params)
{
	not_yet_implemented();
}

extern "C"
void glGetVertexAttribfv(GLuint index, GLenum pname, GLfloat * params)
{
	not_yet_implemented();
}

extern "C"
void glGetVertexAttribiv(GLuint index, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetVertexAttribPointerv(GLuint index, GLenum pname, void ** pointer)
{
	not_yet_implemented();
}

extern "C"
GLuint glCreateProgram()
{
	celia::Buffer _buffer;
	celia::Forwarder::forward_with_reply("glCreateProgram", _buffer);

	return _buffer.pop<GLuint>();
}

extern "C"
GLuint glCreateShader(GLenum type)
{
	celia::Buffer _buffer;
	_buffer.push(type);
	celia::Forwarder::forward_with_reply("glCreateShader", _buffer);

	return _buffer.pop<GLuint>();
}

extern "C"
GLboolean glIsProgram(GLuint program)
{
	return GL_TRUE;
}

extern "C"
GLboolean glIsShader(GLuint shader)
{
	return GL_TRUE;
}

extern "C"
void glShaderSource(GLuint shader, GLsizei count, const GLchar *const* string, const GLint * length)
{
	celia::Buffer _buffer;
	_buffer.push(shader);
	_buffer.push(count);
	for (std::size_t i = 0; i < count; ++i)
	{
		std::size_t _length;
		if (length == nullptr)
		{
			_length = std::strlen(string[i]);
		}
		else
		{
			_length = length[i];
		}
		_buffer.push(_length);
		_buffer.push_data(string[i], _length);
	}
	celia::Forwarder::forward("glShaderSource", _buffer);
}

extern "C"
void glUniform1fv(GLint location, GLsizei count, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLfloat) * count);
	celia::Forwarder::forward("glUniform1fv", _buffer);
}

extern "C"
void glUniform2fv(GLint location, GLsizei count, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLfloat) * count * 2);
	celia::Forwarder::forward("glUniform2fv", _buffer);
}

extern "C"
void glUniform3fv(GLint location, GLsizei count, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLfloat) * count * 3);
	celia::Forwarder::forward("glUniform3fv", _buffer);
}

extern "C"
void glUniform4fv(GLint location, GLsizei count, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLfloat) * count * 4);
	celia::Forwarder::forward("glUniform4fv", _buffer);
}

extern "C"
void glUniform1iv(GLint location, GLsizei count, const GLint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLint) * count);
	celia::Forwarder::forward("glUniform1iv", _buffer);
}

extern "C"
void glUniform2iv(GLint location, GLsizei count, const GLint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(value);
	_buffer.push_data(value, sizeof(GLint) * count * 2);
	celia::Forwarder::forward("glUniform2iv", _buffer);
}

extern "C"
void glUniform3iv(GLint location, GLsizei count, const GLint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(value);
	_buffer.push_data(value, sizeof(GLint) * count * 3);
	celia::Forwarder::forward("glUniform3iv", _buffer);
}

extern "C"
void glUniform4iv(GLint location, GLsizei count, const GLint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(value);
	_buffer.push_data(value, sizeof(GLint) * count * 4);
	celia::Forwarder::forward("glUniform4iv", _buffer);
}

extern "C"
void glUniformMatrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * 4 * sizeof(GLfloat));
	celia::Forwarder::forward("glUniformMatrix2fv", _buffer);
}

extern "C"
void glUniformMatrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * 12 * sizeof(GLfloat));
	celia::Forwarder::forward("glUniformMatrix3fv", _buffer);
}

extern "C"
void glUniformMatrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * 16 * sizeof(GLfloat));
	celia::Forwarder::forward("glUniformMatrix4fv", _buffer);
}

extern "C"
void glVertexAttrib1dv(GLuint index, const GLdouble * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib1fv(GLuint index, const GLfloat * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib1sv(GLuint index, const GLshort * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib2dv(GLuint index, const GLdouble * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib2fv(GLuint index, const GLfloat * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib2sv(GLuint index, const GLshort * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib3dv(GLuint index, const GLdouble * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib3fv(GLuint index, const GLfloat * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib3sv(GLuint index, const GLshort * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4Nbv(GLuint index, const GLbyte * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4Niv(GLuint index, const GLint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4Nsv(GLuint index, const GLshort * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4Nubv(GLuint index, const GLubyte * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4Nuiv(GLuint index, const GLuint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4Nusv(GLuint index, const GLushort * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4bv(GLuint index, const GLbyte * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4dv(GLuint index, const GLdouble * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4fv(GLuint index, const GLfloat * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4iv(GLuint index, const GLint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4sv(GLuint index, const GLshort * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4ubv(GLuint index, const GLubyte * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4uiv(GLuint index, const GLuint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttrib4usv(GLuint index, const GLushort * v)
{
	not_yet_implemented();
}

extern "C"
void glUniformMatrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * sizeof(GLfloat) * 6);
	celia::Forwarder::forward("glUniformMatrix2x3fv", _buffer);
}

extern "C"
void glUniformMatrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * sizeof(GLfloat) * 6);
	celia::Forwarder::forward("glUniformMatrix3x2fv", _buffer);
}

extern "C"
void glUniformMatrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * sizeof(GLfloat) * 8);
	celia::Forwarder::forward("glUniformMatrix2x4fv", _buffer);
}

extern "C"
void glUniformMatrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * sizeof(GLfloat) * 8);
	celia::Forwarder::forward("glUniformMatrix4x2fv", _buffer);
}

extern "C"
void glUniformMatrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * sizeof(GLfloat) * 12);
	celia::Forwarder::forward("glUniformMatrix3x4fv", _buffer);
}

extern "C"
void glUniformMatrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push(transpose);
	_buffer.push_data(value, count * sizeof(GLfloat) * 12);
	celia::Forwarder::forward("glUniformMatrix4x3fv", _buffer);
}

extern "C"
void glGetBooleani_v(GLenum target, GLuint index, GLboolean * data)
{
	not_yet_implemented();
}

extern "C"
void glGetIntegeri_v(GLenum target, GLuint index, GLint * data)
{
	not_yet_implemented();
}

extern "C"
GLboolean glIsEnabledi(GLenum target, GLuint index)
{
	not_yet_implemented();
}

extern "C"
void glTransformFeedbackVaryings(GLuint program, GLsizei count, const GLchar *const* varyings, GLenum bufferMode)
{
	not_yet_implemented();
}

extern "C"
void glGetTransformFeedbackVarying(GLuint program, GLuint index, GLsizei bufSize, GLsizei * length, GLsizei * size, GLenum * type, GLchar * name)
{
	not_yet_implemented();
}

extern "C"
void glGetVertexAttribIiv(GLuint index, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetVertexAttribIuiv(GLuint index, GLenum pname, GLuint * params)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI1iv(GLuint index, const GLint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI2iv(GLuint index, const GLint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI3iv(GLuint index, const GLint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI4iv(GLuint index, const GLint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI1uiv(GLuint index, const GLuint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI2uiv(GLuint index, const GLuint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI3uiv(GLuint index, const GLuint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI4uiv(GLuint index, const GLuint * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI4bv(GLuint index, const GLbyte * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI4sv(GLuint index, const GLshort * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI4ubv(GLuint index, const GLubyte * v)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribI4usv(GLuint index, const GLushort * v)
{
	not_yet_implemented();
}

extern "C"
void glGetUniformuiv(GLuint program, GLint location, GLuint * params)
{
	not_yet_implemented();
}

extern "C"
void glBindFragDataLocation(GLuint program, GLuint color, const GLchar * name)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	_buffer.push(color);
	std::size_t _size = std::strlen(name);
	_buffer.push(_size);
	_buffer.push_data(name, _size);
	celia::Forwarder::forward("glBindFragDataLocation", _buffer);
}

extern "C"
GLint glGetFragDataLocation(GLuint program, const GLchar * name)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	std::size_t _size = std::strlen(name);
	_buffer.push(_size);
	_buffer.push_data(name, _size);
	celia::Forwarder::forward_with_reply("glGetFragDataLocation", _buffer);

	return _buffer.pop<GLint>();
}

extern "C"
void glUniform1uiv(GLint location, GLsizei count, const GLuint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLuint) * count);
	celia::Forwarder::forward("glUniform1uiv", _buffer);
}

extern "C"
void glUniform2uiv(GLint location, GLsizei count, const GLuint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLuint) * count * 2);
	celia::Forwarder::forward("glUniform2uiv", _buffer);
}

extern "C"
void glUniform3uiv(GLint location, GLsizei count, const GLuint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLuint) * count * 3);
	celia::Forwarder::forward("glUniform3uiv", _buffer);
}

extern "C"
void glUniform4uiv(GLint location, GLsizei count, const GLuint * value)
{
	celia::Buffer _buffer;
	_buffer.push(location);
	_buffer.push(count);
	_buffer.push_data(value, sizeof(GLuint) * count * 4);
	celia::Forwarder::forward("glUniform4uiv", _buffer);
}

extern "C"
void glTexParameterIiv(GLenum target, GLenum pname, const GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glTexParameterIuiv(GLenum target, GLenum pname, const GLuint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetTexParameterIiv(GLenum target, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetTexParameterIuiv(GLenum target, GLenum pname, GLuint * params)
{
	not_yet_implemented();
}

extern "C"
void glClearBufferiv(GLenum buffer, GLint drawbuffer, const GLint * value)
{
	celia::Buffer _buffer;
	_buffer.push(buffer);
	_buffer.push(drawbuffer);
	_buffer.push_data(value, sizeof(GLint) * 4);
	celia::Forwarder::forward("glClearBufferiv", _buffer);
}

extern "C"
void glClearBufferuiv(GLenum buffer, GLint drawbuffer, const GLuint * value)
{
	celia::Buffer _buffer;
	_buffer.push(buffer);
	_buffer.push(drawbuffer);
	_buffer.push_data(value, sizeof(GLuint) * 4);
	celia::Forwarder::forward("glClearBufferuiv", _buffer);
}

extern "C"
void glClearBufferfv(GLenum buffer, GLint drawbuffer, const GLfloat * value)
{
	celia::Buffer _buffer;
	_buffer.push(buffer);
	_buffer.push(drawbuffer);
	_buffer.push_data(value, sizeof(GLfloat) * 4);
	celia::Forwarder::forward("glClearBufferfv", _buffer);
}

extern "C"
const GLchar* glGetStringi(GLenum name, GLuint index)
{
	typedef std::pair<GLenum, GLuint> NameIndexPair;
	auto key = NameIndexPair(name, index);
	static std::map<NameIndexPair, std::string> _strings;
	{
		auto iter = _strings.find(key);
		if (iter != _strings.end())
		{
			return iter->second.c_str();
		}
	}

	celia::Buffer _buffer;
	_buffer.push(name);
	_buffer.push(index);

	celia::Forwarder::forward_with_reply("glGetStringi", _buffer);

	_strings[key] = std::string((const char*)_buffer.get_data(), _buffer.get_data_size());
	return _strings[key].c_str();
}

extern "C"
GLboolean glIsRenderbuffer(GLuint renderbuffer)
{
	return GL_TRUE;
}

extern "C"
void glDeleteRenderbuffers(GLsizei n, const GLuint * renderbuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(renderbuffers, sizeof(GLuint) * n);
	celia::Forwarder::forward("glDeleteRenderbuffers", _buffer);
}

extern "C"
void glGenRenderbuffers(GLsizei n, GLuint * renderbuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenRenderbuffers", _buffer);
	_buffer.copy(renderbuffers, sizeof(GLuint) * n);
}

extern "C"
void glGetRenderbufferParameteriv(GLenum target, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
GLboolean glIsFramebuffer(GLuint framebuffer)
{
	return GL_TRUE;
}

extern "C"
void glDeleteFramebuffers(GLsizei n, const GLuint * framebuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(framebuffers, sizeof(GLuint) * n);
	celia::Forwarder::forward("glDeleteFramebuffers", _buffer);
}

extern "C"
void glGenFramebuffers(GLsizei n, GLuint * framebuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenFramebuffers", _buffer);
	_buffer.copy(framebuffers, sizeof(GLuint) * n);
}

extern "C"
GLenum glCheckFramebufferStatus(GLenum target)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	celia::Forwarder::forward_with_reply("glCheckFramebufferStatus", _buffer);
	return _buffer.pop<GLenum>();
}

extern "C"
void glGetFramebufferAttachmentParameteriv(GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glDeleteVertexArrays(GLsizei n, const GLuint * arrays)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(arrays, sizeof(GLuint) * n);
	celia::Forwarder::forward("glDeleteVertexArrays", _buffer);
}

extern "C"
void glGenVertexArrays(GLsizei n, GLuint * arrays)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenVertexArrays", _buffer);
	_buffer.copy(arrays, sizeof(GLuint) * n);
}

extern "C"
GLboolean glIsVertexArray(GLuint array)
{
	return GL_TRUE;
}

extern "C"
void glGetUniformIndices(GLuint program, GLsizei uniformCount, const GLchar *const* uniformNames, GLuint * uniformIndices)
{
	not_yet_implemented();
}

extern "C"
void glGetActiveUniformsiv(GLuint program, GLsizei uniformCount, const GLuint * uniformIndices, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetActiveUniformName(GLuint program, GLuint uniformIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformName)
{
	not_yet_implemented();
}

extern "C"
GLuint glGetUniformBlockIndex(GLuint program, const GLchar * uniformBlockName)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	std::size_t _size = std::strlen(uniformBlockName);
	_buffer.push(_size);
	_buffer.push_data(uniformBlockName, _size);
	celia::Forwarder::forward_with_reply("glGetUniformBlockIndex", _buffer);
	return _buffer.pop<GLuint>();
}

extern "C"
void glGetActiveUniformBlockiv(GLuint program, GLuint uniformBlockIndex, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetActiveUniformBlockName(GLuint program, GLuint uniformBlockIndex, GLsizei bufSize, GLsizei * length, GLchar * uniformBlockName)
{
	not_yet_implemented();
}

extern "C"
void glMultiDrawElementsBaseVertex(GLenum mode, const GLsizei * count, GLenum type, const void *const* indices, GLsizei drawcount, const GLint * basevertex)
{
	not_yet_implemented();
}

extern "C"
GLboolean glIsSync(GLsync sync)
{
	return GL_TRUE;
}

extern "C"
void glGetInteger64v(GLenum pname, GLint64 * data)
{
	not_yet_implemented();
}

extern "C"
void glGetSynciv(GLsync sync, GLenum pname, GLsizei bufSize, GLsizei * length, GLint * values)
{
	not_yet_implemented();
}

extern "C"
void glGetInteger64i_v(GLenum target, GLuint index, GLint64 * data)
{
	not_yet_implemented();
}

extern "C"
void glGetBufferParameteri64v(GLenum target, GLenum pname, GLint64 * params)
{
	not_yet_implemented();
}

extern "C"
void glGetMultisamplefv(GLenum pname, GLuint index, GLfloat * val)
{
	not_yet_implemented();
}

extern "C"
void glBindFragDataLocationIndexed(GLuint program, GLuint colorNumber, GLuint index, const GLchar * name)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	_buffer.push(colorNumber);
	_buffer.push(index);
	std::size_t _size = std::strlen(name);
	_buffer.push(_size);
	_buffer.push_data(name, _size);
	celia::Forwarder::forward("glBindFragDataLocationIndexed", _buffer);
}

extern "C"
GLint glGetFragDataIndex(GLuint program, const GLchar * name)
{
	celia::Buffer _buffer;
	_buffer.push(program);
	std::size_t _size = std::strlen(name);
	_buffer.push(_size);
	_buffer.push_data(name, _size);
	celia::Forwarder::forward_with_reply("glGetFragDataIndex", _buffer);
	return _buffer.pop<GLint>();
}

extern "C"
void glGenSamplers(GLsizei count, GLuint * samplers)
{
	celia::Buffer _buffer;
	_buffer.push(count);
	celia::Forwarder::forward("glGenSamplers", _buffer);
	_buffer.copy(samplers, sizeof(GLuint) * count);
}

extern "C"
void glDeleteSamplers(GLsizei count, const GLuint * samplers)
{
	celia::Buffer _buffer;
	_buffer.push(count);
	_buffer.push_data(samplers, sizeof(GLuint) * count);
	celia::Forwarder::forward("glDeleteSamplers", _buffer);
}

extern "C"
GLboolean glIsSampler(GLuint sampler)
{
	return GL_TRUE;
}

extern "C"
void glSamplerParameteriv(GLuint sampler, GLenum pname, const GLint * param)
{
	not_yet_implemented();
}

extern "C"
void glSamplerParameterfv(GLuint sampler, GLenum pname, const GLfloat * param)
{
	not_yet_implemented();
}

extern "C"
void glSamplerParameterIiv(GLuint sampler, GLenum pname, const GLint * param)
{
	not_yet_implemented();
}

extern "C"
void glSamplerParameterIuiv(GLuint sampler, GLenum pname, const GLuint * param)
{
	not_yet_implemented();
}

extern "C"
void glGetSamplerParameteriv(GLuint sampler, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetSamplerParameterIiv(GLuint sampler, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetSamplerParameterfv(GLuint sampler, GLenum pname, GLfloat * params)
{
	not_yet_implemented();
}

extern "C"
void glGetSamplerParameterIuiv(GLuint sampler, GLenum pname, GLuint * params)
{
	not_yet_implemented();
}

extern "C"
void glGetQueryObjecti64v(GLuint id, GLenum pname, GLint64 * params)
{
	not_yet_implemented();
}

extern "C"
void glGetQueryObjectui64v(GLuint id, GLenum pname, GLuint64 * params)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribP1uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribP2uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribP3uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value)
{
	not_yet_implemented();
}

extern "C"
void glVertexAttribP4uiv(GLuint index, GLenum type, GLboolean normalized, const GLuint * value)
{
	not_yet_implemented();
}

extern "C"
void glVertexP2uiv(GLenum type, const GLuint * value)
{
	not_yet_implemented();
}

extern "C"
void glVertexP3uiv(GLenum type, const GLuint * value)
{
	not_yet_implemented();
}

extern "C"
void glVertexP4uiv(GLenum type, const GLuint * value)
{
	not_yet_implemented();
}

extern "C"
void glTexCoordP1uiv(GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glTexCoordP2uiv(GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glTexCoordP3uiv(GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glTexCoordP4uiv(GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glMultiTexCoordP1uiv(GLenum texture, GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glMultiTexCoordP2uiv(GLenum texture, GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glMultiTexCoordP3uiv(GLenum texture, GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glMultiTexCoordP4uiv(GLenum texture, GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glNormalP3uiv(GLenum type, const GLuint * coords)
{
	not_yet_implemented();
}

extern "C"
void glColorP3uiv(GLenum type, const GLuint * color)
{
	not_yet_implemented();
}

extern "C"
void glColorP4uiv(GLenum type, const GLuint * color)
{
	not_yet_implemented();
}

extern "C"
void glSecondaryColorP3uiv(GLenum type, const GLuint * color)
{
	not_yet_implemented();
}

extern "C"
GLboolean glIsRenderbufferEXT(GLuint renderbuffer)
{
	return GL_TRUE;
}

extern "C"
void glDeleteRenderbuffersEXT(GLsizei n, const GLuint * renderbuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(renderbuffers, sizeof(GLuint) * n);
	celia::Forwarder::forward("glDeleteRenderbuffersEXT", _buffer);
}

extern "C"
void glGenRenderbuffersEXT(GLsizei n, GLuint * renderbuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenRenderbuffersEXT", _buffer);
	_buffer.copy(renderbuffers, sizeof(GLuint) * n);
}

extern "C"
GLboolean glIsFramebufferEXT(GLuint framebuffer)
{
	return GL_TRUE;
}

extern "C"
void glGetRenderbufferParameterivEXT(GLenum target, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

extern "C"
void glGenFramebuffersEXT(GLsizei n, GLuint * framebuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	celia::Forwarder::forward_with_reply("glGenFramebuffersEXT", _buffer);
	_buffer.copy(framebuffers, sizeof(GLuint) * n);
}

extern "C"
void glDeleteFramebuffersEXT(GLsizei n, const GLuint * framebuffers)
{
	celia::Buffer _buffer;
	_buffer.push(n);
	_buffer.push_data(framebuffers, sizeof(GLuint) * n);
	celia::Forwarder::forward("glDeleteFramebuffersEXT", _buffer);
}

extern "C"
GLenum glCheckFramebufferStatusEXT(GLenum target)
{
	celia::Buffer _buffer;
	_buffer.push(target);
	celia::Forwarder::forward_with_reply("glCheckFramebufferStatusEXT", _buffer);
	return _buffer.pop<GLenum>();
}

extern "C"
void glGetFramebufferAttachmentParameterivEXT(GLenum target, GLenum attachment, GLenum pname, GLint * params)
{
	not_yet_implemented();
}

// XXX: Generator doesn't pick up glCopyImageSubData because it's OpenGL 4.3
extern "C"
void glCopyImageSubData(GLuint srcName, GLenum srcTarget, GLint srcLevel, GLint srcX, GLint srcY, GLint srcZ, GLuint dstName, GLenum dstTarget, GLint dstLevel, GLint dstX, GLint dstY, GLint dstZ, GLsizei srcWidth, GLsizei srcHeight, GLsizei srcDepth)
{
	celia::Buffer _buffer;
	_buffer.push(srcName);
	_buffer.push(srcTarget);
	_buffer.push(srcLevel);
	_buffer.push(srcX);
	_buffer.push(srcY);
	_buffer.push(srcZ);
	_buffer.push(dstName);
	_buffer.push(dstTarget);
	_buffer.push(dstLevel);
	_buffer.push(dstX);
	_buffer.push(dstY);
	_buffer.push(dstZ);
	_buffer.push(srcWidth);
	_buffer.push(srcHeight);
	_buffer.push(srcDepth);

	celia::Forwarder::forward("glCopyImageSubData", _buffer);
}

// XXX: These are hacks!

extern "C"
Bool glXMakeCurrent(Display* display, Window window, GLXDrawable drawable)
{
	typedef Bool (* Original)(Display*, Window, GLXDrawable);
	static Original original_func = nullptr;
	if (original_func == nullptr)
	{
		original_func = (Original)ganymede_load_function("glXMakeCurrent");
	}

	celia::InputQueue::set_window(window);

	return original_func(display, window, drawable);
}

#include <dlfcn.h>
#define GLX_WIDTH			0x801D
#define GLX_HEIGHT			0x801E

extern "C"
void glXSwapBuffers(Display* display, Drawable drawable)
{
	typedef int (* glXQueryDrawableFunc)(Display*, Drawable, int, unsigned int*);
	static glXQueryDrawableFunc glXQueryDrawable = nullptr;
	if (glXQueryDrawable == nullptr)
	{
		void* gl = dlopen("libGL.so", RTLD_NOW);
		glXQueryDrawable = (glXQueryDrawableFunc)dlsym(gl, "glXQueryDrawable");
	}

	unsigned int width, height;

	glXQueryDrawable(display, drawable, GLX_WIDTH, &width);
	glXQueryDrawable(display, drawable, GLX_HEIGHT, &height);

	celia::Buffer _buffer;
	_buffer.push(width);
	_buffer.push(height);

	celia::Forwarder::forward("glXSwapBuffers", _buffer);
}
