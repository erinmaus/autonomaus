// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2018 [bk]door.maus

#ifndef TOUCHSTONE_PLATFORM_WINDOWS_HPP
#define TOUCHSTONE_PLATFORM_WINDOWS_HPP

#include <cstdio>
#include <cassert>
#include <utility>

namespace touchstone
{
	namespace windows
	{
		#ifdef TOUCHSTONE_BUILD_WINDOWS

		void register_hook(const char* glFuncName, void* origFunc);
		void* get_hook(const char* glFuncName);

		#endif
	}
}

#endif
