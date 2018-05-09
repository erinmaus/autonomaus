// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "acropolis/application.hpp"
#include "thoreau/thoreau.hpp"

acropolis::Application::TileView::TileView(Application* application)
{
	this->application = application;
}

void acropolis::Application::TileView::update(float delta)
{
	time += delta;
}

void acropolis::Application::TileView::draw()
{
	if (get_size() != this->previous_size || this->is_dirty)
	{
		this->is_dirty = false;
		this->previous_size = get_size();
		this->thumbnail = std::make_unique<principle::Thumbnail>(
			this->previous_size.x, this->previous_size.y);

		auto& candidate_manager = this->application->candidate_manager;
		auto& draw_manager = this->application->get_game_manager().get_draw_manager();
		auto& terrain_manager = this->application->get_game_manager().get_terrain_manager();
		for (auto& i: draw_manager)
		{
			auto& draw = i.second;
			if (draw.get_draw_type() != principle::Draw::draw_type_terrain)
			{
				continue;
			}

			this->thumbnail->add_terrain(
				terrain_manager.get(draw.get_tag()),
				draw.get_model_matrix(),
				draw.get_draw_parameters());
		}

		for (int x = -1; x <= 1; ++x)
		{
			for (int y = -1; y <= 1; ++y)
			{
				auto current_coordinate = thoreau::TileCoordinate(
					this->tile_coordinate.x + x,
					this->tile_coordinate.y + y,
					this->tile_coordinate.layer);

				auto begin = candidate_manager.space_begin(current_coordinate);
				auto end = candidate_manager.space_end(current_coordinate);
				for (auto i = begin; i != end; ++i)
				{
					auto candidate = candidate_manager.get(i->second);
					if (candidate == nullptr)
					{
						continue;
					}

					auto draw_parameters = kompvter::DrawParameters(
						kompvter::DrawParameters::primitive_triangles,
						0, candidate->get_vertex_index_mapper().get_buffer().get_data_size());
					this->thumbnail->add_model(
						candidate->get_model(),
						candidate->get_model_matrix(),
						candidate->get_vertex_index_mapper(),
						draw_parameters);
				}
			}
		}

		auto default_bounds = this->thumbnail->get_bounds();
		if (std::fabs(default_bounds.min.y) == HUGE_VALF)
		{
			auto& map = this->application->get_game_manager().get_runtime_map();
			if (map.has_tile(this->tile_coordinate))
			{
				auto& tile = map.get_tile(this->tile_coordinate);
				if (tile.has_elevation())
				{
					default_bounds.min.y = tile.get_elevation() - 128.0f;
					default_bounds.max.y = tile.get_elevation() + 512.0f;
				}
			}
		}

		kvlc::Bounds bounds;
		bounds.min.x = (this->tile_coordinate.x - 1.0f) * thoreau::TILE_SIZE;
		bounds.min.y = default_bounds.min.y;
		bounds.min.z = (this->tile_coordinate.y - 1.0f) * thoreau::TILE_SIZE;
		bounds.max.x = (this->tile_coordinate.x + 1.0f) * thoreau::TILE_SIZE;
		bounds.max.y = default_bounds.max.y;
		bounds.max.z = (this->tile_coordinate.y + 1.0f) * thoreau::TILE_SIZE;

		this->thumbnail->set_bounds(bounds);
		this->thumbnail->set_background_color(glm::vec4(0.2f, 0.3f, 0.6f, 1.0f));
	}

	apply_viewport();
	glDisable(GL_CULL_FACE);
	this->thumbnail->start_draw(false);

	auto rotation_y = glm::rotate(
		glm::mat4(1.0f),
		(float)(this->time * M_PI / 2.0),
		glm::vec3(0.0f, 1.0f, 0.0f));
	auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, -1.0f, 1.0f));
	auto model_matrix = rotation_y * scale;
	for (int x = -1; x <= 1; ++x)
	{
		for (int y = -1; y <= 1; ++y)
		{
			this->thumbnail->draw_clipped_to_tile(
				x + this->tile_coordinate.x,
				y + this->tile_coordinate.y,
				model_matrix);
		}
	}

	this->thumbnail->stop_draw();
}

void acropolis::Application::TileView::show(
	const thoreau::TileCoordinate& tile_coordinate)
{
	this->tile_coordinate = tile_coordinate;
	this->is_dirty = true;
}
