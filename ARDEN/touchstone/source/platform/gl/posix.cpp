// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifdef TOUCHSTONE_BUILD_POSIX

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <dlfcn.h>
#include <X11/Xlib.h>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/immediateDispatch.hpp"

static void* load_gl_library()
{
	auto library_name = std::getenv("ARDEN_OPENGL_LIBRARY");
	if (library_name == nullptr)
	{
		std::cerr << "System OpenGL library not provided; aborting." << std::endl;
		std::abort();
	}

	auto library = dlopen(library_name, RTLD_GLOBAL | RTLD_LAZY);
	if (library == nullptr)
	{
		std::cerr << "Couldn't open system OpenGL library; aborting." << std::endl;
		std::abort();
	}

	return library;
}

void* touchstone::ImmediateDispatch::load_gl_function(const char* name)
{
	static void* gl_library = load_gl_library();

	typedef void* (* glXGetProcAddressARBProc)(const char*);
	static auto glXGetProcAddressARB =
		(glXGetProcAddressARBProc)dlsym(gl_library, "glXGetProcAddressARB");

	void* result = dlsym(gl_library, name);
	if (result == nullptr)
	{
		assert(glXGetProcAddressARB != nullptr);
		result = glXGetProcAddressARB(name);
	}

	return result;
}

#endif
