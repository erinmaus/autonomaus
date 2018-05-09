// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2016 [bk]door.maus

#include <stdexcept>
#include <GL/glew.h>
#include <allegro5/allegro_opengl.h>
#include "principle/graphics/display.hpp"

const int principle::Display::DEFAULT_WIDTH = 800;
const int principle::Display::DEFAULT_HEIGHT = 600;

principle::Display::Display(
	ALLEGRO_EVENT_QUEUE* queue, int x, int y, int width, int height)
{
	al_set_new_display_flags(
		ALLEGRO_WINDOWED | ALLEGRO_RESIZABLE | ALLEGRO_OPENGL_3_0);
	al_set_new_window_position(x, y);

	al_set_new_display_option(ALLEGRO_STENCIL_SIZE, 8, ALLEGRO_REQUIRE);

	this->display = al_create_display(width, height);
	if (this->display == nullptr)
	{
		throw std::runtime_error("couldn't create display");
	}

	if (glewInit() != GLEW_OK)
	{
		throw std::runtime_error("couldn't initialiaze GLEW");
	}

	al_register_event_source(queue, al_get_display_event_source(this->display));
}

principle::Display::~Display()
{
	if (this->display != nullptr)
	{
		al_destroy_display(this->display);
	}
}

void principle::Display::resize(int width, int height)
{
	al_resize_display(this->display, width, height);
}

void principle::Display::acknowledge_resize()
{
	al_acknowledge_resize(this->display);
}

void principle::Display::save_screenshot(const std::string& filename)
{
	ALLEGRO_BITMAP* backbuffer = al_get_backbuffer(this->display);
	al_save_bitmap(filename.c_str(), backbuffer);
}

int principle::Display::get_x() const
{
	int x, y;
	al_get_window_position(this->display, &x, &y);

	return x;
}

int principle::Display::get_y() const
{
	int x, y;
	al_get_window_position(this->display, &x, &y);

	return y;
}

int principle::Display::get_width() const
{
	return al_get_display_width(this->display);
}

int principle::Display::get_height() const
{
	return al_get_display_height(this->display);
}

void principle::Display::flip()
{
	al_flip_display();
}
