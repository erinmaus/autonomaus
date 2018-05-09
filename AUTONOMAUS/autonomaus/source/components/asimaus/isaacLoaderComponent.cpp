// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/filesystem.hpp>
#include "autonomaus/components/asimaus/isaacLoaderComponent.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "autonomaus/state/gameStateManager.hpp"

autonomaus::IsaacLoaderComponent::IsaacLoaderComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

void autonomaus::IsaacLoaderComponent::show()
{
	this->visible = true;
}

void autonomaus::IsaacLoaderComponent::update()
{
	for (auto i: this->loaded)
	{
		i.second->update();
	}

	if (this->visible)
	{
		draw();
	}
}

void autonomaus::IsaacLoaderComponent::reload()
{
	// Nothing.
}

std::string autonomaus::IsaacLoaderComponent::get_name() const
{
	return "Isaac Loader";
}

void autonomaus::IsaacLoaderComponent::draw()
{
	ImGui::Begin("Isaac Loader", &this->visible);
	{
		ImGui::Text("Loaded Modules");
		if (this->loaded.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			std::string pending_removal;

			for (auto& i: this->loaded)
			{
				ImGui::PushID(i.first.c_str());

				ImGui::BulletText("%s", i.first.c_str());

				ImGui::SameLine();
				if (ImGui::SmallButton("Reload"))
				{
					Isaac::unload(i.second);
					auto result = load_choice(i.first);
					if (result != nullptr)
					{
						i.second = result;
					}
					else
					{
						pending_removal = i.first;
					}
				}

				ImGui::SameLine();
				if (ImGui::SmallButton("Unload"))
				{
					Isaac::unload(i.second);
					pending_removal = i.first;
				}

				ImGui::PopID();
			}

			this->loaded.erase(pending_removal);
		}

		ImGui::Separator();
		ImGui::Spacing();

		auto choices = get_choices();
		if (choices.empty())
		{
			ImGui::BulletText("(none)");
		}
		else
		{
			for (auto module: choices)
			{
				ImGui::PushID(module.c_str());

				ImGui::BulletText("%s", module.c_str());
				if (this->loaded.count(module) == 0)
				{
					ImGui::SameLine();
					if (ImGui::SmallButton("Load"))
					{
						auto result = load_choice(module);
						if (result != nullptr)
						{
							this->loaded.emplace(module, result);
						}
					}
				}

				ImGui::PopID();
			}
		}
	}
	ImGui::End();
}

static boost::filesystem::path make_relative(
	const boost::filesystem::path& from,
	const boost::filesystem::path& to)
{
	boost::filesystem::path::const_iterator from_iter = from.begin();
	boost::filesystem::path::const_iterator to_iter = to.begin();

	while (from_iter != from.end() && to_iter != to.end() && (*to_iter) == (*from_iter))
	{
		++to_iter;
		++from_iter;
	}

	boost::filesystem::path result;
	while (from_iter != from.end())
	{
		result /= "..";
		++from_iter;
	}

	while (to_iter != to.end())
	{
		result /= *to_iter;
		++to_iter;
	}

	return result;
}

std::set<std::string> autonomaus::IsaacLoaderComponent::get_choices()
{
	auto& resource_manager = get_autonomaus().get_game_state().get_resource_manager();

	boost::filesystem::path isaac_path(Autonomaus::get_config_path("isaac"));

	std::set<std::string> files;
	resource_manager.for_each_file(
		isaac_path.string(),
		[&files, &isaac_path](const std::string& filename)
		{
			auto choice = make_relative(isaac_path, filename);
			files.insert(choice.string());
		});

	return files;
}

autonomaus::Isaac* autonomaus::IsaacLoaderComponent::load_choice(const std::string& name)
{
	boost::filesystem::path path(Autonomaus::get_config_path("isaac"));
	path.append(name);

	return autonomaus::Isaac::load(get_autonomaus(), path.string());
}
