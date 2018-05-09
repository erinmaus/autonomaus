// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include "principle/common/gui.hpp"
#include "plato/application.hpp"
#include "plato/tools/selectTool.hpp"
#include "principle/deps/imgui/imgui.h"

plato::Application::FindResourceWindow::FindResourceWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
}

std::string plato::Application::FindResourceWindow::get_title() const
{
	return "Find Resource";
}

void plato::Application::FindResourceWindow::open(
	const std::string& type,
	const std::function<void(twoflower::Resource)>& callback)
{
	this->type = type;
	this->callback = callback;
	this->filter.clear();
	principle::Window::open();
}

void plato::Application::FindResourceWindow::update_gui()
{
	if (!this->application->atlas)
	{
		close();
		return;
	}

	ImGui::PushItemWidth(384);
	{
		ImGui::Text("Filter");
		ImGui::SameLine();
		ImGui::InputString("##filter", this->filter);
	}
	ImGui::PopItemWidth();

	auto resources = this->application->brochure->resources();
	std::size_t count = 0;
	for (auto i = resources.by_fuzzy_name(this->filter); i != resources.end(); ++i)
	{
		if (i->get_type() != this->type)
		{
			continue;
		}

		if (count > 100)
		{
			break;
		}
		++count;

		ImGui::PushID(i->get_id());
		ImGui::Bullet();
		ImGui::TextWrapped("%s (id: %d)", i->get_name().c_str(), i->get_id());

		if (ImGui::Button("View"))
		{
			this->application->tool_factory.view(*i);
		}

		ImGui::SameLine();
		if (ImGui::Button("Select"))
		{
			this->callback(*i);
			close();
		}

		ImGui::PopID();
	}
}
