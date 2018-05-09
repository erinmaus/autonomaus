// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/time.hpp"

#ifdef AUTONOMAUS_BUILD_POSIX
#include <time.h>
#include <unistd.h>

float autonomaus::get_current_time()
{
	struct timespec t;
	clock_gettime(CLOCK_MONOTONIC, &t);

	return t.tv_sec + t.tv_nsec / 1000000000.0f;
}

#endif

#ifdef AUTONOMAUS_BUILD_WINDOWS
#include <windows.h>

float autonomaus::get_current_time()
{
	LARGE_INTEGER value;
	if (!QueryPerformanceFrequency(&value))
	{
		return 0.0f;
	}

	return value.QuadPart / 1000.0f;
}

#endif

