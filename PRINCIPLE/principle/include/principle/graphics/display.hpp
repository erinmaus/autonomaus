// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#ifndef PRINCIPLE_GRAPHICS_DISPLAY_HPP
#define PRINCIPLE_GRAPHICS_DISPLAY_HPP

#include <mutex>
#include <string>
#include <allegro5/allegro.h>

namespace principle
{
	class Display
	{
	public:
		static const int DEFAULT_WIDTH;
		static const int DEFAULT_HEIGHT;

		Display(ALLEGRO_EVENT_QUEUE* queue, int x, int y, int width, int height);
		~Display();

		void resize(int width, int height);
		void acknowledge_resize();

		void save_screenshot(const std::string& filename);

		int get_x() const;
		int get_y() const;
		int get_width() const;
		int get_height() const;

		void flip();

	private:
		std::mutex mutex;
		ALLEGRO_DISPLAY* display = nullptr;
	};
}

#endif
