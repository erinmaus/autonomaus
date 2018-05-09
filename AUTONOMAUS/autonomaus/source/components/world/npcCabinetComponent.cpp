// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <sstream>
#include <cstring>
#include "autonomaus/state/npcProvider.hpp"
#include "autonomaus/components/world/npcCabinetComponent.hpp"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "autonomaus/graphics/worldDrawHook.hpp"

autonomaus::NPCCabinetComponent::NPCCabinetComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	// Nothing.
}

void autonomaus::NPCCabinetComponent::show()
{
	this->visible = true;
}

void autonomaus::NPCCabinetComponent::reload()
{
	this->profiles.clear();
	this->npc_draws.clear();
}

void autonomaus::NPCCabinetComponent::update()
{
	if (this->visible)
	{
		draw();
	}
}

std::string autonomaus::NPCCabinetComponent::get_name() const
{
	return "NPC Cabinet";
}

void autonomaus::NPCCabinetComponent::draw()
{
	ImGui::Begin("NPC Cabinet", &this->visible);
	{
		ImGui::BeginGroup();
		{
			char input[256] = {};
			std::strncpy(input, this->pending_profile.c_str(), sizeof(input));
			if (ImGui::InputText("Profile", input, sizeof(input)))
			{
				this->pending_profile = input;
			}

			ImGui::SameLine();
			if (ImGui::Button("Add") && !this->pending_profile.empty())
			{
				if (this->profiles.count(this->pending_profile) == 0)
				{
					NPCProvider::add_profile(
						get_autonomaus().get_game_state(),
						get_autonomaus().get_npc_state(),
						this->pending_profile);
					this->profiles.insert(this->pending_profile);
				}

				this->pending_profile.clear();
			}
		}
		ImGui::EndGroup();

		ImGui::Separator();
		ImGui::Spacing();

		ImGui::Text("Profiles");
		ImGui::BeginGroup();
		{
			std::string profile_pending_removal;
			for (auto& profile: this->profiles)
			{
				ImGui::PushID(profile.c_str());

				ImGui::Bullet();
				if (ImGui::SmallButton("-"))
				{
					NPCProvider::remove_profile(
						get_autonomaus().get_game_state(),
						get_autonomaus().get_npc_state(),
						profile);
					profile_pending_removal = profile;
				}
				ImGui::SameLine();
				ImGui::Text("%s", profile.c_str());

				ImGui::PopID();
			}

			if (!profile_pending_removal.empty())
			{
				this->profiles.erase(profile_pending_removal);
			}
		}
		ImGui::EndGroup();
	}
	ImGui::End();

	draw_npcs();
}

void autonomaus::NPCCabinetComponent::draw_npcs()
{
	std::set<glooper::NPC::ID> unvisited;
	for (auto& i: this->npc_draws)
	{
		unvisited.insert(i.first);
	}

	auto& npcs = *get_autonomaus().get_npc_state();
	auto& gui = get_autonomaus().get_gui_draw_hook().before();
	auto& world = get_autonomaus().get_world_draw_hook().after();
	for (auto& i: npcs)
	{
		auto& draw = this->npc_draws[i.first];
		auto& npc = i.second;

		if (!npc.get_is_on_screen())
		{
			continue;
		}

		gui.start(draw.first);
		{
			std::stringstream stream;
			stream << "profile: " << npc.get_profile_name() << std::endl;
			stream << "variant: " << npc.get_profile_variant() << std::endl;
			stream << "id:" << npc.get_id() << std::endl;

			stream << "animations:";
			auto& animations = npc.get_animations();
			if (animations.empty())
			{
				stream << " (none)" << std::endl;
			}
			else
			{
				for (auto& animation: npc.get_animations())
				{
					stream << " " << animation;
				}
			}

			gui.draw_text(
				stream.str(),
				npc.get_position() + glm::vec3(8),
				glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
			gui.draw_text(stream.str(), npc.get_position());
		}
		gui.stop();

		world.start(draw.second);
		{
			world.draw_box_outline(
				npc.get_model_matrix(),
				npc.get_bounds().compute_size(),
				glm::vec4(1.0f));
		}
		world.stop();

		unvisited.erase(i.first);
	}

	for (auto i: unvisited)
	{
		this->npc_draws.erase(i);
	}
}
