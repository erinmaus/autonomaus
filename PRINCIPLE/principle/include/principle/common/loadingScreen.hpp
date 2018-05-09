// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_COMMON_LOADING_SCREEN_HPP
#define PRINCIPLE_COMMON_LOADING_SCREEN_HPP

#include <mutex>
#include <thread>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include "principle/graphics/display.hpp"

namespace principle
{
	class LoadingScreen
	{
	public:
		LoadingScreen(
			ALLEGRO_EVENT_QUEUE* queue, Display& display,
			int num_frames, int num_draw_frames);
		~LoadingScreen();

		bool get_quit_pending() const;
		void next_frame(bool is_draw);
		void set_message(const std::string& message);
		void update();

	private:
		ALLEGRO_EVENT_QUEUE* queue;
		Display* display;

		static std::string get_resource_filename(const std::string& resource);

		int num_frames = 0;
		int num_draw_frames = 0;
		int target_frames = 0;
		int target_draw_frames = 0;

		float last_frame = 0.0f;
		float draw_time = 0.0f;

		bool quit_pending = false;

		ALLEGRO_FONT* font = nullptr;
		ALLEGRO_BITMAP* spinner_foreground = nullptr;
		ALLEGRO_BITMAP* spinner_background = nullptr;

		std::string message;

		void draw();
	};
}

#endif
