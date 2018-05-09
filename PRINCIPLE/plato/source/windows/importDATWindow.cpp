// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <boost/regex.hpp>
#include "principle/common/gui.hpp"
#include "principle/deps/imgui/imgui.h"
#include "plato/application.hpp"

plato::Application::ImportDATWindow::ImportDATWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
}

std::string plato::Application::ImportDATWindow::get_title() const
{
	return "Import DAT";
}

void plato::Application::ImportDATWindow::update_gui()
{
	if (!this->application->brochure)
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Brochure not loaded. Woops.");
		return;
	}

	ImGui::PushItemWidth(150);
	{
		ImGui::Text("Type");
		ImGui::SameLine();
		ImGui::InputString("##type", this->action_type);

		ImGui::SameLine();
		ImGui::Text("Name");
		ImGui::SameLine();
		ImGui::InputString("##name", this->action_name);

		ImGui::Text("Skill");
		ImGui::SameLine();
		ImGui::InputString("##skill", this->action_skill);
	}
	ImGui::PopItemWidth();

	if (!this->application->brochure->has_action_definition(this->action_type, this->action_name))
	{
		ImGui::TextColored(
			ImVec4(1, 0, 0, 1),
			"Action not found.");
	}
	else
	{
		if (ImGui::Button("Import..."))
		{
			auto filename = this->application->get_input_filename();
			if (!filename.empty())
			{
				results.clear();
				import(filename);
			}
		}
	}

	ImGui::Text("Result");
	ImGui::Separator();
	ImGui::Spacing();

	if (results.empty())
	{
		ImGui::Text("(none)");
	}
	else
	{
		for (auto& result: results)
		{
			ImGui::BulletText("%s", result.c_str());
		}
	}
}

void plato::Application::ImportDATWindow::import(const std::string& filename)
{
	std::ifstream stream(filename);

	twoflower::Resource current_resource;
	while (stream.good())
	{
		std::string keyword;
		stream >> keyword;

		if (keyword == "ACTION")
		{
			std::string unused;
			std::getline(stream, unused);
		}
		else if (keyword == "REQUIRE" || keyword == "INPUT")
		{
			int quantity;
			stream >> quantity;

			std::string thing;
			{
				stream >> thing;
				std::string s;
				std::getline(stream, s);
				thing += s;
			}

			bool is_skill = false;
			if (keyword == "REQUIRE")
			{
				boost::regex skill("(.*) Level");
				boost::regex quest(".*[Cc]omplete.*?'(.+)'");
				boost::smatch m;

				if (boost::regex_match(thing, m, skill))
				{
					thing = m[1];
					is_skill = true;
				}
				else if (boost::regex_match(thing, m, quest))
				{
					thing = m[1];
				}
			}

			if (thing == "RuneScape Member")
			{
				continue;
			}

			if (current_resource.get_id())
			{
				auto actions = this->application->brochure->actions(current_resource);
				auto action = *actions.by_type(this->action_type, this->action_name);

				auto resources = this->application->brochure->resources();
				auto begin = resources.by_name(thing);
				auto end = resources.end();

				if (begin == end)
				{
					std::fprintf(
						stderr,
						"resource '%s' not found, cannot add requirement to '%s'\n",
						thing.c_str(),
						current_resource.get_name().c_str());
					continue;
				}

				auto resource = *begin;
				if (++begin != end)
				{
					std::fprintf(
						stderr,
						"warning, resource '%s' is ambiguous\n",
						thing.c_str(),
						current_resource.get_name().c_str());
					continue;
				}

				twoflower::Requirement requirement;

				if (keyword == "INPUT")
				{
					requirement.builder().set_is_input(true);
					requirement.builder().set_count(quantity);
				}
				else if (is_skill)
				{
					requirement.builder().set_count(mapp::xp_for_level(quantity));
				}

				this->application->brochure->builder().connect(requirement, action, resource);
			}
		}
		else
		{
			float xp;
			stream >> xp;

			std::string resource;
			{
				stream >> resource;
				std::string s;
				std::getline(stream, s);
				resource += s;
			}

			int quantity = 1;
			{
				boost::regex e("(.*) x(\\d*)");
				boost::smatch m;

				if (boost::regex_match(resource, m, e))
				{
					resource = m[1];
					quantity = std::atoi(((std::string)(m[2])).c_str());
				}
			}

			auto resources = this->application->brochure->resources();
			auto begin = resources.by_type(keyword);
			auto end = resources.end();

			if (begin != end)
			{
				while (begin->get_name() != resource && begin != end)
				{
					++begin;
				}

				if (begin == end)
				{
					current_resource = twoflower::Resource();
					std::fprintf(
						stderr,
						"resource %s (type: %s) not found, resolve manually\n",
						resource.c_str(),
						keyword.c_str());
					continue;
				}
				else
				{
					current_resource = *begin;
				}

				bool ambiguous = false;
				while (++begin != end)
				{
					if (begin->get_name() == resource)
					{
						mapp::Item a, b;
						if (this->application->game->item(*begin, a) &&
							this->application->game->item(current_resource, b))
						{
							if (a.object_id < b.object_id)
							{
								current_resource = *begin;
							}

						}

						ambiguous = true;
					}
				}

				if (ambiguous)
				{
					std::fprintf(
						stderr,
						"resource %s (type: %s) is ambiguous, resolve manually\n",
						resource.c_str(),
						keyword.c_str());
				}

				this->results.push_back(resource);

				auto actions = this->application->brochure->actions(current_resource);
				for (auto i = actions.by_type(this->action_type, this->action_name); i != actions.end(); ++i)
				{
					this->application->brochure->builder().remove_action(*i);
				}

				twoflower::Action action;
				action.builder().set_type(this->action_type);
				action.builder().set_name(this->action_name);
				action = this->application->brochure->builder().connect(action, current_resource);

				auto skill = resources.by_type("skill");
				while (skill != end)
				{
					if (skill->get_name() == this->action_skill)
					{
						twoflower::Requirement requirement;
						requirement.builder().set_is_output(true);
						requirement.builder().set_count(xp);
						this->application->brochure->builder().connect(requirement, action, *skill);
					}

					++skill;
				}

				twoflower::Requirement output;
				output.builder().set_is_output(true);
				output.builder().set_count(quantity);
				this->application->brochure->builder().connect(output, action, current_resource);
			}
		}
	}
}
