// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <limits>
#include "autonomaus/core/rng.hpp"

autonomaus::RNG::RNG()
{
	state.a = time(NULL);
	for (auto i = 0; i < 20; ++i)
	{
		iterate();
	}
}

std::uint32_t autonomaus::RNG::next()
{
	return instance.iterate();
}

std::uint32_t autonomaus::RNG::next(std::uint32_t max)
{
	return instance.iterate() % max;
}

std::uint32_t autonomaus::RNG::next(std::uint32_t min, std::uint32_t max)
{
	auto difference = max - min;
	return min + instance.iterate() % difference;
}

float autonomaus::RNG::nextf()
{
	return (float)instance.iterate() / std::numeric_limits<std::uint32_t>::max();
}

float autonomaus::RNG::nextf(float max)
{
	return nextf() * max;
}

float autonomaus::RNG::nextf(float min, float max)
{
	auto difference = max - min;
	return min + nextf() * difference;
}

std::uint32_t autonomaus::RNG::iterate()
{
	std::unique_lock<std::mutex> lock(this->mutex);

#define rotate(x,k) (((x) << (k)) | ((x) >> (32 - (k))))
	auto e = state.a - rotate(state.b, 27);
	state.a = state.b ^ rotate(state.c, 17);
	state.b = state.c + state.d;
	state.c = state.d + e;
	state.d = e + state.a;
#undef rotate

	return state.d;
}

autonomaus::RNG autonomaus::RNG::instance;
