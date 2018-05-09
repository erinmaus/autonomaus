// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_COMMON_UTILITY_HPP
#define CELIA_COMMON_UTILITY_HPP

#include <cstddef>

namespace celia { namespace utility
{
	std::size_t get_texture_pixel_data_size(
		int format, int type,
		int width, int height = 1, int depth = 1);
	std::size_t get_pname_count(int pname);
} }

#endif
