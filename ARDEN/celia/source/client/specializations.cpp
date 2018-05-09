// This file is a part of ARDEN. It was generated during the build process.
//
// ARDEN is a component of [hdt], created by [bk]door.maus.

#include <cassert>
#include <cstring>
#define ALLEGRO_UNSTABLE
#include <allegro5/allegro.h>
#include <allegro5/allegro_opengl.h>
#include "celia/client/functions.hpp"
#include "celia/client/client.hpp"
#include "celia/common/buffer.hpp"
#include "celia/common/utility.hpp"

extern "C"
void cxglTexParameterfv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto pname = _buffer.pop<GLenum>();
	auto params = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * 4);

	glTexParameterfv(target, pname, params);
}

extern "C"
void cxglTexParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto pname = _buffer.pop<GLenum>();
	auto params = _buffer.pop_data<GLint>(sizeof(GLint) * 4);

	glTexParameteriv(target, pname, params);
}

extern "C"
void cxglTexImage1D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto internalformat = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto border = _buffer.pop<GLint>();
	auto format = _buffer.pop<GLenum>();
	auto type = _buffer.pop<GLenum>();
	auto data = _buffer.pop_data<void*>(celia::utility::get_texture_pixel_data_size(format, type, width));

	glTexImage1D(target, level, internalformat, width, border, format, type, data);
}

extern "C"
void cxglTexImage2D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto internalformat = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto border = _buffer.pop<GLint>();
	auto format = _buffer.pop<GLenum>();
	auto type = _buffer.pop<GLenum>();
	auto data = _buffer.pop_data<void*>(celia::utility::get_texture_pixel_data_size(format, type, width, height));

	glTexImage2D(target, level, internalformat, width, height, border, format, type, data);
}

extern "C"
void cxglReadPixels(celia::Buffer& _buffer, celia::Client& _client)
{
	auto x = _buffer.pop<GLint>();
	auto y = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto format = _buffer.pop<GLenum>();
	auto type = _buffer.pop<GLenum>();

	celia::Buffer _result;
	std::size_t _size = celia::utility::get_texture_pixel_data_size(format, type, width, height);
	void* v = _result.push_data((void*)nullptr, _size);

	glReadPixels(x, y, width, height, format, type, v);

	_client.reply("glReadPixels", _result);
}

extern "C"
void cxglGetBooleanv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto pname = _buffer.pop<GLenum>();

	GLboolean v[4];
	glGetBooleanv(pname, v);

	celia::Buffer _result;
	_result.push_data(v, sizeof(GLboolean) * 4);

	_client.reply("glGetBooleanv", _result);
}

extern "C"
void cxglGetDoublev(celia::Buffer& _buffer, celia::Client& _client)
{
	auto pname = _buffer.pop<GLenum>();

	GLdouble v[4];
	glGetDoublev(pname, v);

	celia::Buffer _result;
	_result.push_data(v, sizeof(GLdouble) * 4);

	_client.reply("glGetDoublev", _result);
}

extern "C"
void cxglGetError(celia::Buffer& _buffer, celia::Client& _client)
{
	celia::Buffer _result;
	_result.push(glGetError());

	_client.reply("glGetError", _result);
}

extern "C"
void cxglGetFloatv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto pname = _buffer.pop<GLenum>();

	GLfloat v[4];
	glGetFloatv(pname, v);

	celia::Buffer _result;
	_result.push_data(v, sizeof(GLfloat) * 4);

	_client.reply("glGetFloatv", _result);
}

extern "C"
void cxglGetIntegerv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto pname = _buffer.pop<GLenum>();

	GLint v[4];
	glGetIntegerv(pname, v);

	celia::Buffer _result;
	_result.push_data(v, sizeof(GLint) * 4);

	_client.reply("glGetIntegerv", _result);
}

extern "C"
void cxglGetString(celia::Buffer& _buffer, celia::Client& _client)
{
	auto name = _buffer.pop<GLenum>();

	const GLubyte* v = glGetString(name);

	celia::Buffer _result;
	std::size_t _size = std::strlen((const char*)v);
	_result.push_data(v, _size);

	_client.reply("glGetString", _result);
}

extern "C"
void cxglGetTexImage(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetTexParameterfv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetTexParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetTexLevelParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetTexLevelParameterfv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsEnabled(celia::Buffer& _buffer, celia::Client& _client)
{
	auto cap = _buffer.pop<GLenum>();
	GLboolean v = glIsEnabled(cap);

	celia::Buffer _result;
	_result.push(v);
	_client.reply("glIsEnabled", _result);
}

extern "C"
void cxglGetPointerv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteTextures(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	GLuint* textures = _buffer.pop_data<GLuint>(sizeof(GLuint) * n);

	glDeleteTextures(n, textures);
}

extern "C"
void cxglGenTextures(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	celia::Buffer _result;
	GLuint* textures = _result.push_data((GLuint*)nullptr, sizeof(GLuint) * n);

	glGenTextures(n, textures);

	_client.reply("glGenTextures", _result);
}

extern "C"
void cxglIsTexture(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglTexImage3D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto internalformat = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto depth = _buffer.pop<GLsizei>();
	auto border = _buffer.pop<GLint>();
	auto format = _buffer.pop<GLenum>();
	auto type = _buffer.pop<GLenum>();
	auto data = _buffer.pop_data<void*>(celia::utility::get_texture_pixel_data_size(format, type, width, height, depth));

	glTexImage3D(target, level, internalformat, width, height, depth, border, format, type, data);
}

extern "C"
void cxglTexSubImage1D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto xoffset = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto format = _buffer.pop<GLenum>();
	auto type = _buffer.pop<GLenum>();
	auto data = _buffer.pop_data<void>(celia::utility::get_texture_pixel_data_size(format, type, width));

	glTexSubImage1D(target, level, xoffset, width, format, type, data);
}

extern "C"
void cxglTexSubImage2D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto xoffset = _buffer.pop<GLint>();
	auto yoffset = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto format = _buffer.pop<GLenum>();
	auto type = _buffer.pop<GLenum>();
	auto data = _buffer.pop_data<void>(celia::utility::get_texture_pixel_data_size(format, type, width, height));

	glTexSubImage2D(target, level, xoffset, yoffset, width, height, format, type, data);
}

extern "C"
void cxglTexSubImage3D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto xoffset = _buffer.pop<GLint>();
	auto yoffset = _buffer.pop<GLint>();
	auto zoffset = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto depth = _buffer.pop<GLsizei>();
	auto format = _buffer.pop<GLenum>();
	auto type = _buffer.pop<GLenum>();
	auto data = _buffer.pop_data<void>(celia::utility::get_texture_pixel_data_size(format, type, width, height, depth));

	glTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, type, data);
}

extern "C"
void cxglCompressedTexImage3D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto internalformat = _buffer.pop<GLenum>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto depth = _buffer.pop<GLsizei>();
	auto border = _buffer.pop<GLint>();
	auto imageSize = _buffer.pop<GLsizei>();
	auto data = _buffer.pop_data<void>(imageSize);

	glCompressedTexImage3D(target, level, internalformat, width, height, depth, border, imageSize, data);
}

extern "C"
void cxglCompressedTexImage2D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto internalformat = _buffer.pop<GLenum>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto border = _buffer.pop<GLint>();
	auto imageSize = _buffer.pop<GLsizei>();
	auto data = _buffer.pop_data<void>(imageSize);

	glCompressedTexImage2D(target, level, internalformat, width, height, border, imageSize, data);
}

extern "C"
void cxglCompressedTexImage1D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto internalformat = _buffer.pop<GLenum>();
	auto width = _buffer.pop<GLsizei>();
	auto border = _buffer.pop<GLint>();
	auto imageSize = _buffer.pop<GLsizei>();
	auto data = _buffer.pop_data<void>(imageSize);

	glCompressedTexImage1D(target, level, internalformat, width, border, imageSize, data);
}

extern "C"
void cxglCompressedTexSubImage3D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto xoffset = _buffer.pop<GLint>();
	auto yoffset = _buffer.pop<GLint>();
	auto zoffset = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto depth = _buffer.pop<GLsizei>();
	auto format = _buffer.pop<GLenum>();
	auto imageSize = _buffer.pop<GLsizei>();
	auto data = _buffer.pop_data<void>(imageSize);

	glCompressedTexSubImage3D(target, level, xoffset, yoffset, zoffset, width, height, depth, format, imageSize, data);
}

extern "C"
void cxglCompressedTexSubImage2D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto xoffset = _buffer.pop<GLint>();
	auto yoffset = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto height = _buffer.pop<GLsizei>();
	auto format = _buffer.pop<GLenum>();
	auto imageSize = _buffer.pop<GLsizei>();
	auto data = _buffer.pop_data<void>(imageSize);

	glCompressedTexSubImage2D(target, level, xoffset, yoffset, width, height, format, imageSize, data);
}

extern "C"
void cxglCompressedTexSubImage1D(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto level = _buffer.pop<GLint>();
	auto xoffset = _buffer.pop<GLint>();
	auto width = _buffer.pop<GLsizei>();
	auto format = _buffer.pop<GLenum>();
	auto imageSize = _buffer.pop<GLsizei>();
	auto data = _buffer.pop_data<void>(imageSize);

	glCompressedTexSubImage1D(target, level, xoffset, width, format, imageSize, data);
}

extern "C"
void cxglGetCompressedTexImage(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglMultiDrawElements(celia::Buffer& _buffer, celia::Client& _client)
{
	auto mode = _buffer.pop<GLenum>();
	auto drawcount = _buffer.pop<GLsizei>();
	auto type = _buffer.pop<GLenum>();
	auto count = _buffer.pop_data<GLsizei>(sizeof(GLsizei) * drawcount);
	auto indices = _buffer.pop_data<const void*>(sizeof(void*) * drawcount);

	glMultiDrawElements(mode, count, type, indices, drawcount);
}

extern "C"
void cxglMultiDrawArrays(celia::Buffer& _buffer, celia::Client& _client)
{
	auto mode = _buffer.pop<GLenum>();
	auto drawcount = _buffer.pop<GLsizei>();
	auto first = _buffer.pop_data<GLint>(sizeof(GLint) * drawcount);
	auto count = _buffer.pop_data<GLsizei>(sizeof(GLsizei) * drawcount);

	glMultiDrawArrays(mode, first, count, drawcount);
}

extern "C"
void cxglPointParameterfv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglPointParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGenQueries(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteQueries(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsQuery(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetQueryiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetQueryObjectiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetQueryObjectuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteBuffers(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	GLuint* buffers = _buffer.pop_data<GLuint>(sizeof(GLuint) * n);

	glDeleteBuffers(n, buffers);
}

extern "C"
void cxglBindBuffer(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto buffer = _buffer.pop<GLuint>();

	glBindBuffer(target, buffer);
}

extern "C"
void cxglGenBuffers(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	celia::Buffer _result;
	GLuint* buffers = _result.push_data((GLuint*)nullptr, sizeof(GLuint) * n);

	glGenBuffers(n, buffers);

	_client.reply("glGenBuffers", _result);
}

extern "C"
void cxglIsBuffer(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglBufferData(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto size = _buffer.pop<GLsizeiptr>();
	auto data = _buffer.pop_data<void>(size);
	auto usage = _buffer.pop<GLenum>();

	glBufferData(target, size, data, usage);
}

extern "C"
void cxglBufferSubData(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto offset = _buffer.pop<GLintptr>();
	auto size = _buffer.pop<GLsizeiptr>();
	auto data = _buffer.pop_data<void>(size);

	glBufferSubData(target, offset, size, data);
}

extern "C"
void cxglGetBufferSubData(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

// XXX: Missing from Allegro.

#ifndef GL_BUFFER_MAP_POINTER
#define GL_BUFFER_MAP_POINTER 0x88BD
#endif

#ifndef GL_BUFFER_MAP_LENGTH
#define GL_BUFFER_MAP_LENGTH 0x9120
#endif

#ifndef GL_BUFFER_ACCESS_FLAGS
#define GL_BUFFER_ACCESS_FLAGS 0x911F
#endif

extern "C"
void cxglMapBuffer(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();
	auto access = _buffer.pop<GLenum>();

	void* v = glMapBuffer(target, access);

	GLint64 _length;
	glGetBufferParameteri64v(target, GL_BUFFER_MAP_LENGTH, &_length);

	celia::Buffer _result;
	if (access == GL_READ_ONLY || access == GL_READ_WRITE)
	{
		_result.push_data(v, _length);
	}
	else
	{
		_result.push_data((void*)nullptr, _length);
	}

	_client.reply("glMapBuffer", _result);
}

extern "C"
void cxglUnmapBuffer(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();

	GLint _flags;
	glGetBufferParameteriv(target, GL_BUFFER_ACCESS_FLAGS, &_flags);

	if (_flags == GL_WRITE_ONLY || _flags == GL_READ_WRITE)
	{
		void* p;
		glGetBufferPointerv(target, GL_BUFFER_MAP_POINTER, &p);

		auto _length = _buffer.pop<std::size_t>();
		std::memcpy(p, _buffer.pop_data<void>(_length), _length);
	}

	glUnmapBuffer(target);
}

extern "C"
void cxglGetBufferParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetBufferPointerv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDrawBuffers(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	auto bufs = _buffer.pop_data<GLenum>(sizeof(GLenum) * n);

	glDrawBuffers(n, bufs);
}

extern "C"
void cxglBindAttribLocation(celia::Buffer& _buffer, celia::Client& _client)
{
	auto program = _buffer.pop<GLuint>();
	auto index = _buffer.pop<GLuint>();
	auto _size = _buffer.pop<std::size_t>();
	std::string name(_buffer.pop_data<char>(_size), _size);

	glBindAttribLocation(program, index, name.c_str());
}

extern "C"
void cxglGetActiveUniform(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetActiveAttrib(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetAttachedShaders(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetAttribLocation(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetProgramiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto program = _buffer.pop<GLuint>();
	auto pname = _buffer.pop<GLenum>();

	GLint v;
	glGetProgramiv(program, pname, &v);

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glGetProgramiv", _result);
}

extern "C"
void cxglGetProgramInfoLog(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetShaderiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto shader = _buffer.pop<GLuint>();
	auto pname = _buffer.pop<GLenum>();

	GLint v;
	glGetShaderiv(shader, pname, &v);

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glGetShaderiv", _result);
}

extern "C"
void cxglGetShaderInfoLog(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetShaderSource(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetUniformLocation(celia::Buffer& _buffer, celia::Client& _client)
{
	auto program = _buffer.pop<GLuint>();
	std::size_t _size = _buffer.pop<std::size_t>();
	std::string name(_buffer.pop_data<char>(_size), _size);

	GLint v = glGetUniformLocation(program, name.c_str());

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glGetUniformLocation", _result);
}

extern "C"
void cxglGetUniformfv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetUniformiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetVertexAttribdv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetVertexAttribfv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetVertexAttribiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetVertexAttribPointerv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglCreateProgram(celia::Buffer& _buffer, celia::Client& _client)
{
	GLuint v = glCreateProgram();

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glCreateProgram", _result);
}

extern "C"
void cxglCreateShader(celia::Buffer& _buffer, celia::Client& _client)
{
	auto type = _buffer.pop<GLenum>();

	GLuint v = glCreateShader(type);

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glCreateShader", _result);
}


extern "C"
void cxglIsProgram(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsShader(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglShaderSource(celia::Buffer& _buffer, celia::Client& _client)
{
	GLuint shader = _buffer.pop<GLuint>();
	GLsizei count = _buffer.pop<GLuint>();
	GLchar** string = new GLchar*[count];
	GLint* length = new GLint[count];
	for (std::size_t i = 0; i < count; ++i)
	{
		length[i] = _buffer.pop<std::size_t>();
		string[i] = _buffer.pop_data<GLchar>(length[i]);
	}

	glShaderSource(shader, count, (const GLchar**)string, length);

	delete[] string;
}

extern "C"
void cxglUniform1fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count);

	glUniform1fv(location, count, value);
}

extern "C"
void cxglUniform2fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 2);

	glUniform2fv(location, count, value);
}

extern "C"
void cxglUniform3fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 3);

	glUniform3fv(location, count, value);
}

extern "C"
void cxglUniform4fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 4);

	glUniform4fv(location, count, value);
}

extern "C"
void cxglUniform1iv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLint>(sizeof(GLint) * count);

	glUniform1iv(location, count, value);
}

extern "C"
void cxglUniform2iv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLint>(sizeof(GLint) * count * 2);

	glUniform2iv(location, count, value);
}

extern "C"
void cxglUniform3iv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLint>(sizeof(GLint) * count * 3);

	glUniform3iv(location, count, value);
}

extern "C"
void cxglUniform4iv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLint>(sizeof(GLint) * count * 4);

	glUniform4iv(location, count, value);
}

extern "C"
void cxglUniformMatrix2fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 4);

	glUniformMatrix2fv(location, count, transpose, value);
}

extern "C"
void cxglUniformMatrix3fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 12);

	glUniformMatrix3fv(location, count, transpose, value);
}

extern "C"
void cxglUniformMatrix4fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 16);

	glUniformMatrix4fv(location, count, transpose, value);
}

extern "C"
void cxglVertexAttrib1dv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib1fv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib1sv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib2dv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib2fv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib2sv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib3dv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib3fv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib3sv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4Nbv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4Niv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4Nsv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4Nubv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4Nuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4Nusv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4bv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4dv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4fv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4iv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4sv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4ubv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttrib4usv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglUniformMatrix2x3fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 6);

	glUniformMatrix2x3fv(location, count, transpose, value);
}

extern "C"
void cxglUniformMatrix3x2fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 6);

	glUniformMatrix3x2fv(location, count, transpose, value);
}

extern "C"
void cxglUniformMatrix2x4fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 8);

	glUniformMatrix2x4fv(location, count, transpose, value);
}

extern "C"
void cxglUniformMatrix4x2fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 18);

	glUniformMatrix4x2fv(location, count, transpose, value);
}

extern "C"
void cxglUniformMatrix3x4fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 12);

	glUniformMatrix3x4fv(location, count, transpose, value);
}

extern "C"
void cxglUniformMatrix4x3fv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto transpose = _buffer.pop<GLboolean>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * count * 12);

	glUniformMatrix4x3fv(location, count, transpose, value);
}

extern "C"
void cxglGetBooleani_v(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetIntegeri_v(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsEnabledi(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglTransformFeedbackVaryings(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetTransformFeedbackVarying(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetVertexAttribIiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetVertexAttribIuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI1iv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI2iv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI3iv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI4iv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI1uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI2uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI4bv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI4sv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI4ubv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribI4usv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetUniformuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglBindFragDataLocation(celia::Buffer& _buffer, celia::Client& _client)
{
	auto program = _buffer.pop<GLuint>();
	auto color = _buffer.pop<GLuint>();
	auto _size = _buffer.pop<std::size_t>();
	std::string name(_buffer.pop_data<char>(_size), _size);

	glBindFragDataLocation(program, color, name.c_str());
}

extern "C"
void cxglGetFragDataLocation(celia::Buffer& _buffer, celia::Client& _client)
{
	auto program = _buffer.pop<GLuint>();
	auto _size = _buffer.pop<std::size_t>();
	std::string name(_buffer.pop_data<char>(_size), _size);

	GLint v = glGetFragDataLocation(program, name.c_str());
	celia::Buffer _result;
	_result.push(v);

	_client.reply("glGetFragDataLocation", _result);
}

extern "C"
void cxglUniform1uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLuint>(sizeof(GLuint) * count);

	glUniform1uiv(location, count, value);
}

extern "C"
void cxglUniform2uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLuint>(sizeof(GLuint) * count * 2);

	glUniform2uiv(location, count, value);
}

extern "C"
void cxglUniform3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLuint>(sizeof(GLuint) * count * 3);

	glUniform3uiv(location, count, value);
}

extern "C"
void cxglUniform4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto location = _buffer.pop<GLint>();
	auto count = _buffer.pop<GLsizei>();
	auto value = _buffer.pop_data<GLuint>(sizeof(GLuint) * count * 4);

	glUniform4uiv(location, count, value);
}

extern "C"
void cxglTexParameterIiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglTexParameterIuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetTexParameterIiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetTexParameterIuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglClearBufferiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto buffer = _buffer.pop<GLenum>();
	auto drawbuffer = _buffer.pop<GLint>();
	auto value = _buffer.pop_data<GLint>(sizeof(GLint) * 4);

	glClearBufferiv(buffer, drawbuffer, value);
}

extern "C"
void cxglClearBufferuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto buffer = _buffer.pop<GLenum>();
	auto drawbuffer = _buffer.pop<GLint>();
	auto value = _buffer.pop_data<GLuint>(sizeof(GLuint) * 4);

	glClearBufferuiv(buffer, drawbuffer, value);
}

extern "C"
void cxglClearBufferfv(celia::Buffer& _buffer, celia::Client& _client)
{
	auto buffer = _buffer.pop<GLenum>();
	auto drawbuffer = _buffer.pop<GLint>();
	auto value = _buffer.pop_data<GLfloat>(sizeof(GLfloat) * 4);

	glClearBufferfv(buffer, drawbuffer, value);
}

extern "C"
void cxglGetStringi(celia::Buffer& _buffer, celia::Client& _client)
{
	auto name = _buffer.pop<GLenum>();
	auto index = _buffer.pop<GLuint>();

	const GLubyte* v = glGetStringi(name, index);

	celia::Buffer _result;
	_result.push_data(v, std::strlen((const char*)v));

	_client.reply("glGetStringi", _result);
}

extern "C"
void cxglIsRenderbuffer(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteRenderbuffers(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	GLuint* renderbuffers = _buffer.pop_data<GLuint>(sizeof(GLuint) * n);

	glDeleteRenderbuffers(n, renderbuffers);
}

extern "C"
void cxglGenRenderbuffers(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	celia::Buffer _result;
	GLuint* renderbuffers = _result.push_data((GLuint*)nullptr, sizeof(GLuint) * n);

	glGenRenderbuffers(n, renderbuffers);

	_client.reply("glGenRenderbuffers", _result);
}

extern "C"
void cxglGetRenderbufferParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsFramebuffer(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteFramebuffers(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	GLuint* framebuffers = _buffer.pop_data<GLuint>(sizeof(GLuint) * n);

	glDeleteFramebuffers(n, framebuffers);
}

extern "C"
void cxglGenFramebuffers(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	celia::Buffer _result;
	GLuint* framebuffers = _result.push_data((GLuint*)nullptr, sizeof(GLuint) * n);

	glGenFramebuffers(n, framebuffers);

	_client.reply("glGenFramebuffers", _result);
}

extern "C"
void cxglCheckFramebufferStatus(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();

	GLenum v = glCheckFramebufferStatusEXT(target);

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glCheckFramebufferStatusEXT", _result);
}

extern "C"
void cxglGetFramebufferAttachmentParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteVertexArrays(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	GLuint* arrays = _buffer.pop_data<GLuint>(sizeof(GLuint) * n);

	glDeleteVertexArrays(n, arrays);
}

extern "C"
void cxglGenVertexArrays(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	celia::Buffer _result;
	GLuint* arrays = _result.push_data((GLuint*)nullptr, sizeof(GLuint) * n);

	glGenVertexArrays(n, arrays);

	_client.reply("glGenVertexArrays", _result);
}

extern "C"
void cxglIsVertexArray(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetUniformIndices(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetActiveUniformsiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetActiveUniformName(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetUniformBlockIndex(celia::Buffer& _buffer, celia::Client& _client)
{
	auto program = _buffer.pop<GLuint>();
	auto _size = _buffer.pop<std::size_t>();
	std::string uniformBlockName(_buffer.pop_data<char>(_size), _size);

	GLuint v = glGetUniformBlockIndex(program, uniformBlockName.c_str());

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glGetUniformBlockIndex", _result);
}

extern "C"
void cxglGetActiveUniformBlockiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetActiveUniformBlockName(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglMultiDrawElementsBaseVertex(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsSync(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetInteger64v(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetSynciv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetInteger64i_v(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetBufferParameteri64v(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetMultisamplefv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglBindFragDataLocationIndexed(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetFragDataIndex(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGenSamplers(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteSamplers(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsSampler(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglSamplerParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglSamplerParameterfv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglSamplerParameterIiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglSamplerParameterIuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetSamplerParameteriv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetSamplerParameterIiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetSamplerParameterfv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetSamplerParameterIuiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetQueryObjecti64v(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGetQueryObjectui64v(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribP1uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribP2uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribP3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexAttribP4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexP2uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexP3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglVertexP4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglTexCoordP1uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglTexCoordP2uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglTexCoordP3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglTexCoordP4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglMultiTexCoordP1uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglMultiTexCoordP2uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglMultiTexCoordP3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglMultiTexCoordP4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglNormalP3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglColorP3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglColorP4uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglSecondaryColorP3uiv(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglIsRenderbufferEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteRenderbuffersEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	GLuint* renderbuffers = _buffer.pop_data<GLuint>(sizeof(GLuint) * n);

	glDeleteRenderbuffersEXT(n, renderbuffers);
}

extern "C"
void cxglGenRenderbuffersEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	celia::Buffer _result;
	GLuint* renderbuffers = _result.push_data((GLuint*)nullptr, sizeof(GLuint) * n);

	glGenRenderbuffersEXT(n, renderbuffers);

	_client.reply("glGenRenderbuffersEXT", _result);
}

extern "C"
void cxglIsFramebufferEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglDeleteFramebuffersEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	GLuint* framebuffers = _buffer.pop_data<GLuint>(sizeof(GLuint) * n);

	glDeleteFramebuffers(n, framebuffers);
}

extern "C"
void cxglGetRenderbufferParameterivEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglGenFramebuffersEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	auto n = _buffer.pop<GLsizei>();
	celia::Buffer _result;
	GLuint* framebuffers = _result.push_data((GLuint*)nullptr, sizeof(GLuint) * n);

	glGenFramebuffersEXT(n, framebuffers);

	_client.reply("glGenFramebuffersEXT", _result);
}

extern "C"
void cxglCheckFramebufferStatusEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	auto target = _buffer.pop<GLenum>();

	GLenum v = glCheckFramebufferStatusEXT(target);

	celia::Buffer _result;
	_result.push(v);

	_client.reply("glCheckFramebufferStatusEXT", _result);
}

extern "C"
void cxglGetFramebufferAttachmentParameterivEXT(celia::Buffer& _buffer, celia::Client& _client)
{
	assert(false);
}

extern "C"
void cxglCopyImageSubData(celia::Buffer& _buffer, celia::Client& _client)
{
	ALLEGRO_DEFINE_PROC_TYPE(void, Func, (GLuint, GLenum, GLint, GLint, GLint, GLint, GLuint, GLenum, GLint, GLint, GLint, GLint, GLsizei, GLsizei, GLsizei));
	static Func gl_func = (Func)al_get_opengl_proc_address("glCopyImageSubData");

	auto srcName = _buffer.pop<GLuint>();
	auto srcTarget = _buffer.pop<GLenum>();
	auto srcLevel = _buffer.pop<GLint>();
	auto srcX = _buffer.pop<GLint>();
	auto srcY = _buffer.pop<GLint>();
	auto srcZ = _buffer.pop<GLint>();
	auto dstName = _buffer.pop<GLuint>();
	auto dstTarget = _buffer.pop<GLenum>();
	auto dstLevel = _buffer.pop<GLint>();
	auto dstX = _buffer.pop<GLint>();
	auto dstY = _buffer.pop<GLint>();
	auto dstZ = _buffer.pop<GLint>();
	auto srcWidth = _buffer.pop<GLsizei>();
	auto srcHeight = _buffer.pop<GLsizei>();
	auto srcDepth = _buffer.pop<GLsizei>();

	gl_func(srcName, srcTarget, srcLevel, srcX, srcY, srcZ, dstName, dstTarget, dstLevel, dstX, dstY, dstZ, srcWidth, srcHeight, srcDepth);
}

extern "C"
void cxglXSwapBuffers(celia::Buffer& _buffer, celia::Client& client)
{
	auto width = _buffer.pop<int>();
	auto height = _buffer.pop<int>();

	client.resize(width, height);
	al_flip_display();
}
