// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/application.hpp"
#include "principle/common/window.hpp"
#include "principle/deps/imgui/imgui.h"

principle::Window::Window(Application* application)
{
	this->application = application;
	this->application->add_window(this);
}

principle::Window::~Window()
{
	this->application->remove_window(this);
}

void principle::Window::open()
{
	this->focus = true;
	this->is_open = true;
}

void principle::Window::close()
{
	this->is_open = false;
}

bool principle::Window::get_is_open() const
{
	return this->is_open;
}

bool principle::Window::get_is_closed() const
{
	return !this->is_open;
}

void principle::Window::update(float timestamp)
{
	// Nothing.
}

void principle::Window::draw()
{
	auto app_name = this->application->get_name();
	auto window_title = get_title() + "###" + get_id();

	if (this->is_open)
	{
		if (this->focus)
		{
			ImGui::SetNextWindowFocus();
			this->focus = false;
		}

		if (this->can_close)
		{
			ImGui::Begin(window_title.c_str(), &this->is_open, get_flags());
		}
		else
		{
			ImGui::Begin(window_title.c_str(), nullptr, get_flags());
		}

		update_gui();

		ImGui::End();
	}
}

bool principle::Window::get_can_close() const
{
	return this->can_close;
}

void principle::Window::set_can_close(bool value)
{
	this->can_close = value;

	if (!value)
	{
		open();
	}
}

int principle::Window::get_flags() const
{
	return 0;
}

std::string principle::Window::get_title() const
{
	return "Window";
}

std::string principle::Window::get_id() const
{
	return this->application->get_name() + ":" + get_title();
}

std::string principle::Window::get_id(const std::string& suffix) const
{
	return get_id() + ":" + suffix;
}
