// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <boost/container/flat_map.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include "chomsky/world/detail/scenery.hpp"
#include "chomsky/world/detail/prop.hpp"
#include "continent1/heuristics.hpp"
#include "continent7/application.hpp"
#include "kommon/graphics/image.hpp"
#include "kvlc/geometry/mesh.hpp"
#include "kvlc/geometry/ray.hpp"
#include "kvlc/collision/query.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "kvre/model/batchedModelDraws.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/deps/imgui/imgui.h"
#include "principle/graphics/utility.hpp"
#include "principle/scene/maxTileElevationsNode.hpp"
#include "thoreau/thoreau.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"

static void make_box(
	kvncer::Mesh& mesh,
	kvncer::MeshBuffer& vertex_buffer,
	kvncer::MeshBuffer& index_buffer);

continent7::Application::Application() :
	principle::Application("continent7"),
	proxy_pass(this),
	main_window(this),
	export_window(this),
	edit_tile_window(this),
	candidate_query_window(this),
	toolbox(this),
	visible_tile_pass(&get_game_manager().get_transform_state(), &get_game_manager().get_terrain_elevation_state(), &this->visible_tile_state),
	scenery_elevation_pass(&get_game_manager().get_transform_state(), &get_game_manager().get_terrain_elevation_state(), &this->scenery_elevation_state, &this->relative_scenery_elevation_state),
	camera_controller(&camera),
	candidate_manager(&batched_model_manager, &get_game_manager().get_runtime_map())
{
	this->scene_shader = principle::Material::load_shader_from_file(
		"model.vert.glsl", "untextured_clipped_model.frag.glsl");
	this->scene_material.set_shader(this->scene_shader);

	auto& world_update_manager = get_game_manager().get_world_update_manager();
	world_update_manager.add_pass(&this->visible_tile_pass);
	world_update_manager.add_pass(&this->batched_model_manager);
	world_update_manager.add_pass(&this->candidate_manager);
	world_update_manager.add_pass(&this->scenery_elevation_pass);
	world_update_manager.add_pass(&this->proxy_pass);

	make_box(this->box_mesh, this->box_vertex_buffer, this->box_index_buffer);
}

continent7::Application::~Application()
{
	delete this->scene_shader;

	clear_meshes();
}

void continent7::Application::update(float delta)
{
	this->camera_controller.update(delta);
	this->camera.update(delta);
}

void continent7::Application::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	principle::Application::draw();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	this->camera.set_viewport_width(get_display()->get_width());
	this->camera.set_viewport_height(get_display()->get_height());
	if (this->draw_scenes)
	{
		for (auto& scene: this->scenes)
		{
			scene->draw();
		}

		update_current_tile();
		{
			glm::vec3 world_position;
			world_position.x = (this->current_tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
			world_position.y = this->current_tile_elevation;
			world_position.z = (this->current_tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

			glLineWidth(4.0f);
			draw_box(world_position);
			glLineWidth(1.0f);
		}
	}

	auto& map = get_game_manager().get_runtime_map();
	for (auto i: this->visible_tile_state)
	{
		auto tile_coordinate = i.first;

		bool is_visible = false;
		float elevation;
		if (map.has_tile(tile_coordinate))
		{
			auto& tile = map.get_tile(tile_coordinate);
			if (tile.has_elevation() && tile.is_impassable())
			{
				elevation = tile.get_elevation();
				is_visible = true;
			}
		}

		if (is_visible)
		{
			glm::vec3 world_position;
			world_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
			world_position.y = elevation;
			world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;
			draw_box(world_position);
		}
	}
}

void continent7::Application::update_gui()
{
	auto& imgui = ImGui::GetIO();
	principle::Application::update_gui();

	if (this->show_current_tile_info && !imgui.WantCaptureMouse)
	{
		auto& map = get_game_manager().get_runtime_map();
		auto& minimap = get_game_manager().get_minimap();

		auto tile_coordinate = get_current_tile_coordinate(true);
		ImGui::BeginTooltip();
		ImGui::Text(
			"Tile (%d, %d; %d)",
			tile_coordinate.x,
			tile_coordinate.y,
			tile_coordinate.layer);
		if (this->visible_tile_state.has(this->current_tile_coordinate))
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0, 1, 0, 1));
			ImGui::BulletText("visible");
			ImGui::PopStyleColor();
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1, 1, 0, 1));
			ImGui::BulletText("not visible");
			ImGui::PopStyleColor();
		}

		if (map.has_tile(tile_coordinate))
		{
			auto& tile = map.get_tile(tile_coordinate);

			if (tile.has_elevation())
			{
				ImGui::BulletText("elevation: %.0f", tile.get_elevation());
			}
			else
			{
				ImGui::BulletText("elevation: (none)");
			}
			ImGui::BulletText("height: %.0f", tile.get_height());

			ImGui::BulletText("flags: 0x%08x", tile.get_flags());

			if (tile.is_wall() || tile.get_flags())
			{
				ImGui::BulletText("is wall");

				if (tile.get_flags() & thoreau::Tile::flag_wall_diagonal)
				{
					if (tile.get_flags() & thoreau::Tile::flag_wall_diagonal_left)
					{
						ImGui::BulletText("is left (/) diagonal");
					}
					else if (tile.get_flags() & thoreau::Tile::flag_wall_diagonal_right)
					{
						ImGui::BulletText("is right (\\) diagonal");
					}
				}
			}

			if (tile.is_door())
			{
				ImGui::BulletText("is door");
			}

			if (tile.is_portal())
			{
				ImGui::BulletText("is portal");
			}

			if (tile.is_impassable())
			{
				ImGui::BulletText("is impassable");
			}
			else
			{
				ImGui::BulletText("is passable");
			}

			if (this->player_sectors.count(thoreau::sector_from_tile(tile_coordinate)) != 0)
			{
				ImGui::BulletText("player in (or adjacent) to sector");
			}

			if (minimap.get_is_visible())
			{
				std::set<std::string> features;
				for (auto i = minimap.features_begin(); i != minimap.features_end(); ++i)
				{
					auto feature_coordinate = minimap.minimap_to_tile(i->second);
					if (feature_coordinate == tile_coordinate)
					{
						features.insert(i->first);
					}
				}

				if (!features.empty())
				{
					ImGui::Text("features:");

					for (auto& i: features)
					{
						ImGui::BulletText("%s", i.c_str());
					}
				}
			}
		}

		auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);
		auto elevation = this->sector_elevation_textures.find(sector_coordinate);
		if (elevation != this->sector_elevation_textures.end())
		{
			ImGui::Image(
				elevation->second.get(),
				ImVec2(elevation->second->get_width(), elevation->second->get_height()));

			auto walls = this->sector_wall_textures.find(sector_coordinate);
			if (walls != this->sector_wall_textures.end())
			{
				ImGui::SameLine();
				ImGui::Image(
					walls->second.get(),
					ImVec2(walls->second->get_width(), walls->second->get_height()));
			}
		}

		ImGui::EndTooltip();
	}
}

void continent7::Application::on_mouse_button_down(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_button_down(event);
}

void continent7::Application::on_mouse_button_up(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_button_up(event);
}

void continent7::Application::on_mouse_move(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_move(event);

	auto display = get_display();
	this->mouse_position = event.get_position();
	this->mouse_position.y = display->get_height() - mouse_position.y;
	if (event.get_difference().x != 0 || event.get_difference().y != 0)
	{
		this->current_tile_dirty = true;
	}
}

void continent7::Application::on_mouse_scroll(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_scroll(event);
}

void continent7::Application::on_key_down(const principle::KeyboardEvent& event)
{
	this->camera_controller.on_key_down(event);

	if (event.get_key_code() == ALLEGRO_KEY_LCTRL ||
		event.get_key_code() == ALLEGRO_KEY_RCTRL)
	{
		this->show_current_tile_info = !this->show_current_tile_info;
	}

	if (event.get_key_code() == ALLEGRO_KEY_TAB)
	{
		this->draw_scenes = false;
	}

	if (event.get_key_code() == ALLEGRO_KEY_LSHIFT ||
		event.get_key_code() == ALLEGRO_KEY_RSHIFT)
	{
		++this->is_multiselection;
	}

	if (event.get_key_code() == ALLEGRO_KEY_P)
	{
		this->toolbox.push_undo_stack();
		this->toolbox.mark_passable(get_current_tile_coordinate(true));
		notify_map_update();
	}
	else if (event.get_key_code() == ALLEGRO_KEY_I)
	{
		this->toolbox.push_undo_stack();
		this->toolbox.mark_impassable(get_current_tile_coordinate(true));
		notify_map_update();
	}
	else if (event.get_key_code() == ALLEGRO_KEY_M)
	{
		this->toolbox.push_undo_stack();
		this->toolbox.elevation_from_ray(
			get_current_tile_coordinate(true),
			get_current_tile_elevation() + this->max_elevation_deviation,
			this->make_passable);
		notify_map_update();
	}
	else if (event.get_key_code() == ALLEGRO_KEY_N)
	{
		this->toolbox.push_undo_stack();
		this->toolbox.neighbor_elevations_from_ray(
			get_current_tile_coordinate(true),
			get_current_tile_elevation() + this->max_elevation_deviation,
			this->max_elevation_deviation,
			this->make_passable);
		notify_map_update();
	}
	else if (event.get_key_code() == ALLEGRO_KEY_F)
	{
		if (this->is_multiselection)
		{
			this->edit_tile_window.add_tile(get_current_tile_coordinate(true));
		}
		else
		{
			this->edit_tile_window.set_tile(get_current_tile_coordinate(true));
		}
	}
	else if (event.get_key_code() == ALLEGRO_KEY_C)
	{
		this->candidate_query_window.open(get_current_tile_coordinate(true));
	}
}

void continent7::Application::on_key_up(const principle::KeyboardEvent& event)
{
	this->camera_controller.on_key_up(event);

	if (event.get_key_code() == ALLEGRO_KEY_TAB)
	{
		this->draw_scenes = true;
	}

	if (event.get_key_code() == ALLEGRO_KEY_LSHIFT ||
		event.get_key_code() == ALLEGRO_KEY_RSHIFT)
	{
		--this->is_multiselection;
	}
}

void continent7::Application::on_start_trace()
{
	this->scenes.clear();

	this->sector_elevation_textures.clear();
	this->sector_wall_textures.clear();
	this->dirty_sectors.clear();
	this->visible_sectors.clear();
	this->visited_tiles.clear();

	this->toolbox.clear_undo_stack();

	this->bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));

	this->current_frame = 0;
	this->map_collision_scene.clear();
	this->map_collision_scene_types.clear();
}

void continent7::Application::on_finish_trace()
{
	auto& minimap = get_game_manager().get_minimap();
	auto center = this->bounds.compute_center();
	auto size = this->bounds.compute_size();

	glm::vec3 world_position;
	if (minimap.get_is_visible())
	{
		world_position.x = minimap.get_center().x * thoreau::TILE_SIZE;
		world_position.y = center.y + size.y / 2.0f;
		world_position.z = minimap.get_center().y * thoreau::TILE_SIZE;
	}
	else
	{
		world_position = center + size.y / 2.0f;
	}

	this->camera.set_target(world_position);
	this->camera.set_vertical_rotation(-M_PI / 4.0f);

	notify_map_update();
}

void continent7::Application::notify_map_update()
{
	for (auto sector: this->dirty_sectors)
	{
		update_sector_elevations_texture(sector);
		update_sector_walls_texture(sector);

		if (this->visible_sectors.count(sector) == 0)
		{
			if (!this->player_sector_only ||
				this->player_sectors.count(sector) != 0)
			{
				this->visible_sectors[sector] = true;
			}
		}
	}

	this->dirty_sectors.clear();
}

void continent7::Application::populate_scene()
{
	this->scenes.emplace_back(new principle::Scene());
	this->scenes.back()->set_camera(&this->camera);

	auto& game = get_game_manager();
	auto& draw_manager = game.get_draw_manager();
	auto& model_manager = game.get_model_manager();
	auto& terrain_manager = game.get_terrain_manager();
	for (auto& draw: draw_manager)
	{
		if (model_manager.has(draw.first) &&
			draw.second.get_draw_type() != principle::Draw::draw_type_actor &&
			draw.second.get_draw_type() != principle::Draw::draw_type_unclassified)
		{
			if (model_manager.tag_has_vertex_index_mapper(draw.first))
			{
				add_model(
					draw.second,
					model_manager.get(draw.first),
					model_manager.get_tag_vertex_index_mapper(draw.first));
			}
			else
			{
				add_model(draw.second, model_manager.get(draw.first));
			}
		}
		else if (terrain_manager.has(draw.first))
		{
			add_terrain(draw.second, terrain_manager.get(draw.first));
		}
	}
}

void continent7::Application::add_model(
	const principle::Draw& draw,
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper)
{
	/// XXX: Hack to prevent the skybox from rendering.
	if (glm::ivec4(model->get_color(0) * 255.0f) == glm::ivec4(90, 236, 47, 255))
	{
		return;
	}

	auto mesh = create_mesh_for_model(model, index_mapper);
	auto model_node = this->scenes.back()->add_node<principle::ModelNode>(
		mesh,
		kvncer::gl::marshal_index_mapper_format_from_kompvter(index_mapper.get_format()),
		draw.get_draw_parameters().get_offset_start(),
		draw.get_draw_parameters().get_offset_stop());
	{
		model_node->set_model_matrix(draw.get_model_matrix());
		model_node->set_material(&this->scene_material);
	}

	auto begin = kvre::IndexedVertexIterator::begin(
		index_mapper, draw.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		index_mapper, draw.get_draw_parameters());

	kvlc::Bounds bounds(model->get_position(*begin), model->get_position(*begin));
	model->for_each_position(
		begin, end,
		[&bounds](const glm::vec3& position, std::size_t)
		{
			bounds.expand(position);
		});
	bounds.transform(draw.get_model_matrix());

	this->bounds.expand(bounds);
}

void continent7::Application::add_terrain(
	const principle::Draw& draw,
	const kvre::Terrain* terrain)
{
	auto mesh = create_mesh_for_terrain(terrain);
	auto terrain_node = this->scenes.back()->add_node<principle::ModelNode>(
		mesh,
		GL_NONE,
		draw.get_draw_parameters().get_offset_start(),
		draw.get_draw_parameters().get_offset_stop());
	{
		terrain_node->set_model_matrix(draw.get_model_matrix());
		terrain_node->set_material(&this->scene_material);
	}

	auto begin = kvre::RangeVertexIterator(draw.get_draw_parameters().get_offset_start());
	auto end = kvre::RangeVertexIterator(draw.get_draw_parameters().get_offset_stop());

	kvlc::Bounds bounds(terrain->get_position(*begin), terrain->get_position(*begin));
	terrain->for_each_position(
		begin, end,
		[&bounds](const glm::vec3& position, std::size_t)
		{
			bounds.expand(position);
		});
	bounds.transform(draw.get_model_matrix());

	this->bounds.expand(bounds);
}

kvncer::Mesh* continent7::Application::create_mesh_for_model(
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper)
{
	auto mesh = new kvncer::Mesh();
	auto vertex_buffer = new kvncer::MeshBuffer();
	this->meshes.push_back(mesh);
	this->mesh_buffers.push_back(vertex_buffer);

	principle::generate_untextured_model_vertex_data(
		model, *mesh, *vertex_buffer);

	if (index_mapper)
	{
		auto index_buffer = new kvncer::MeshBuffer();
		this->mesh_buffers.push_back(index_buffer);

		index_buffer->set_buffer(index_mapper.get_buffer());
		mesh->set_indices(index_buffer);
	}

	return mesh;
}

kvncer::Mesh* continent7::Application::create_mesh_for_terrain(
	const kvre::Terrain* terrain)
{
	auto mesh = new kvncer::Mesh();
	auto vertex_buffer = new kvncer::MeshBuffer();
	this->meshes.push_back(mesh);
	this->mesh_buffers.push_back(vertex_buffer);

	principle::generate_untextured_terrain_vertex_data(
		terrain, *mesh, *vertex_buffer);

	return mesh;
}

void continent7::Application::clear_meshes()
{
	for (auto mesh: this->meshes)
	{
		delete mesh;
	}
	this->meshes.clear();

	for (auto buffer: this->mesh_buffers)
	{
		delete buffer;
	}
	this->mesh_buffers.clear();
}

void continent7::Application::update_sector_elevations_texture(
	const thoreau::SectorCoordinate& sector_coordinate)
{
	auto& map = get_game_manager().get_runtime_map();

	auto begin = map.sector_begin(sector_coordinate);
	auto end = map.sector_end(sector_coordinate);

	std::map<thoreau::TileCoordinate, thoreau::Tile> tiles;
	float min_elevation = HUGE_VALF, max_elevation = -HUGE_VALF;
	for (auto i = begin; i != end; ++i)
	{
		auto tile_coordinate = *i;
		if (tile_coordinate.layer != sector_coordinate.layer)
		{
			continue;
		}

		auto& tile = map.get_tile(tile_coordinate);
		if (!tile.has_elevation())
		{
			continue;
		}

		min_elevation = glm::min(min_elevation, tile.get_elevation());
		max_elevation = glm::max(max_elevation, tile.get_elevation());

		tiles[tile_coordinate] = tile;
	}

	const int TILE_SIZE = 4;
	int width = TILE_SIZE * thoreau::SECTOR_SIZE;
	int height = TILE_SIZE * thoreau::SECTOR_SIZE;
	kommon::Image image(width, height);

	int min_tile_x = sector_coordinate.x * thoreau::SECTOR_SIZE;
	int min_tile_y = sector_coordinate.y * thoreau::SECTOR_SIZE;
	for (auto& i: tiles)
	{
		auto elevation = i.second.get_elevation();
		float relative_elevation = (elevation - min_elevation) / (max_elevation - min_elevation);
		float hue = relative_elevation * 360.0f;
		auto color = glm::rgbColor(glm::vec3(hue, 0.5f, 0.75f));

		int left = (i.first.x - min_tile_x) * TILE_SIZE;
		int bottom = (i.first.y - min_tile_y) * TILE_SIZE;

		for (int y = 0; y < TILE_SIZE; ++y)
		{
			image.draw_line(
				left, bottom + y,
				left + TILE_SIZE, bottom + y,
				glm::vec4(color, 1.0f));
		}
	}

	auto texture = std::make_unique<kvncer::Texture>(
		kvncer::Texture::component_format_rgba, false,
		width, height);
	texture->update(
		0, 0, width, height,
		kvncer::Texture::component_format_rgba,
		kvncer::Texture::component_type_unsigned_byte,
		image.get_pixels());

	this->sector_elevation_textures[sector_coordinate] = std::move(texture);
}

void continent7::Application::update_sector_walls_texture(
	const thoreau::SectorCoordinate& sector_coordinate)
{
	auto& map = get_game_manager().get_runtime_map();

	auto begin = map.sector_begin(sector_coordinate);
	auto end = map.sector_end(sector_coordinate);

	std::map<thoreau::TileCoordinate, thoreau::Tile> tiles;
	for (auto i = begin; i != end; ++i)
	{
		auto tile_coordinate = *i;
		if (tile_coordinate.layer != sector_coordinate.layer)
		{
			continue;
		}

		auto& tile = map.get_tile(tile_coordinate);
		tiles[tile_coordinate] = tile;
	}

	const int TILE_SIZE = 4;
	int width = TILE_SIZE * thoreau::SECTOR_SIZE;
	int height = TILE_SIZE * thoreau::SECTOR_SIZE;
	kommon::Image image(width, height);

	int min_tile_x = sector_coordinate.x * thoreau::SECTOR_SIZE;
	int min_tile_y = sector_coordinate.y * thoreau::SECTOR_SIZE;
	for (auto& i: tiles)
	{
		auto& tile = i.second;

		int left = (i.first.x - min_tile_x) * TILE_SIZE;
		int right = left + TILE_SIZE;
		int bottom = (i.first.y - min_tile_y) * TILE_SIZE;
		int top = bottom + TILE_SIZE;

		if (tile.is_impassable() && tile.has_elevation())
		{
			if (tile.get_flags() & thoreau::Tile::flag_wall_diagonal_right)
			{
				image.draw_line(right, bottom, left, top, glm::vec4(1.0f));
			}
			else if (tile.get_flags() & thoreau::Tile::flag_wall_diagonal_left)
			{
				image.draw_line(left, bottom, right, top, glm::vec4(1.0f));
			}
			else
			{
				glm::vec4 color;
				if (tile.get_flags() & thoreau::Tile::flag_water)
				{
					color = glm::vec4(0.0f, 0.0f, 1.0f, 0.5f);
				}
				else
				{
					color = glm::vec4(1.0f, 0.0f, 0.0f, 0.5f);
				}

				for (int y = bottom; y <= top; ++y)
				{
					for (int x = left; x <= right; ++x)
					{
						image.set_pixel(x, y, color);
					}
				}
			}
		}

		if (tile.is_portal())
		{
			for (int y = bottom; y <= top; ++y)
			{
				for (int x = left; x <= right; ++x)
				{
					image.set_pixel(x, y, glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));
				}
			}

			continue;
		}

		if (!tile.is_wall() && !tile.is_door())
		{
			continue;
		}

		const auto wall_color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		const auto door_color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

		int flags = tile.get_flags();
		if (flags & thoreau::Tile::flag_door_left)
		{
			image.draw_line(left, top, left, bottom, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_left)
		{
			image.draw_line(left, top, left, bottom, wall_color);
		}

		if (flags & thoreau::Tile::flag_door_right)
		{
			image.draw_line(right, top, right, bottom, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_right)
		{
			image.draw_line(right, top, right, bottom, wall_color);
		}

		if (flags & thoreau::Tile::flag_door_top)
		{
			image.draw_line(left, top, right, top, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_top)
		{
			image.draw_line(left, top, right, top, wall_color);
		}

		if (flags & thoreau::Tile::flag_door_bottom)
		{
			image.draw_line(left, bottom, right, bottom, door_color);
		}
		else if (flags & thoreau::Tile::flag_wall_bottom)
		{
			image.draw_line(left, bottom, right, bottom, wall_color);
		}
	}

	auto texture = std::make_unique<kvncer::Texture>(
		kvncer::Texture::component_format_rgba, false,
		width, height);
	texture->update(
		0, 0, width, height,
		kvncer::Texture::component_format_rgba,
		kvncer::Texture::component_type_unsigned_byte,
		image.get_pixels());

	this->sector_wall_textures[sector_coordinate] = std::move(texture);
}

continent7::Application::ProxyPass::ProxyPass(Application* application)
{
	this->application = application;
}

void continent7::Application::ProxyPass::start()
{
	++this->application->current_frame;
}

void continent7::Application::ProxyPass::stop()
{
	auto& map = this->application->get_game_manager().get_runtime_map();
	auto& minimap = this->application->get_game_manager().get_minimap();
	auto& actor_bounds = this->application->get_game_manager().get_actor_bounds_state();
	thoreau::SectorCoordinate player_sector;

	if (minimap.get_is_visible())
	{
		float distance = HUGE_VALF;
		int layer = 0;
		for (auto& actor: actor_bounds)
		{
			auto world_coordinate = actor.first;
			auto tile_position = glm::vec2(world_coordinate.x, world_coordinate.y);
			tile_position /= glm::vec2(thoreau::TILE_SIZE);

			float difference = glm::length(tile_position - minimap.get_center());
			if (difference < distance)
			{
				distance = difference;
				layer = world_coordinate.layer;
			}
		}

		if (distance < 2.0f)
		{
			if (this->application->override_layer)
			{
				layer = this->application->current_layer;
			}
			else
			{
				this->application->current_layer = layer;
			}

			minimap.extract(map, layer);

			player_sector = thoreau::sector_from_tile(minimap.get_center_tile());
			player_sector.layer = layer;

			this->application->dirty_sectors.insert(player_sector);
			this->application->visible_sectors[player_sector] = true;

			this->application->player_sectors.insert(player_sector);
			if (this->application->include_adjacent)
			{
				for (int i = -1; i < 1; ++i)
				{
					for (int j = -1; j < 1; ++j)
					{
						auto adjacent = player_sector;
						adjacent.x += i;
						adjacent.y += j;

						this->application->player_sectors.insert(adjacent);
					}
				}
			}
		}
	}

	auto& scenery_elevations = this->application->relative_scenery_elevation_state;
	for (auto i: this->application->visible_tile_state)
	{
		auto tile_coordinate = i.first;

		this->application->dirty_sectors.insert(thoreau::sector_from_tile(tile_coordinate));
		this->application->visible_sectors[thoreau::sector_from_tile(tile_coordinate)] = true;

		if (this->application->visited_tiles.count(tile_coordinate) != 0)
		{
			continue;
		}
		this->application->visited_tiles.insert(tile_coordinate);

		if (!map.has_tile(tile_coordinate))
		{
			continue;
		}

		auto scenery_elevations_tile_coordinate = tile_coordinate;
		scenery_elevations_tile_coordinate.layer = 0;
		if (!scenery_elevations.has(scenery_elevations_tile_coordinate))
		{
			continue;
		}

		auto& elevations = scenery_elevations.get(scenery_elevations_tile_coordinate);
		auto max = elevations.get_max(tile_coordinate.layer);
		if (max == HUGE_VALF)
		{
			continue;
		}

		auto& tile = map.get_tile(tile_coordinate);
		if (!tile.has_elevation())
		{
			continue;
		}

		if (tile.get_height() != 0)
		{
			continue;
		}

		float height = max - tile.get_max_elevation();
		if (height > 0.0f)
		{
			tile.set_height(height);
		}
	}

	if (this->application->automatic_enabled)
	{
		this->application->apply_tile_heuristic();
		this->application->on_start_trace();
	}
	else
	{
		this->application->populate_scene();
	}
}

void continent7::Application::ProxyPass::draw_model(
	int tag, const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> points;
	if (!(chomsky::is_scenery_draw_event(event) ||
		chomsky::is_prop_draw_event(event)))
	{
		return;
	}

	points = transform_unskinned_model(model, event);
	add_model(tag, principle::Draw::draw_type_scenery, points, event);
}

void continent7::Application::ProxyPass::draw_terrain(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> points = transform_terrain(terrain, event);
	add_model(tag, principle::Draw::draw_type_terrain, points, event);
}

void continent7::Application::ProxyPass::draw_water(
	int tag, const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> points = transform_terrain(terrain, event);
	add_model(tag, principle::Draw::draw_type_water, points, event);
}

std::vector<glm::vec3> continent7::Application::ProxyPass::transform_unskinned_model(
	const kvre::Model* model,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> result;
	auto model_matrix = event.get_model_matrix();
	for (std::size_t i = 0; i < model->get_num_vertices(); ++i)
	{
		auto position = model->get_position(i);
		auto transformed_position = model_matrix * glm::vec4(position, 1.0f);
		result.push_back(glm::vec3(transformed_position));
	}

	return result;
}

std::vector<glm::vec3> continent7::Application::ProxyPass::transform_terrain(
	const kvre::Terrain* terrain,
	const kompvter::ModelDrawEvent& event)
{
	std::vector<glm::vec3> result;
	auto model_matrix = event.get_model_matrix();
	for (std::size_t i = 0; i < terrain->get_num_vertices(); ++i)
	{
		auto position = terrain->get_position(i);
		auto transformed_position = model_matrix * glm::vec4(position, 1.0f);
		result.push_back(glm::vec3(transformed_position));
	}

	return result;
}

void continent7::Application::ProxyPass::add_model(
	int tag,
	int draw_type,
	const std::vector<glm::vec3>& points,
	const kompvter::ModelDrawEvent& event)
{
	auto begin = kvre::IndexedVertexIterator::begin(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto end = kvre::IndexedVertexIterator::end(
		event.get_vertex_index_mapper(),
		event.get_draw_parameters());
	auto num_indices = std::distance(begin, end);
	if (num_indices == 0 || num_indices % 3 != 0)
	{
		return;
	}

	kvlc::Mesh mesh;
	mesh.set_num_points(points.size());
	auto mesh_points = mesh.get_points();
	for (std::size_t i = 0; i < points.size(); ++i)
	{
		mesh_points[i] = glm::vec4(points[i], 0.0f);
	}

	mesh.set_num_triangles(num_indices / 3);
	auto mesh_indices = mesh.get_triangles();
	for (std::size_t i = 0; i < num_indices; ++i)
	{
		auto current_index = *(begin + i);
		mesh_indices[i] = current_index;
	}

	auto scene_tag = this->application->map_collision_scene.add(mesh);
	this->application->map_collision_scene_tags[scene_tag] = std::make_pair(this->application->current_frame, tag);
	this->application->map_collision_scene_types[scene_tag] = draw_type;
}

void continent7::Application::update_current_tile()
{
	if (!this->current_tile_dirty)
	{
		return;
	}

	auto display = get_display();
	auto& map = get_game_manager().get_runtime_map();

	auto view_matrix = this->camera.get_view_matrix();
	auto projection_matrix = this->camera.get_projection_matrix();
	auto viewport = glm::vec4(0, 0, display->get_width(), display->get_height());

	auto v = glm::scale(glm::mat4(1.0f), glm::vec3(1 / 2048.0f)) * view_matrix;
	glm::vec3 a = glm::unProject(
		glm::vec3(this->mouse_position, 0.0f),
		v, projection_matrix, viewport);
	glm::vec3 b = glm::unProject(
		glm::vec3(this->mouse_position, 0.1f),
		v, projection_matrix, viewport);
	glm::vec3 c = glm::unProject(
		glm::vec3(this->mouse_position, 1.0f),
		v, projection_matrix, viewport);

	kvlc::Ray ray;
	ray.position = a;
	ray.direction = glm::normalize(b - a);
	float length = glm::length(c - a);

	kvlc::Query query;
	if (!this->map_collision_scene.test(ray, length, query))
	{
		return;
	}

	auto best_position = glm::vec3(0, 0, 0);
	auto best_elevation = -HUGE_VALF;
	for (auto i: query)
	{
		if (i.second.y > best_elevation)
		{
			best_position = i.second;
			best_elevation = i.second.y;
		}

		auto tile_coordinate = thoreau::tile_from_world(i.second);
		if (!map.has_tile(tile_coordinate.x, tile_coordinate.y) ||
			!map.try_get_tile_layer(tile_coordinate.x, tile_coordinate.y, i.second.y, tile_coordinate.layer))
		{
			continue;
		}

		auto& tile = map.get_tile(tile_coordinate);
		if (best_elevation > tile.get_elevation())
		{
			this->current_tile_elevation = i.second.y;
			this->current_tile_coordinate = tile_coordinate;
			//best_elevation = tile.get_elevation();
		}
	}

	this->current_tile_elevation = best_elevation;
	this->current_tile_dirty = false;
}

thoreau::TileCoordinate continent7::Application::get_current_tile_coordinate(bool override_layer)
{
	update_current_tile();

	if (override_layer)
	{
		auto result = this->current_tile_coordinate;
		result.layer = this->current_layer;

		return result;
	}

	return this->current_tile_coordinate;
}

float continent7::Application::get_current_tile_elevation()
{
	update_current_tile();
	return this->current_tile_elevation;
}

void continent7::Application::apply_tile_heuristic()
{
	auto& map = get_game_manager().get_runtime_map();
	for (auto& i: this->visible_tile_state)
	{
		if (i.first.layer != this->current_layer)
		{
			continue;
		}

		if (!map.has_tile(i.first))
		{
			continue;
		}

		auto& tile = map.get_tile(i.first);
		if (!tile.has_elevation())
		{
			continue;
		}

		glm::vec3 center;
		center.x = (i.first.x + 0.5f) * thoreau::TILE_SIZE;
		center.y = tile.get_min_elevation() + this->token_elevation;
		center.z = (i.first.y + 0.5f) * thoreau::TILE_SIZE;
		
		kvlc::Bounds bounds;
		bounds.min = center - this->token_size;
		bounds.max = center + this->token_size;

		kvlc::Query query;
		bool hit = false;
		if (this->map_collision_scene.test(bounds, query))
		{
			for (auto j: query)
			{
				if (this->map_collision_scene_types[j.first] == principle::Draw::draw_type_scenery ||
					this->map_collision_scene_types[j.first] == principle::Draw::draw_type_prop)
				{
					hit = true;
					break;
				}
			}
		}

		//bool is_tile_sparse = false;
		bool is_tile_sparse = continent1::heuristics::is_tile_sparse(i.first, this->candidate_manager);
		if (hit && !is_tile_sparse)// || continent1::heuristics::is_tile_impassable(i.first, this->candidate_manager))
		{
			auto sector_coordinate = thoreau::sector_from_tile(i.first);
			this->dirty_sectors.insert(sector_coordinate);

			if (map.has_tile(i.first))
			{
				auto& tile = map.get_tile(i.first);
				tile.append_flags(thoreau::Tile::flag_impassable);
				tile.unset_flags(thoreau::Tile::flag_passable);
			}
		}
	}

	notify_map_update();
}

static void make_box(
	kvncer::Mesh& mesh,
	kvncer::MeshBuffer& vertex_buffer,
	kvncer::MeshBuffer& index_buffer)
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	const float UNIT_XZ_MIN = -256.0f;
	const float UNIT_XZ_MAX = 256.0f;
	const float UNIT_Y_MIN = 0.0f;
	const float UNIT_Y_MAX = 1024.0f;
	const glm::vec4 COLOR = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);

	Vertex vertices[] =
	{
		{ glm::vec3(UNIT_XZ_MIN, UNIT_Y_MIN, UNIT_XZ_MIN), COLOR },
		{ glm::vec3(UNIT_XZ_MAX, UNIT_Y_MIN, UNIT_XZ_MIN), COLOR },
		{ glm::vec3(UNIT_XZ_MAX, UNIT_Y_MIN, UNIT_XZ_MAX), COLOR },
		{ glm::vec3(UNIT_XZ_MIN, UNIT_Y_MIN, UNIT_XZ_MAX), COLOR },
		{ glm::vec3(UNIT_XZ_MIN, UNIT_Y_MAX, UNIT_XZ_MIN), COLOR },
		{ glm::vec3(UNIT_XZ_MAX, UNIT_Y_MAX, UNIT_XZ_MIN), COLOR },
		{ glm::vec3(UNIT_XZ_MAX, UNIT_Y_MAX, UNIT_XZ_MAX), COLOR },
		{ glm::vec3(UNIT_XZ_MIN, UNIT_Y_MAX, UNIT_XZ_MAX), COLOR }
	};

	std::uint32_t indices[] =
	{
		// Top
		0, 1,
		1, 2,
		2, 3,
		3, 0,

		// Bottom
		4, 5,
		5, 6,
		6, 7,
		7, 4,

		// Corners
		0, 4,
		1, 5,
		2, 6,
		3, 7
	};

	kompvter::Buffer vertex_buffer_data;
	vertex_buffer_data.allocate(sizeof(vertices));
	vertex_buffer_data.update(0, (const std::uint8_t*)vertices, sizeof(vertices));
	vertex_buffer.set_buffer(vertex_buffer_data);

	kompvter::Buffer index_buffer_data;
	index_buffer_data.allocate(sizeof(indices));
	index_buffer_data.update(0, (const std::uint8_t*)indices, sizeof(indices));
	index_buffer.set_buffer(index_buffer_data);

	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position,
		&vertex_buffer,
		3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_color,
		&vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color), sizeof(Vertex));
	mesh.set_indices(
		&index_buffer,
		GL_UNSIGNED_INT, 0, sizeof(indices));
}

void continent7::Application::draw_box(const glm::vec3& position)
{
	auto model_matrix = glm::translate(glm::mat4(1.0f), position);
	auto shader = this->scene_shader;

	shader->use();
	shader->set("principle_MinBounds", glm::vec3(-HUGE_VALF));
	shader->set("principle_MaxBounds", glm::vec3(HUGE_VALF));
	shader->set("principle_Model", model_matrix);
	shader->set("principle_View", this->camera.get_view_matrix());
	shader->set("principle_Projection", this->camera.get_projection_matrix());

	this->box_mesh.draw(GL_LINES);
}
