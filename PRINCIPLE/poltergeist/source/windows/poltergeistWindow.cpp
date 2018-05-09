// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "poltergeist/application.hpp"
#include "principle/deps/imgui/imgui.h"

poltergeist::Application::PoltergeistWindow::PoltergeistWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
	open();
	set_can_close(false);
}

std::string poltergeist::Application::PoltergeistWindow::get_title() const
{
	return "POLTERGEIST";
}

void poltergeist::Application::PoltergeistWindow::update_gui()
{
	if (ImGui::Button("Element Stream"))
	{
		this->application->gui_draws_window.open();
	}

	ImGui::SameLine();
	if (ImGui::Button("State"))
	{
		this->application->gui_state_window.open();
	}

	ImGui::SameLine();
	if (ImGui::Button("Item Icons"))
	{
		this->application->item_icons_window.open();
	}

	ImGui::Separator();

	if (ImGui::Button("Diff Tool"))
	{
		this->application->diff_window.open();
	}

	ImGui::Separator();
}
