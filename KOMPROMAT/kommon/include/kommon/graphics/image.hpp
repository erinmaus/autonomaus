// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMMON_GRAPHICS_IMAGE_HPP
#define KOMMON_GRAPHICS_IMAGE_HPP

#include <string>
#include <vector>
#include <glm/glm.hpp>

namespace kommon
{
	class Image
	{
	public:
		Image();
		Image(int width, int height);
		~Image() = default;

		int get_width() const;
		int get_height() const;

		std::uint8_t* get_pixels();
		const std::uint8_t* get_pixels() const;
		std::size_t get_pixels_size() const;

		glm::vec4 get_pixel(int x, int y) const;
		void set_pixel(int x, int y, const glm::vec4& value);

		void draw_line(int x1, int y1, int x2, int y2, const glm::vec4& color);

		void save_png(const std::string& filename) const;
		void load_png(const std::string& filename);

	private:
		int width, height;
		std::vector<glm::u8vec4> pixels;
	};
}

#endif
