// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include "plato/application.hpp"
#include "plato/utility.hpp"
#include "plato/tools/complexShapeTool.hpp"
#include "principle/deps/imgui/imgui.h"

plato::Application::ViewActionWindow::ViewActionWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
	this->id = ++this->application->current_window_id;
}

plato::Application::ViewActionWindow::ViewActionWindow(
	const twoflower::Action& action,
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	this->id = ++this->application->current_window_id;
	add(action);
}

void plato::Application::ViewActionWindow::add(const twoflower::Action& action)
{
	this->actions.push_back(action);
	open();
}

void plato::Application::ViewActionWindow::draw()
{
	if (get_is_open())
	{
		auto position = ImVec2(
			this->application->current_mouse_x,
			this->application->current_mouse_y);
		ImGui::SetNextWindowPos(position, ImGuiSetCond_Appearing);
		ImGui::SetNextWindowSize(ImVec2(320, 480), ImGuiSetCond_Appearing);
	}

	principle::Window::draw();
}

void plato::Application::ViewActionWindow::close()
{
	this->application->remove_window(this);
	principle::Window::close();
}

int plato::Application::ViewActionWindow::get_flags() const
{
	return ImGuiWindowFlags_NoSavedSettings;
}

std::string plato::Application::ViewActionWindow::get_title() const
{
	return "View Action";
}

std::string plato::Application::ViewActionWindow::get_id() const
{
	return principle::Window::get_id() + "@" + std::to_string(this->id);
}

void plato::Application::ViewActionWindow::update_gui()
{
	for (std::size_t i = 0; i < this->actions.size(); ++i)
	{
		this->application->brochure_widget_manager->view(this->actions[i]);
		if (i + 1 < this->actions.size())
		{
			ImGui::Separator();
			ImGui::Spacing();
		}
	}
}
