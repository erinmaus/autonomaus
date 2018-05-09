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

plato::Application::EditResourceWindow::EditResourceWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
	this->id = ++this->application->current_window_id;
}

plato::Application::EditResourceWindow::EditResourceWindow(
	const twoflower::Resource& resource,
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	this->id = ++this->application->current_window_id;
	add(resource);
}

void plato::Application::EditResourceWindow::add(const twoflower::Resource& resource)
{
	this->resources.push_back(resource);
	open();
}

void plato::Application::EditResourceWindow::draw()
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

void plato::Application::EditResourceWindow::close()
{
	this->application->remove_window(this);
	principle::Window::close();
}

int plato::Application::EditResourceWindow::get_flags() const
{
	return ImGuiWindowFlags_NoSavedSettings;
}

std::string plato::Application::EditResourceWindow::get_title() const
{
	return "Edit Resource";
}

std::string plato::Application::EditResourceWindow::get_id() const
{
	return principle::Window::get_id() + "@" + std::to_string(this->id);
}

void plato::Application::EditResourceWindow::update_gui()
{
	for (std::size_t i = 0; i < this->resources.size(); ++i)
	{
		this->application->brochure_widget_manager->edit(this->resources[i]);
		if (i + 1 < this->resources.size())
		{
			ImGui::Separator();
			ImGui::Spacing();
		}
	}
}
