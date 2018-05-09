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

plato::Application::ViewResourceWindow::ViewResourceWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
	this->id = ++this->application->current_window_id;
}

plato::Application::ViewResourceWindow::ViewResourceWindow(
	const twoflower::Resource& resource,
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	this->id = ++this->application->current_window_id;
	add(resource);
	open();
}

void plato::Application::ViewResourceWindow::add(const twoflower::Resource& resource)
{
	this->resources.push_back(resource);
}

void plato::Application::ViewResourceWindow::draw()
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

void plato::Application::ViewResourceWindow::close()
{
	this->application->remove_window(this);
	principle::Window::close();
}

int plato::Application::ViewResourceWindow::get_flags() const
{
	return ImGuiWindowFlags_NoSavedSettings;
}

std::string plato::Application::ViewResourceWindow::get_title() const
{
	return "View Resource";
}

std::string plato::Application::ViewResourceWindow::get_id() const
{
	return principle::Window::get_id() + "@" + std::to_string(this->id);
}

void plato::Application::ViewResourceWindow::update_gui()
{
	for (std::size_t i = 0; i < this->resources.size(); ++i)
	{
		this->application->brochure_widget_manager->view(this->resources[i]);
		if (i + 1 < this->resources.size())
		{
			ImGui::Separator();
			ImGui::Spacing();
		}
	}
}
