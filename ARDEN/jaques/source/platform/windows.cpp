// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifdef JAQUES_BUILD_WINDOWS

#include "jaques/frame.hpp"
#include "jaques/platform.hpp"
#include "touchstone/gl/dispatch.hpp"
#include "touchstone/gl/immediateDispatch.hpp"

extern "C"
PROC jaqGetProcAddress(LPCSTR lpszProc)
{
	return (PROC)touchstone::ImmediateDispatch::load_gl_function(lpszProc);
}

static std::map<HGLRC, HDC> hdc_map;

extern "C"
HGLRC jaqCreateContext(HDC hdc)
{
	auto& dispatch = jaques::Frame::get_dispatch();
	auto result = dispatch.CreateContext(hdc);
	hdc_map[result] = hdc;

	return result;
}

extern "C"
BOOL jaqDeleteContext(HGLRC hglrc)
{
	auto& dispatch = jaques::Frame::get_dispatch();
	auto result = dispatch.DeleteContext(hglrc);

	hdc_map.erase(hglrc);

	return result;
}

extern "C"
BOOL jaqMakeCurrent(HDC hdc, HGLRC hglrc)
{
	auto& dispatch = jaques::Frame::get_dispatch();

	hdc_map[hglrc] = hdc;

	jaques::Frame::before_switch_context((std::uintptr_t)hglrc, (std::uintptr_t)hdc);
	auto result = dispatch.MakeCurrent(hdc, hglrc);
	jaques::Frame::after_switch_context((std::uintptr_t)hglrc, (std::uintptr_t)hdc);

	return result;
}

extern "C"
void jaqSwapBuffers()
{
	auto& dispatch = jaques::Frame::get_dispatch();

	jaques::Frame::end();
}

std::string dlerror()
{
	//Get the error message, if any.
	DWORD errorMessageID = ::GetLastError();
	if (errorMessageID == 0)
		return std::string(); //No error message has been recorded

	LPSTR messageBuffer = nullptr;
	size_t size = FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
		NULL, errorMessageID, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPSTR)&messageBuffer, 0, NULL);

	std::string message(messageBuffer, size);

	//Free the buffer.
	LocalFree(messageBuffer);

	return message;
}

ganymede::CreatePluginProc jaques::load_plugin(const std::string& filename)
{
	auto handle = LoadLibrary(filename.c_str());
	if (handle == nullptr)
	{
		auto error_message = dlerror();
		if (!error_message.empty())
		{
			std::fprintf(
				stdout, "couldn't load '%s': %s\n",
				filename.c_str(), error_message.c_str());
			std::exit(1);
		}
		else
		{
			std::fprintf(
				stdout, "couldn't load '%s'\n",
				filename.c_str());
			std::exit(1);
		}

		return nullptr;
	}

	auto symbol = GetProcAddress(handle, ganymede::CREATE_PLUGIN_PROC_NAME);
	if (symbol == nullptr)
	{
		auto error_message = dlerror();
		if (!error_message.empty())
		{
			std::fprintf(
				stdout, "couldn't load symbol %s from '%s': %s\n",
				ganymede::CREATE_PLUGIN_PROC_NAME, filename.c_str(),
				error_message.c_str());
		}
		else
		{
			std::fprintf(
				stdout, "couldn't load symbol %s from '%s'\n",
				ganymede::CREATE_PLUGIN_PROC_NAME, filename.c_str());
		}

		FreeLibrary(handle);
		return nullptr;
	}

	return (ganymede::CreatePluginProc)symbol;
}

#endif
