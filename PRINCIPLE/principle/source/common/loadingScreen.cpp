// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cmath>
#include <cstdlib>
#include <GL/glew.h>
#include <allegro5/allegro5.h>
#include <allegro5/allegro_font.h>
#include <allegro5/allegro_image.h>
#include <allegro5/allegro_ttf.h>
#include "principle/common/loadingScreen.hpp"

principle::LoadingScreen::LoadingScreen(
	ALLEGRO_EVENT_QUEUE* queue, Display& display,
	int num_frames, int num_draw_frames) :
		queue(queue),
		display(&display),
		target_frames(num_frames),
		target_draw_frames(num_draw_frames),
		last_frame(al_get_time())
{
	this->font = al_load_font(get_resource_filename("font.ttf").c_str(), 18, 0);
	this->spinner_foreground = al_load_bitmap(get_resource_filename("spinner_foreground.png").c_str());
	this->spinner_background = al_load_bitmap(get_resource_filename("spinner_background.png").c_str());
}

principle::LoadingScreen::~LoadingScreen()
{
	al_destroy_font(this->font);
	al_destroy_bitmap(this->spinner_foreground);
	al_destroy_bitmap(this->spinner_background);
}

bool principle::LoadingScreen::get_quit_pending() const
{
	return this->quit_pending;
}

void principle::LoadingScreen::next_frame(bool is_draw)
{
	++this->num_frames;
	
	float current_time = al_get_time();
	if (is_draw)
	{
		float difference = current_time - this->last_frame;
		this->draw_time += difference;
		++this->num_draw_frames;
	}

	this->last_frame = current_time;

	update();
}

void principle::LoadingScreen::set_message(const std::string& value)
{
	this->message = value;
}

void principle::LoadingScreen::update()
{
	bool can_draw = false;
	while (!al_is_event_queue_empty(this->queue))
	{
		ALLEGRO_EVENT event;
		al_wait_for_event(this->queue, &event);

		switch (event.type)
		{
			case ALLEGRO_EVENT_DISPLAY_RESIZE:
				this->display->acknowledge_resize();
				break;
			case ALLEGRO_EVENT_DISPLAY_CLOSE:
				this->quit_pending = true;
				break;
			case ALLEGRO_EVENT_TIMER:
				can_draw = true;
				break;
		}
	}

	if (can_draw)
	{
		draw();
	}
}

void principle::LoadingScreen::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT);
	glViewport(0, this->display->get_height(), this->display->get_width(), 0);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_SCISSOR_TEST);
	glDisable(GL_STENCIL_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	glBindVertexArray(0);
	glUseProgram(0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

	if (this->spinner_background != nullptr &&
		this->spinner_foreground != nullptr)
	{
		float rotation = -(al_get_time() * M_PI);
		al_draw_rotated_bitmap(
			this->spinner_background,
			al_get_bitmap_width(this->spinner_background) / 2.0f,
			al_get_bitmap_height(this->spinner_background) / 2.0f,
			this->display->get_width() / 2.0f,
			this->display->get_height() / 2.0f,
			0.0f, 0);
		al_draw_rotated_bitmap(
			this->spinner_foreground,
			al_get_bitmap_width(this->spinner_foreground) / 2.0f,
			al_get_bitmap_height(this->spinner_foreground) / 2.0f,
			this->display->get_width() / 2.0f,
			this->display->get_height() / 2.0f,
			rotation, 0);
	}

	if (this->font != nullptr && this->num_frames != 0)
	{
		al_draw_textf(
			this->font, al_map_rgb_f(1, 1, 1),
			0, 0, ALLEGRO_ALIGN_LEFT,
			"frame %d of %d (%0.0f%% done)",
			this->num_frames, this->target_frames,
			(float)this->num_frames / this->target_frames);

		if (this->num_draw_frames > 0)
		{
			float time_per_frame = this->draw_time / this->num_draw_frames;
			float remaining_time = (this->target_draw_frames - this->num_draw_frames) * time_per_frame;

			if (remaining_time > 60.0f)
			{
				al_draw_textf(
					this->font, al_map_rgb_f(1, 1, 1),
					0, 24, ALLEGRO_ALIGN_LEFT,
					"estimated time remaining: % 2d minutes % 2d seconds",
					((int)remaining_time) / 60, ((int)remaining_time) % 60);
			}
			else
			{
				al_draw_textf(
					this->font, al_map_rgb_f(1, 1, 1),
					0, 24, ALLEGRO_ALIGN_LEFT,
					"estimated time remaining: % 2d seconds",
					(int)remaining_time);
			}
		}
	}

	if (this->font != nullptr && !this->message.empty())
	{
		al_draw_textf(
			this->font, al_map_rgb_f(1, 1, 1),
			0, this->display->get_height() - 24, ALLEGRO_ALIGN_LEFT,
			"%s",
			this->message.c_str());
	}

	this->display->flip();
}

std::string principle::LoadingScreen::get_resource_filename(
	const std::string& resource)
{
	std::string prefix;
	{
		const char* share_path = std::getenv("HDT_SHARE");
		if (share_path == nullptr)
		{
			prefix = "";
		}
		else
		{
			prefix = std::string(share_path) + "/principle/resources/";
		}
	}

	return prefix + resource;
}
