// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_DRAW_PARAMETERS_HPP
#define KOMPVTER_GRAPHICS_DRAW_PARAMETERS_HPP

#include <cstddef>

namespace kompvter
{
	class DrawParameters
	{
	public:
		enum
		{
			primitive_none = 0,
			primitive_triangles,
			primitive_triangle_fan,
			primitive_triangle_strip,
			primitive_points,
			primitive_lines,
			primitive_line_strip,
			primitive_line_loop,
			primitive_max
		};

		DrawParameters() = default;
		DrawParameters(
			int primitive,
			std::size_t offset_start, std::size_t offset_stop);
		~DrawParameters() = default;

		int get_primitive_type() const;
		std::size_t get_offset_start() const;
		std::size_t get_offset_stop() const;

		bool operator ==(const DrawParameters& other) const;
		bool operator !=(const DrawParameters& other) const;

	private:
		int primitive_type = primitive_none;
		std::size_t offset_start = 0, offset_stop = 0;
	};
}

#endif
