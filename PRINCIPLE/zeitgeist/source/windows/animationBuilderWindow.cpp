// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "zeitgeist/application.hpp"
#include "chomsky/world/query/actorTileSpacePointCloudMatchExecutor.hpp"
#include "principle/deps/imgui/imgui.h"

zeitgeist::Application::AnimationBuilderWindow::AnimationBuilderWindow(Application* application) :
	principle::Window(application)
{
	this->application = application;
}

void zeitgeist::Application::AnimationBuilderWindow::add(const thoreau::WorldCoordinate& location)
{
	auto& actor_point_cloud_state = this->application->get_game_manager().get_actor_point_cloud_state();
	if (!actor_point_cloud_state.has(location))
	{
		return;
	}

	this->query = chomsky::TileSpacePointCloudMatch();

	auto& point_cloud = actor_point_cloud_state.get(location);
	for (auto& i: *point_cloud)
	{
		auto offset = i.first;
		auto& tile_point_cloud = i.second;

		this->query.set(offset, tile_point_cloud);
	}

	open();
}

void zeitgeist::Application::AnimationBuilderWindow::on_frame()
{
	if (!this->extract)
	{
		return;
	}

	chomsky::ActorTileSpacePointCloudMatchExecutor executor(
		&this->application->get_game_manager().get_animated_block());

	for (auto& i: this->application->get_game_manager().get_actor_point_cloud_state())
	{
		if (executor.execute(i.first, this->query))
		{
			auto& draw = this->application->get_game_manager().get_actor_draw_state().get(i.first);
			this->animation.extract(draw);

			if (this->dump_meta)
			{
				printf("START\n\n");

				for (std::size_t i = 0; i < draw.get_num_bones(); ++i)
				{
					auto bone = draw.get_bone(i);
					printf("BONE %lu\n", i);
					for (int i = 0; i < 4; ++i)
					{
						printf("%f %f %f %f\n", bone[i].x, bone[i].y, bone[i].z, bone[i].w);
					}

					glm::vec4 x = bone * glm::vec4(1, 0, 0, 1);
					printf("ROT_X %f %f %f\n", x.x, x.y, x.z);
					glm::vec4 y = bone * glm::vec4(0, 1, 0, 1);
					printf("ROT_Y %f %f %f\n", y.x, y.y, y.z);
					glm::vec4 z = bone * glm::vec4(0, 0, 1, 1);
					printf("ROT_Z %f %f %f\n", z.x, z.y, z.z);
					glm::vec4 t = bone * glm::vec4(0, 0, 0, 1);
					printf("TRANSLATION %f %f %f\n", t.x, t.y, t.z);

					printf("\n");
				}

				printf("\n");
			}
		}
	}
}

std::string zeitgeist::Application::AnimationBuilderWindow::get_title() const
{
	return "Animation Builder";
}

void zeitgeist::Application::AnimationBuilderWindow::update_gui()
{
	ImGui::Text("Frames: %lu", this->animation.get_num_frames());

	if (ImGui::Button("Clean"))
	{
		auto c = this->animation.clean();
		std::printf("removed %lu frames\n", c);
	}

	ImGui::SameLine();
	if (ImGui::Button("Clear"))
	{
		this->animation = chomsky::ActorAnimation();
	}

	ImGui::SameLine();
	if (ImGui::Button("Import"))
	{
		auto filename = this->application->get_input_filename();
		if (!filename.empty())
		{
			std::ifstream stream(filename, std::ios::binary);
			mantorok::DATReader reader(stream);
			mantorok::DATDeserializer<chomsky::ActorAnimation>::deserialize(
				this->animation, reader);
			this->hits = this->animation.get_hit_ratio_threshold();
			this->distance = this->animation.get_distance_threshold();
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("Export"))
	{
		auto filename = this->application->get_output_filename();
		if (!filename.empty())
		{
			std::ofstream stream(filename, std::ios::binary);
			mantorok::DATWriter writer(stream);
			mantorok::DATSerializer<chomsky::ActorAnimation>::serialize(
				this->animation, writer);
		}
	}

	ImGui::Checkbox("Automatic Extract", &this->extract);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Automatically extract frames from trace when replayed.");
	}

	ImGui::Checkbox("Dump Meta", &this->dump_meta);
	if (ImGui::IsItemHovered())
	{
		ImGui::SetTooltip("Dump frame meta information (e.g., matrix transforms) to stdout.");
	}

	ImGui::InputFloat("Hits", &this->hits);
	ImGui::InputFloat("Distance", &this->distance);

	this->animation.set_hit_ratio_threshold(this->hits);
	this->animation.set_distance_threshold(this->distance);

	if (ImGui::Button("Find Matches"))
	{
		find_matches();
	}
}

void zeitgeist::Application::AnimationBuilderWindow::find_matches()
{
	this->application->query_results_window.clear();

	auto& actor_draw_state = this->application->get_game_manager().get_actor_draw_state();
	for (auto& i: actor_draw_state)
	{
		chomsky::ActorAnimation other;
		other.extract(i.second);

		if (animation.match(other))
		{
			this->application->query_results_window.add_world(i.first);
		}
	}

	this->application->query_results_window.open();
}
