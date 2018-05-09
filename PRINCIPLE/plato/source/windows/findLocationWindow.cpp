// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include "plato/application.hpp"
#include "plato/tools/selectTool.hpp"
#include "principle/deps/imgui/imgui.h"

plato::Application::FindLocationWindow::FindLocationWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
}

std::string plato::Application::FindLocationWindow::get_title() const
{
	return "Find Locations";
}

void plato::Application::FindLocationWindow::open(
	const std::function<void(mapp::Location)>& callback)
{
	this->callback = callback;
	this->address.clear();
	principle::Window::open();
}

void plato::Application::FindLocationWindow::update_gui()
{
	if (!this->application->atlas)
	{
		close();
		return;
	}

	char input[512] = {};
	std::strcpy(input, this->address.c_str());
	if (ImGui::InputText("Address", input, sizeof(input)))
	{
		this->address = input;
	}

	if (ImGui::Button("Click Location..."))
	{
		auto tool = new SelectTool(
			*this->application->toolbox,
			[this](auto location)
			{
				this->callback(location);
				this->close();
			});
		this->application->toolbox->use(tool);
	}

	ImGui::Separator();
	ImGui::Spacing();

	auto results = this->application->atlas->fuzzy_find(this->address);
	if (results.empty())
	{
		ImGui::BulletText("(none)");
	}

	std::size_t count = 0;
	for (auto& i: results)
	{
		ImGui::Bullet();

		ImGui::SameLine();
		if (ImGui::Button("Select"))
		{
			this->callback(i);
			close();
		}

		ImGui::SameLine();
		ImGui::TextWrapped(
			"%s (id: %d)",
			this->application->atlas->address(i).c_str(),
			i.resource.get_id());

		if (count > 5)
		{
			break;
		}
		++count;
	}
}
