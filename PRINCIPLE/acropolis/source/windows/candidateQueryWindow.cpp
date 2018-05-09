// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "acropolis/application.hpp"
#include "principle/deps/imgui/imgui.h"
#include "thoreau/thoreau.hpp"

acropolis::Application::CandidateQueryWindow::CandidateQueryWindow(
	Application* application) :
		principle::Window(application),
		thumbnail(256, 384)
{
	this->application = application;
	open();
}

void acropolis::Application::CandidateQueryWindow::update(float delta)
{
	this->thumbnail_time += delta;
}

std::string acropolis::Application::CandidateQueryWindow::get_title() const
{
	return "Candidate Query";
}

void acropolis::Application::CandidateQueryWindow::update_gui()
{
	auto& candidate_manager = this->application->candidate_manager;

	ImGui::Text("Count: %lu", candidate_manager.count());

	static const char* NORMAL_NAMES[] =
	{
		"X",
		"Y",
		"Z"
	};
	static const char* SIDE_NAMES[] =
	{
		"left",
		"right",
		"top",
		"bottom"
	};

	for (auto& i: candidate_manager)
	{
		if (i.first > 1024)
		{
			break;
		}

		auto tag = i.first;
		auto& candidate = i.second;

		ImGui::Spacing();
		ImGui::Separator();

		ImGui::PushID(tag);
		ImGui::Text("candidate %5d", tag);
		ImGui::SameLine();
		ImGui::SmallButton("Preview");
		if (ImGui::IsItemHovered())
		{
			preview_candidate(tag);
			show_candidate();
		}

		ImGui::BulletText("max triangle surface area: %f", candidate.get_max_triangle_surface_area());
		ImGui::Indent();
		for (int j = 0; j < continent1::Candidate::normal_max; ++j)
		{
			ImGui::BulletText(
				"%s max triangle surface area: %f",
				NORMAL_NAMES[j],
				candidate.get_max_triangle_surface_area_by_normal(j));
		}
		ImGui::Unindent();

		ImGui::BulletText("total surface area: %f", candidate.get_surface_area_sum());
		ImGui::Indent();
		for (int j = 0; j < continent1::Candidate::normal_max; ++j)
		{
			ImGui::BulletText(
				"%s surface area sum: %f",
				NORMAL_NAMES[j],
				candidate.get_surface_area_sum_by_normal(j));
		}
		ImGui::Unindent();

		auto bounds = candidate.get_bounds().compute_size();
		ImGui::BulletText(
			"bounds: (%f, %f, %f)",
			bounds.x,
			bounds.y,
			bounds.z);
		ImGui::BulletText(
			"tile: min (%d, %d; %d), max (%d, %d; %d)",
			candidate.get_min_tile().x, candidate.get_min_tile().y, candidate.get_min_tile().layer,
			candidate.get_max_tile().x, candidate.get_max_tile().y, candidate.get_max_tile().layer);

		ImGui::BulletText(
			"vertices: %lu (%lu unique)",
			candidate.get_num_vertices(),
			candidate.get_num_unique_vertices());
		ImGui::BulletText("triangles: %lu", candidate.get_num_triangles());

		if (ImGui::CollapsingHeader("Tile Properties"))
		{
			auto min = candidate.get_min_tile();
			auto max = candidate.get_max_tile();
			for (int layer = min.layer; layer <= max.layer; ++layer)
			{
				for (int x = min.x; x <= max.x; ++x)
				{
					for (int y = min.y; y <= max.y; ++y)
					{
						auto tile_coordinate = thoreau::TileCoordinate(x, y, layer);

						ImGui::PushID(tile_coordinate.to_packed());
						ImGui::BulletText("tile (%d, %d; %d)", x, y, layer);
						ImGui::SameLine();
						ImGui::SmallButton("Preview");
						if (ImGui::IsItemHovered())
						{
							preview_candidate(tag, tile_coordinate);
							show_candidate();
						}

						ImGui::Indent();
						ImGui::BulletText(
							"vertices: %lu (unique: %lu)",
							candidate.get_num_vertices(tile_coordinate),
							candidate.get_num_unique_vertices(tile_coordinate));

						ImGui::BulletText(
							"triangles: %lu",
							candidate.get_num_triangles(tile_coordinate));
						ImGui::Indent();
						for (int j = 0; j < continent1::Candidate::side_max; ++j)
						{
							ImGui::BulletText(
								"%s triangles: %lu (%.0f%%)",
								SIDE_NAMES[j],
								candidate.get_num_triangles_on_side(tile_coordinate, j),
								(float)candidate.get_num_triangles_on_side(tile_coordinate, j) / candidate.get_num_triangles(tile_coordinate) * 100.0f);
						}
						ImGui::Unindent();

						ImGui::BulletText(
							"token intersections: %lu",
							candidate.get_num_token_intersections(tile_coordinate));
						ImGui::Unindent();

						ImGui::PopID();
					}
				}
			}
		}

		ImGui::PopID();
	}
}

void acropolis::Application::CandidateQueryWindow::preview_candidate(
	int tag)
{
	if (tag != this->thumbnail_candidate ||
		this->thumbnail_tile_coordinate.x != 0 ||
		this->thumbnail_tile_coordinate.y != 0 ||
		this->thumbnail_tile_coordinate.layer != 0)
	{
		this->thumbnail_tile_coordinate = thoreau::TileCoordinate(0, 0);
		this->thumbnail_candidate = tag;

		auto& candidate_manager = this->application->candidate_manager;
		auto candidate = candidate_manager.get(tag);

		assert(candidate != nullptr);

		this->thumbnail.clear();
		this->thumbnail.unset_bounds();

		auto draw_parameters = kompvter::DrawParameters(
			kompvter::DrawParameters::primitive_triangles,
			0, candidate->get_vertex_index_mapper().get_buffer().get_data_size());
		this->thumbnail.add_model(
			candidate->get_model(),
			candidate->get_model_matrix(),
			candidate->get_vertex_index_mapper(),
			draw_parameters);
	}
}

void acropolis::Application::CandidateQueryWindow::preview_candidate(
	int tag, const thoreau::TileCoordinate& tile_coordinate)
{
	if (this->thumbnail_candidate != tag ||
		tile_coordinate != this->thumbnail_tile_coordinate)
	{
		this->thumbnail_candidate = 0;
		preview_candidate(tag);

		this->thumbnail_tile_coordinate = tile_coordinate;

		auto& map = this->application->get_game_manager().get_runtime_map();
		float max_y = this->thumbnail.get_bounds().max.y;
		float min_y = map.get_tile(tile_coordinate).get_elevation();
		if (map.has_tile(tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer + 1))
		{
			max_y = map.get_tile(tile_coordinate.x, tile_coordinate.y, tile_coordinate.layer + 1).get_elevation();
		}

		kvlc::Bounds bounds;
		bounds.min.x = tile_coordinate.x * thoreau::TILE_SIZE;
		bounds.min.y = min_y;
		bounds.min.z = tile_coordinate.y * thoreau::TILE_SIZE;
		bounds.max.x = (tile_coordinate.x + 1.0f) * thoreau::TILE_SIZE;
		bounds.max.y = max_y;
		bounds.max.z = (tile_coordinate.y + 1.0f) * thoreau::TILE_SIZE;
		this->thumbnail.set_bounds(bounds);
	}
}

void acropolis::Application::CandidateQueryWindow::show_candidate()
{
	auto model_matrix = glm::rotate(
		glm::mat4(1.0f),
		(float)(this->thumbnail_time * M_PI / 2.0),
		glm::vec3(0.0f, 1.0f, 0.0f));

	kvncer::Texture* texture;
	glDisable(GL_SCISSOR_TEST);
	if (this->thumbnail_tile_coordinate != thoreau::TileCoordinate(0, 0))
	{
		this->thumbnail.start_draw();
		this->thumbnail.draw_clipped_to_tile(
			this->thumbnail_tile_coordinate.x,
			this->thumbnail_tile_coordinate.y,
			model_matrix);
		texture = this->thumbnail.stop_draw();
	}
	else
	{
		texture = this->thumbnail.draw(model_matrix);
	}
	glDisable(GL_SCISSOR_TEST);
	ImGui::BeginTooltip();
	ImGui::Image(texture, ImVec2(texture->get_width(), texture->get_height()));
	ImGui::EndTooltip();
}