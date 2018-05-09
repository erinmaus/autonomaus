// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifdef TOUCHSTONE_BUILD_WINDOWS

#include <cassert>
#include <cstdlib>
#include <iostream>
#include <map>
#define NOMINMAX
#include <Windows.h>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/immediateDispatch.hpp"
#include "touchstone/platform/windows.h"

static std::map<std::string, void*> windows_orig_funcs;
void touchstone::windows::register_hook(const char* glFuncName, void* origFunc)
{
	windows_orig_funcs[glFuncName] = origFunc;
}

void* touchstone::windows::get_hook(const char* glFuncName)
{
	auto iter = windows_orig_funcs.find(glFuncName);
	if (iter != windows_orig_funcs.end())
	{
		return iter->second;
	}

	return nullptr;
}

static HMODULE load_gl_library()
{
	auto library_name = std::getenv("ARDEN_OPENGL_LIBRARY");
	if (library_name == nullptr)
	{
		std::cerr << "System OpenGL library not provided; aborting." << std::endl;
		std::abort();
	}

	AllocConsole();
	FILE* f;
	freopen_s(&f, "CONOUT$", "w", stdout);
	printf("SUCCESS: %s\n", library_name);
	fflush(stdout);
	Sleep(1000);

	auto library = LoadLibrary(library_name);
	if (library == nullptr)
	{
		std::cout << "Couldn't open system OpenGL library; aborting." << std::endl;
		Sleep(10000);
		std::abort();
	}

	return library;
}

void* touchstone::ImmediateDispatch::load_gl_function(const char* name)
{
	static auto gl_library = load_gl_library();

	typedef PROC (WINAPI * wglGetProcAddressProc)(const char*);
	static auto wglGetProcAddressFunc =
		(wglGetProcAddressProc)touchstone::windows::get_hook("wglGetProcAddress");
	
	auto result = touchstone::windows::get_hook(name);
	if (result == nullptr)
	{
		result = ::GetProcAddress(gl_library, name);
		if (result == nullptr)
		{
			assert(wglGetProcAddressFunc != nullptr);
			result = wglGetProcAddressFunc(name);
		}
	}

	printf("%s %p\n", name, result);

	return result;
}

#endif
