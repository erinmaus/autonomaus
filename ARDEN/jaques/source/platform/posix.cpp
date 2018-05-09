// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifdef JAQUES_BUILD_POSIX

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <dlfcn.h>
#include <X11/Xlib.h>
#include <map>
#include "jaques/frame.hpp"
#include "jaques/platform.hpp"
#include "touchstone/gl/dispatch.hpp"
#include "touchstone/gl/immediateDispatch.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/trace/callStream.hpp"

extern "C"
void* glXGetProcAddress(const char* proc)
{
	auto result = dlsym(RTLD_DEFAULT, proc);
	if (result == nullptr)
	{
		result = touchstone::ImmediateDispatch::load_gl_function(proc);
	}

	return result;
}

extern "C"
void* glXGetProcAddressARB(const char* proc)
{
	return glXGetProcAddress(proc);
}

static std::map<Window, GLXWindow> glx_window_map;

extern "C"
GLXWindow glXCreateWindow(Display* dpy, GLXFBConfig config, Window win, const int* attribList)
{
	auto& dispatch = jaques::Frame::get_dispatch();
	auto result = dispatch.XCreateWindow(dpy, config, win, attribList);

	glx_window_map[result] = win;

	return result;
}

extern "C"
void glDestroyWindow(Display* dpy, GLXWindow window)
{
	auto& dispatch = jaques::Frame::get_dispatch();
	dispatch.XDestroyWindow(dpy, window);

	glx_window_map.erase(window);
}

extern "C"
Bool glXMakeCurrent(Display* dpy, GLXDrawable drawable, GLXContext ctx)
{
	auto& dispatch = jaques::Frame::get_dispatch();

	Window window;
	{
		auto iter = glx_window_map.find(drawable);
		if (iter == glx_window_map.end())
		{
			window = drawable;
		}
		else
		{
			window = iter->second;
		}
	}

	jaques::Frame::before_switch_context((std::uintptr_t)ctx, window);
	auto result = dispatch.XMakeCurrent(dpy, drawable, ctx);
	jaques::Frame::after_switch_context((std::uintptr_t)ctx, window);

	return result;
}

extern "C"
Bool glXMakeContextCurrent(Display* dpy, GLXDrawable draw, GLXDrawable read, GLXContext ctx)
{
	auto& dispatch = jaques::Frame::get_dispatch();

	Window window;
	{
		auto iter = glx_window_map.find(draw);
		if (iter == glx_window_map.end())
		{
			window = draw;
		}
		else
		{
			window = iter->second;
		}
	}

	jaques::Frame::before_switch_context((std::uintptr_t)ctx, window);
	auto result = dispatch.XMakeContextCurrent(dpy, draw, read, ctx);
	jaques::Frame::after_switch_context((std::uintptr_t)ctx, window);

	return result;
}

extern "C"
void glXSwapBuffers(Display* display, GLXDrawable drawable)
{
	typedef void (* glXSwapBuffersProc)(Display*, GLXDrawable);
	static auto original_glXSwapBuffers = (glXSwapBuffersProc)touchstone::ImmediateDispatch::load_gl_function("glXSwapBuffers");

	jaques::Frame::end();

	original_glXSwapBuffers(display, drawable);
}

ganymede::CreatePluginProc jaques::load_plugin(const std::string& filename)
{
	auto handle = dlopen(filename.c_str(), RTLD_LAZY);
	if (handle == nullptr)
	{
		auto error_message = dlerror();
		if (error_message != nullptr)
		{
			std::fprintf(
				stdout, "couldn't load '%s': %s\n",
				filename.c_str(), error_message);
			exit(1);
		}
		else
		{
			std::fprintf(
				stdout, "couldn't load '%s'\n",
				filename.c_str());
			exit(1);
		}
		return nullptr;
	}

	auto symbol = dlsym(handle, ganymede::CREATE_PLUGIN_PROC_NAME);
	if (symbol == nullptr)
	{
		auto error_message = dlerror();
		if (error_message != nullptr)
		{
			std::fprintf(
				stdout, "couldn't load symbol %s from '%s': %s\n",
				ganymede::CREATE_PLUGIN_PROC_NAME, filename.c_str(),
				error_message);
		}
		else
		{
			std::fprintf(
				stdout, "couldn't load symbol %s from '%s'\n",
				ganymede::CREATE_PLUGIN_PROC_NAME, filename.c_str());
		}

		dlclose(handle);
		return nullptr;
	}

	return (ganymede::CreatePluginProc)symbol;
}

#endif
