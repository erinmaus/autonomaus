// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_CORE_RNG_HPP
#define AUTONOMAUS_CORE_RNG_HPP

#include <mutex>
#include <cstdint>

namespace autonomaus
{
	// http://burtleburtle.net/bob/rand/smallprng.html
	struct RNG
	{
	public:
		static std::uint32_t next();
		static std::uint32_t next(std::uint32_t max);
		static std::uint32_t next(std::uint32_t min, std::uint32_t max);

		static float nextf();
		static float nextf(float max);
		static float nextf(float min, float max);

	private:
		RNG();
		~RNG() = default;

		std::uint32_t iterate();

		std::mutex mutex;
		struct
		{
			std::uint32_t a, b, c, d;
		} state;

		static RNG instance;
	};
}

#endif
