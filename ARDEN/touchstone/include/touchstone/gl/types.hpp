// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_TYPES_HPP
#define TOUCHSTONE_GL_TYPES_HPP

#include <cstring>

#ifdef TOUCHSTONE_NATIVE_GL

#include <GL/gl.h>

#ifdef TOUCHSTONE_BUILD_POSIX

#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <GL/glx.h>

#endif

#elif !defined(GL_VERSION_1_1)

#include <cstddef>
#include <cstdint>

typedef unsigned int GLenum;
typedef unsigned char GLboolean;
typedef unsigned int GLbitfield;
typedef signed char GLbyte;
typedef short GLshort;
typedef int GLint;
typedef int GLclampx;
typedef unsigned char GLubyte;
typedef unsigned short GLushort;
typedef unsigned int GLuint;
typedef int GLsizei;
typedef float GLfloat;
typedef float GLclampf;
typedef double GLdouble;
typedef double GLclampd;
typedef char GLchar;

typedef std::ptrdiff_t GLsizeiptr;
typedef std::ptrdiff_t GLintptr;

typedef const struct __GLsync* GLsync;
typedef std::uint64_t GLuint64;
typedef std::int64_t GLint64;

#ifdef TOUCHSTONE_BUILD_POSIX

#include <X11/Xlib.h>
#include <X11/Xutil.h>

typedef struct __GLXcontextRec *GLXContext;
typedef XID GLXPixmap;
typedef XID GLXDrawable;
typedef struct __GLXFBConfigRec *GLXFBConfig;
typedef XID GLXFBConfigID;
typedef XID GLXContextID;
typedef XID GLXWindow;
typedef XID GLXPbuffer;

#endif

#ifdef TOUCHSTONE_BUILD_WINDOWS

#define NOMINMAX
#include <Windows.h>
#include <wingdi.h>

#undef near
#undef far
#undef DrawState

#endif

#endif

#include "touchstone/trace/callStream.hpp"
#include "touchstone/trace/callStreamBuilder.hpp"

namespace touchstone
{
	namespace detail
	{
		template <std::uint8_t Token, typename T>
		struct Type
		{
			static const std::uint32_t value = Token;
			static const std::size_t size = sizeof(T);
			static const std::size_t alignment = alignof(T);
			typedef T type;
		};

		struct GLenum : public Type<1, unsigned int>
		{
			// Nothing.
		};

		struct GLboolean : public Type<2, unsigned char>
		{
			// Nothing.
		};

		struct GLbitfield : public Type<3, unsigned int>
		{
			// Nothing.
		};

		struct GLbyte : public Type<4, signed char>
		{
			// Nothing.
		};

		struct GLshort : public Type<5, short>
		{
			// Nothing.
		};

		struct GLint : public Type<6, int>
		{
			// Nothing.
		};

		struct GLclampx : public Type<7, int>
		{
			// Nothing.
		};

		struct GLubyte : public Type<8, unsigned char>
		{
			// Nothing.
		};

		struct GLushort : public Type<9, unsigned short>
		{
			// Nothing.
		};

		struct GLuint : public Type<10, unsigned int>
		{
			// Nothing.
		};

		struct GLsizei : public Type<11, int>
		{
			// Nothing.
		};

		struct GLfloat : public Type<12, float>
		{
			// Nothing.
		};

		struct GLclampf : public Type<13, float>
		{
			// Nothing.
		};

		struct GLdouble : public Type<14, double>
		{
			// Nothing.
		};

		struct GLclampd : public Type<15, double>
		{
			// Nothing.
		};

		struct GLchar : public Type<16, char>
		{
			// Nothing.
		};

		struct GLsizeiptr : public Type<17, std::ptrdiff_t>
		{
			// Nothing.
		};

		struct GLintptr : public Type<18, std::ptrdiff_t>
		{
			// Nothing.
		};

		struct GLsync : public Type<19, const struct __GLsync*>
		{
			// Nothing.
		};

		struct GLuint64 : public Type<20, std::uint64_t>
		{
			// Nothing.
		};

		struct GLint64 : public Type<21, std::int64_t>
		{
			// Nothing.
		};

		struct GLvoid : public Type<22, const void*>
		{
			// Nothing.
		};

		template <>
		struct CallStreamResultReader<const ::GLubyte*>
		{
			static const ::GLubyte* read(CallStream::Slice& slice);
		};

		template <>
		struct CallStreamResultWriter<const ::GLubyte*>
		{
			static void write(CallStreamBuilder& builder, const ::GLubyte* result);
		};
	}
}

inline const ::GLubyte*
touchstone::detail::CallStreamResultReader<const ::GLubyte*>::read(
	CallStream::Slice& slice)
{
	auto data = (const char*)slice.get_data(alignof(GLubyte));
	auto length = std::strlen(data) + 1;

	slice.advance(length, alignof(GLubyte));
	
	return (const ::GLubyte*)data;
}

inline void touchstone::detail::CallStreamResultWriter<const ::GLubyte*>::write(
	CallStreamBuilder& builder, const ::GLubyte* result)
{
	builder.append(
		std::strlen((const char*)result) + 1, alignof(::GLubyte),
		(const std::uint8_t*)result);
}

#endif
