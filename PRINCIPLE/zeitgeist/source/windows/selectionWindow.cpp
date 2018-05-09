// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "chomsky/world/actorPointCloudWorldUpdatePass.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "principle/deps/imgui/imgui.h"
#include "principle/resources/modelSelection.hpp"
#include "thoreau/thoreau.hpp"
#include "zeitgeist/application.hpp"

zeitgeist::Application::SelectionWindow::SelectionWindow(
	Application* application) :
		principle::Window(application)
{
	this->application = application;
}

std::string zeitgeist::Application::SelectionWindow::get_title() const
{
	return "Selection";
}

void zeitgeist::Application::SelectionWindow::update_gui()
{
	show_thumbnail();
	ImGui::Spacing();

	if (ImGui::Button("Clear"))
	{
		ImGui::OpenPopup(get_id("confirm_clear").c_str());
	}

	ImGui::SameLine();
	if (ImGui::Button("Export Geometry"))
	{
		auto filename = this->application->get_output_filename();
		if (!filename.empty())
		{
			// TODO
		}
	}

	ImGui::SameLine();
	if (ImGui::Button("New Point Cloud Query"))
	{
		new_point_cloud_query();
	}

	if (ImGui::CollapsingHeader("Models"))
	{
		show_selection_list();
	}

	if (ImGui::BeginPopup(get_id("confirm_clear").c_str()))
	{
		ImGui::Text("Are you sure you wish to clear the current selection?");
		if (ImGui::Button("Yes"))
		{
			this->application->selection_manager.unselect_all();
			ImGui::CloseCurrentPopup();
		}

		ImGui::SameLine();
		if (ImGui::Button("No"))
		{
			ImGui::CloseCurrentPopup();
		}
		ImGui::EndPopup();
	}
}

void zeitgeist::Application::SelectionWindow::show_selection_list()
{
	std::unordered_set<principle::ModelSelection*> pending_removals;
	auto& draw_manager = this->application->get_game_manager().get_draw_manager();
	auto profile = kvre::Profile::get_default();
	for (auto& selection: this->application->selection_manager)
	{
		auto model = selection.get_model();
		int buffer_name;
		{
			auto fetcher = profile->get_model_vertex_positions_fetcher(
				model->get_vertex_attributes());
			buffer_name = fetcher->get_buffer().get_name();
		}

		ImGui::PushID(&selection);
		{
			kompvter::DrawParameters draw_parameters(
				kompvter::DrawParameters::primitive_triangles,
				selection.get_index_offset_start(),
				selection.get_index_offset_stop());
			auto begin = kvre::IndexedVertexIterator::begin(
				selection.get_index_mapper(), draw_parameters);
			auto end = kvre::IndexedVertexIterator::end(
				selection.get_index_mapper(), draw_parameters);
			ImGui::Text(
				"model buffer %4d (%5lu vertices)",
				buffer_name, end - begin);
		}

		ImGui::SameLine();
		if (ImGui::Button("Focus"))
		{
			if (draw_manager.has(selection.get_tag()))
			{
				auto& draw = draw_manager.get(selection.get_tag());
				auto bounds = model->get_bounds();
				bounds.transform(draw.get_model_matrix());

				this->application->camera.focus(bounds.compute_center());
			}
		}

		ImGui::SameLine();
		ImGui::Button("Preview");
		if (ImGui::IsItemHovered())
		{
			if (draw_manager.has(selection.get_tag()))
			{
				this->application->show_model_thumbnail(selection.get_tag());
			}
			else
			{
				kompvter::DrawParameters draw_parameters(
					kompvter::DrawParameters::primitive_triangles,
					selection.get_index_offset_start(),
					selection.get_index_offset_stop());
				this->application->show_model_thumbnail(
					selection.get_model(),
					selection.get_index_mapper(),
					draw_parameters);
			}
		}

		ImGui::SameLine();
		if (ImGui::Button("Unselect"))
		{
			pending_removals.insert(&selection);
		}

		ImGui::PopID();
	}

	for (auto selection: pending_removals)
	{
		this->application->selection_manager.unselect(selection);
	}
}

kvncer::Texture* zeitgeist::Application::SelectionWindow::update_thumbnail()
{
	if (!this->thumbnail || this->thumbnail->get_width() != ImGui::GetWindowSize().x)
	{
		int width = ImGui::GetWindowSize().x;
		int height = width * (9.0f / 16.0f);
		this->thumbnail = std::make_unique<principle::Thumbnail>(width, height);
		this->selections.clear();
	}

	std::unordered_set<principle::ModelSelection*> current_selection;
	for (auto& i: this->application->selection_manager)
	{
		current_selection.insert(&i);
	}

	if (current_selection != this->selections)
	{
		this->thumbnail->clear();

		for (auto selection: current_selection)
		{
			kompvter::DrawParameters draw_parameters(
				kompvter::DrawParameters::primitive_triangles,
				selection->get_index_offset_start(),
				selection->get_index_offset_stop());
			this->thumbnail->add_model(
				selection->get_model(),
				glm::mat4(1.0f),
				selection->get_index_mapper(),
				draw_parameters);
		}

		this->selections = std::move(current_selection);
	}

	glm::mat4 x = glm::rotate(glm::mat4(1.0f), this->x_rotation, glm::vec3(-1.0f, 0.0f, 0.0f));
	glm::mat4 y = glm::rotate(glm::mat4(1.0f), this->y_rotation, glm::vec3(0.0f, 1.0f, 0.0f));
	glm::mat4 s = glm::scale(glm::mat4(1.0f), glm::vec3(this->scale));

	return this->thumbnail->draw(s * x * y);
}

void zeitgeist::Application::SelectionWindow::show_thumbnail()
{
	auto thumbnail_image = update_thumbnail();

	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(0, 0));
	ImGui::BeginChildFrame(0, ImVec2(thumbnail_image->get_width(), thumbnail_image->get_height()));
	ImGui::Image(thumbnail_image, ImVec2(thumbnail_image->get_width(), thumbnail_image->get_height()));
	if (ImGui::IsItemHovered())
	{
		if (ImGui::IsMouseDragging(0))
		{
			this->x_rotation += ImGui::GetMouseDragDelta(0).y / 64.0f;
			this->y_rotation += ImGui::GetMouseDragDelta(0).x / 64.0f;
			ImGui::ResetMouseDragDelta(0);
		}

		if (ImGui::IsMouseDragging(1))
		{
			this->scale += ImGui::GetMouseDragDelta(1).y / 64.0f;
			ImGui::ResetMouseDragDelta(1);
		}
	}
	ImGui::EndChildFrame();
	ImGui::PopStyleVar();
	ImGui::PopStyleVar();
}

void zeitgeist::Application::SelectionWindow::new_point_cloud_query()
{
	if (this->application->selection_manager.empty())
	{
		return;
	}

	typedef chomsky::WorldState<
		thoreau::TileCoordinate,
		chomsky::TileSpacePointCloud>
	WorkingPointCloud;

	WorkingPointCloud point_cloud;
	for (auto& selection: this->application->selection_manager)
	{
		auto model = selection.get_model();
		for (std::size_t i = 0; i < model->get_num_vertices(); ++i)
		{
			auto position = model->get_position(i);
			auto color = glm::vec3(model->get_color(i));

			auto tile_coordinate = thoreau::tile_from_world(position);
			auto tile_relative_position = position;
			tile_relative_position.x -= tile_coordinate.x * thoreau::TILE_SIZE;
			tile_relative_position.z -= tile_coordinate.y * thoreau::TILE_SIZE;

			if (tile_relative_position.x < 0.0f)
			{
				tile_relative_position.x = thoreau::TILE_SIZE + tile_relative_position.x;
			}

			if (tile_relative_position.z < 0.0f)
			{
				tile_relative_position.z = thoreau::TILE_SIZE + tile_relative_position.z;
			}

			auto& tile_point_cloud = point_cloud.add(tile_coordinate);
			tile_point_cloud.add(tile_relative_position, color);
		}
	}

	chomsky::TileSpacePointCloudMatch result;
	for (auto& i: point_cloud)
	{
		result.set(i.first, i.second);
	}

	this->application->point_cloud_match_builder_window.add(result);
	this->application->point_cloud_match_builder_window.open();
}
