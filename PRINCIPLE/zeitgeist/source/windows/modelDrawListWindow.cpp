// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <algorithm>
#include <vector>
#include "principle/deps/imgui/imgui.h"
#include "principle/resources/draw.hpp"
#include "kvre/model/batchedModelDraws.hpp"
#include "kvre/profile/profile.hpp"
#include "zeitgeist/application.hpp"

zeitgeist::Application::ModelDrawListWindow::ModelDrawListWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;

	set_can_close(false);
}

std::string zeitgeist::Application::ModelDrawListWindow::get_title() const
{
	return "Model Draws";
}

void zeitgeist::Application::ModelDrawListWindow::update_gui()
{
	ImGui::BeginGroup();
	{
		if (ImGui::Button("View Actors"))
		{
			this->application->actor_draw_list_window.open();
		}
	}
	ImGui::EndGroup();

	ImGui::Text("Filters");
	ImGui::BeginGroup();
	{
		ImGui::SameLine();
		ImGui::Checkbox("Unclassified", &this->show_unclassified);
		ImGui::SameLine();
		ImGui::Checkbox("Scenery", &this->show_scenery);
		ImGui::SameLine();
		ImGui::Checkbox("Props", &this->show_props);
		ImGui::SameLine();
		ImGui::Checkbox("Actors", &this->show_actors);
	}
	ImGui::EndGroup();

	ImGui::Text("Grouping By");
	ImGui::BeginGroup();
	{
		ImGui::SameLine();
		ImGui::RadioButton("Call", &this->grouping, group_by_call);

		ImGui::SameLine();
		ImGui::RadioButton("Model", &this->grouping, group_by_model);
	}
	ImGui::EndGroup();

	auto& game = this->application->get_game_manager();
	auto& draw_manager = game.get_draw_manager();
	auto& model_manager = game.get_model_manager();

	std::vector<int> draws;
	for (std::size_t i = 0; i < draw_manager.count(); ++i)
	{
		auto& draw = draw_manager.get_draw_call(i);

		switch (draw.get_draw_type())
		{
			case principle::Draw::draw_type_scenery:
				if (!this->show_scenery)
				{
					continue;
				}
				break;
			case principle::Draw::draw_type_prop:
				if (!this->show_props)
				{
					continue;
				}
				break;
			case principle::Draw::draw_type_actor:
				if (!this->show_actors)
				{
					continue;
				}
				break;
			case principle::Draw::draw_type_unclassified:
				if (!this->show_unclassified)
				{
					continue;
				}
				break;
			default:
				continue;
		}

		draws.push_back(draw_manager.get_draw_call_tag(i));
	}

	auto profile = kvre::Profile::get_default();
	if (this->grouping == group_by_call)
	{
		for (auto tag: draws)
		{
			auto model = model_manager.get(tag);
			auto& draw = draw_manager.get(tag);
			int buffer_name;
			{
				auto fetcher = profile->get_model_vertex_positions_fetcher(
					model->get_vertex_attributes());
				buffer_name = fetcher->get_buffer().get_name();
			}

			ImGui::PushID(tag);
			{
				ImGui::Text(
					"draw %4lu (model buffer: %4d)",
					draw.get_call_num(), buffer_name);
				ImGui::SameLine();
				show_model_draw(model, draw);
			}
			ImGui::PopID();
		}
	}
	else
	{
		std::sort(
			draws.begin(), draws.end(),
			[&profile, &model_manager](int a, int b)
			{
				auto model_a = model_manager.get(a);
				auto model_b = model_manager.get(b);

				auto fetcher_a = profile->get_model_vertex_positions_fetcher(
					model_a->get_vertex_attributes());
				auto fetcher_b = profile->get_model_vertex_bones_fetcher(
					model_b->get_vertex_attributes());

				auto buffer_a_name = fetcher_a->get_buffer().get_name();
				auto buffer_b_name = fetcher_b->get_buffer().get_name();

				if (buffer_a_name == buffer_b_name)
				{
					return a < b;
				}
				else
				{
					return buffer_a_name < buffer_b_name;
				}
			});

		for (std::size_t i = 0; i < draws.size(); /* Nada. */)
		{
			auto model = model_manager.get(draws[i]);
			int buffer_name;
			{
				auto fetcher = profile->get_model_vertex_positions_fetcher(
					model->get_vertex_attributes());
				buffer_name = fetcher->get_buffer().get_name();
			}

			bool open = ImGui::TreeNode(model, "model (buffer: %4d)", buffer_name);
			ImGui::PushID(buffer_name);
			{
				ImGui::SameLine();
				ImGui::Button("Preview");
				if (ImGui::IsItemHovered())
				{
					auto& index_mapper = model_manager.get_tag_vertex_index_mapper(draws[i]);
					this->application->show_model_thumbnail(model, index_mapper);
				}
			}
			ImGui::PopID();

			auto next_model = model;
			do
			{
				model = next_model;
				auto& draw = draw_manager.get(draws[i]);

				if (open)
				{
					ImGui::Text("draw %4d", draws[i]);

					ImGui::PushID(i);
					{
						ImGui::SameLine();
						show_model_draw(model, draw);
					}
					ImGui::PopID();
				}

				++i;
				if (i < draws.size())
				{
					next_model = model_manager.get(draws[i]);
				}
				else
				{
					break;
				}
			} while (next_model == model);

			if (open)
			{
				ImGui::TreePop();
			}
		}
	}
}

void zeitgeist::Application::ModelDrawListWindow::show_model_draw(
	const kvre::Model* model, const principle::Draw& draw)
{
	ImGui::SameLine();
	if (ImGui::Button("Focus"))
	{
		auto bounds = model->get_bounds();
		bounds.transform(draw.get_model_matrix());

		this->application->camera.focus(bounds.compute_center());
	}

	ImGui::SameLine();
	ImGui::Button("Preview");
	if (ImGui::IsItemHovered())
	{
		this->application->show_model_thumbnail(draw.get_tag());
	}

	ImGui::SameLine();
	if (ImGui::Button("Select"))
	{
		this->application->select(draw.get_tag());
	}

	ImGui::SameLine();
	if (ImGui::Button("Select Batched"))
	{
		kvre::BatchedModelDraws batched_model_draws(
			*model,
			draw.get_event().get_vertex_index_mapper());
			//draw.get_event().get_draw_parameters());

		for (auto& i: batched_model_draws)
		{
			auto selection = this->application->selection_manager.select(model, i);
			selection->set_index_offset_start(0);
			selection->set_index_offset_stop(i.get_num_indices() * i.get_index_size());
			selection->set_model_matrix(draw.get_model_matrix());
		}

		this->application->selection_window.open();
	}
}
