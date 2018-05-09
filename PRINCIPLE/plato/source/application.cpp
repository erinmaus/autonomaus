// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/color_space.hpp>
#include <boost/filesystem.hpp>
#include "kommon/graphics/image.hpp"
#include "plato/application.hpp"
#include "plato/tools/tool.hpp"
#include "plato/tools/complexShapeTool.hpp"
#include "plato/tools/previewTool.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/graphics/guiManager.hpp"
#include "principle/graphics/imageStore.hpp"
#include "principle/deps/imgui/imgui.h"
#include "thoreau/thoreau.hpp"

void make_quad(kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer)
{
	struct Vertex
	{
		glm::vec2 position;
		glm::vec2 texture;
	};

	Vertex vertices[] =
	{
		{ glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
		{ glm::vec2(1.0f, 0.0f), glm::vec2(1.0f, 0.0f) },
		{ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec2(1.0f, 1.0f), glm::vec2(1.0f, 1.0f) },
		{ glm::vec2(0.0f, 1.0f), glm::vec2(0.0f, 1.0f) },
		{ glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f) },
	};

	kompvter::Buffer vertex_buffer_data;
	vertex_buffer_data.allocate(sizeof(vertices));
	vertex_buffer_data.update(0, (const std::uint8_t*)vertices, vertex_buffer_data.get_data_size());
	vertex_buffer.set_buffer(vertex_buffer_data);
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position,
		&vertex_buffer,
		2, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_texture_coordinate,
		&vertex_buffer,
		2, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, texture), sizeof(Vertex));
	mesh.set_indices(nullptr, GL_NONE, 0, 6);
}

plato::Application::Application() :
	principle::Application("plato"),
	main_window(this),
	import_dat_window(this),
	resources_window(this),
	edit_tile_window(this),
	find_location_window(this),
	find_resource_window(this),
	export_window(this),
	tool_renderer(this),
	tool_factory(this),
	visible_tile_pass(&get_game_manager().get_transform_state(), &get_game_manager().get_terrain_elevation_state(), &this->visible_tile_state)
{
	this->quad_shader = principle::Material::load_shader_from_file(
		"sprite.vert.glsl", "sprite.frag.glsl");
	this->quad_shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);
	this->quad_shader->bind_input("vert_Texture", kvncer::Shader::vertex_input_texture_coordinate);
	make_quad(this->quad, this->quad_vertex_buffer);

	get_game_manager().get_world_update_manager().add_pass(&visible_tile_pass);
	
	auto data_path = std::getenv("NECRONOMICON_DATA_PATH");
	if (data_path != nullptr)
	{
		std::string map_path = std::string(data_path) + "/map";
		std::string brochure_path = std::string(data_path) + "/database";

		try
		{
			load_map(map_path);
			load_brochure(brochure_path);
		}
		catch (const std::exception& e)
		{
			std::cout << e.what() << std::endl;
		}
	}
}

plato::Application::~Application()
{
	delete this->quad_shader;
	clear_tiles();
}

void plato::Application::update(float delta)
{
	ALLEGRO_MOUSE_STATE mouse_state;
	al_get_mouse_state(&mouse_state);
	this->current_mouse_x = mouse_state.x;
	this->current_mouse_y = mouse_state.y;
}

void plato::Application::draw()
{
	principle::Application::draw();

	glDisable(GL_DEPTH_TEST);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto view = get_view_matrix();
	auto projection = get_projection_matrix();

	for (auto sector_coordinate: this->visible_sectors)
	{
		if (sector_coordinate.layer != this->current_layer)
		{
			continue;
		}

		auto position = glm::translate(glm::mat4(1.0f), glm::vec3(sector_coordinate.x, sector_coordinate.y, 0));
		auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(thoreau::SECTOR_SIZE, thoreau::SECTOR_SIZE, 1));
		auto transformed_view = view * scale * position;

		draw_sector(sector_coordinate, transformed_view, projection);
	}

	if (this->draw_elevations)
	{
		for (auto& i: this->sector_elevation_textures)
		{
			if (i.first.layer != this->current_layer)
			{
				continue;
			}

			auto sector_coordinate = i.first;
			auto position = glm::translate(glm::mat4(1.0f), glm::vec3(sector_coordinate.x, sector_coordinate.y, 0));
			auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(thoreau::SECTOR_SIZE, thoreau::SECTOR_SIZE, 1));
			auto transformed_view = view * scale * position;

			draw_sector_elevations(i.first, transformed_view, projection);
		}
	}

	if (this->draw_walls)
	{
		for (auto& i: this->sector_wall_textures)
		{
			if (i.first.layer != this->current_layer)
			{
				continue;
			}

			auto sector_coordinate = i.first;
			auto position = glm::translate(glm::mat4(1.0f), glm::vec3(sector_coordinate.x, sector_coordinate.y, 0));
			auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(thoreau::SECTOR_SIZE, thoreau::SECTOR_SIZE, 1));
			auto transformed_view = view * scale * position;

			draw_sector_walls(i.first, transformed_view, projection);
		}
	}

	if (this->preview_tool && this->draw_atlas)
	{
		this->preview_tool->draw(this->tool_renderer);
	}

	if (this->toolbox)
	{
		this->toolbox->draw(this->tool_renderer);
	}

	this->tool_renderer.draw();
}

void plato::Application::update_gui()
{
	auto& imgui = ImGui::GetIO();

	if (!this->menu.empty())
	{
		static const int WINDOW_FLAGS = ImGuiWindowFlags_NoTitleBar
			| ImGuiWindowFlags_NoScrollbar
			| ImGuiWindowFlags_NoResize
			| ImGuiWindowFlags_NoSavedSettings;

		ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
		ImGui::SetNextWindowPos(ImVec2(current_mouse_x, current_mouse_y));
		ImGui::Begin("plato_menu_window", nullptr, WINDOW_FLAGS);
		{
			ImGui::SetWindowSize(ImVec2(1, 1));
			if (ImGui::BeginPopup("plato_tool_selector"))
			{
				this->menu_open = true;
				if (this->menu.empty())
				{
					ImGui::CloseCurrentPopup();
				}

				int id = 0;
				for (auto& item: this->menu)
				{
					ImGui::PushID(id);
					if (ImGui::MenuItem(item.first.c_str()))
					{
						item.second();
					}

					ImGui::PopID();
				}

				if (ImGui::MenuItem("Cancel"))
				{
					clear_menu();
				}

				ImGui::EndPopup();
			}
			else
			{
				if (this->menu_open)
				{
					this->menu_open = false;
					clear_menu();
				}
				else if (!this->menu.empty())
				{
					ImGui::OpenPopup("plato_tool_selector");
				}
			}
		}
		ImGui::End();
		ImGui::PopStyleColor();
	}

	if (this->show_current_tile_info && !imgui.WantCaptureMouse)
	{
		auto& map = get_game_manager().get_map();
		auto& minimap = get_game_manager().get_minimap();

		auto tile_coordinate = thoreau::tile_from_world(this->mouse_world_position);
		tile_coordinate.layer = this->current_layer;

		auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);

		ImGui::BeginTooltip();
		ImGui::Text(
			"Tile (%d, %d; %d)",
			tile_coordinate.x,
			tile_coordinate.y,
			tile_coordinate.layer);
		ImGui::Text(
			"Sector (%d, %d; %d)",
			sector_coordinate.x,
			sector_coordinate.y,
			sector_coordinate.layer);

		if (this->visible_tile_state.has(tile_coordinate))
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

			if (tile.is_wall())
			{
				ImGui::BulletText("is wall");
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

		ImGui::EndTooltip();
	}

	principle::Application::update_gui();

	for (auto window: this->pending_windows)
	{
		this->windows.erase(window);
		delete window;
	}
	this->pending_windows.clear();
}

void plato::Application::on_mouse_move(const principle::MouseEvent& event)
{
	if (this->is_camera_dragging)
	{
		this->camera_offset.x += event.get_difference().x / this->camera_scale;
		this->camera_offset.y -= event.get_difference().y / this->camera_scale;
	}

	if (this->is_camera_zooming)
	{
		this->camera_scale += event.get_difference().y / 16.0f;
		this->camera_scale = std::max(this->camera_scale, 1.0f);
	}

	auto position = event.get_position();
	position.y = get_display()->get_height() - position.y;

	auto view_matrix = glm::inverse(get_view_matrix());
	auto transformed_position = view_matrix * glm::vec4(position, 0.0f, 1.0f);
	transformed_position *= (float)thoreau::TILE_SIZE;

	this->mouse_world_position.x = transformed_position.x;
	this->mouse_world_position.y = transformed_position.y;
	this->mouse_world_position.layer = this->current_layer;

	this->current_mouse_tile_position = thoreau::tile_from_world(this->mouse_world_position);

	if (this->toolbox)
	{
		this->toolbox->set_mouse_tile_coordinate(this->current_mouse_tile_position);
		this->toolbox->on_mouse_move(event);
	}
}

void plato::Application::on_mouse_button_down(
	const principle::MouseEvent& event)
{
	if (event.get_button() == 1)
	{
		if (!this->toolbox || !this->toolbox->active())
		{
			clear_menu();

			add_menu_option("Edit Tile", [this]()
			{
				if (this->is_multiselection)
				{
					this->edit_tile_window.add_tile(this->current_mouse_tile_position);
				}
				else
				{
					this->edit_tile_window.set_tile(this->current_mouse_tile_position);
				}
			});

			if (this->atlas)
			{
				add_menu_option("New Location", [this]()
				{
					this->tool_factory.add_new_location();
				});

				add_menu_option("New Child Location", [this]()
				{
					this->tool_factory.add_new_child_location();
				});

				mapp::Location hover;
				if (this->atlas->find(this->current_mouse_tile_position, hover))
				{
					auto tile_coordinate = this->current_mouse_tile_position;
					int count = 0;
					while (hover.resource.get_id() &&
						hover.resource.get_id() != this->atlas->root().resource.get_id() &&
						count < 2)
					{
						std::string label = "New Child Location (";
						label += hover.resource.get_name();
						label += ")";

						add_menu_option(label, [this, hover]()
						{
							this->tool_factory.add_new_child_location(hover);
						});

						label = "Add Waypoint (";
						label += hover.resource.get_name();
						label += ")";

						add_menu_option(label, [this, hover, tile_coordinate]()
						{
							this->tool_factory.add_new_waypoint(hover, tile_coordinate);
						});

						label = "Set Anchor (";
						label += hover.resource.get_name();
						label += ")";
						add_menu_option(label, [this, hover, tile_coordinate]
						{
							if (this->atlas)
							{
								mapp::LocationDefinition definition = hover;
								definition.has_anchor = true;
								definition.anchor = tile_coordinate;
								this->atlas->update(hover, definition);
							}
						});

						label = "Edit (";
						label += hover.resource.get_name();
						label += ")";
						add_menu_option(label, [this, hover]()
						{
							this->tool_factory.edit(hover.resource);
						});

						hover = this->atlas->parent(hover);
						++count;
					}
				}
			}

			add_menu_option("Remove Sector", [this]()
			{
				this->remove_sector(thoreau::sector_from_tile(this->current_mouse_tile_position));
			});
		}
	}
	else if (event.get_button() == 2)
	{
		this->is_camera_dragging = true;
	}
	else if (event.get_button() == 3)
	{
		this->is_camera_zooming = true;
	}

	if (this->toolbox)
	{
		this->toolbox->on_mouse_button_down(event);
	}
}

void plato::Application::on_mouse_button_up(const principle::MouseEvent& event)
{
	if (event.get_button() == 2)
	{
		this->is_camera_dragging = false;
	}
	else if (event.get_button() == 3)
	{
		this->is_camera_zooming = false;
	}

	if (this->toolbox)
	{
		this->toolbox->on_mouse_button_up(event);
	}
}

void plato::Application::on_key_down(const principle::KeyboardEvent& event)
{
	if (event.get_key_code() == ALLEGRO_KEY_LSHIFT ||
		event.get_key_code() == ALLEGRO_KEY_RSHIFT)
	{
		++this->is_multiselection;
	}

	if (event.get_key_code() == ALLEGRO_KEY_LCTRL ||
		event.get_key_code() == ALLEGRO_KEY_RCTRL)
	{
		this->show_current_tile_info = !this->show_current_tile_info;
	}

	if (event.get_key_code() == ALLEGRO_KEY_F)
	{
		auto current_tile_coordinate = thoreau::tile_from_world(this->mouse_world_position);

		auto& map = get_game_manager().get_map();
		if (!map.has_tile(current_tile_coordinate))
		{
			map.add_tile(current_tile_coordinate);
		}

		if (this->is_multiselection)
		{
			this->edit_tile_window.add_tile(current_tile_coordinate);
		}
		else
		{
			this->edit_tile_window.set_tile(current_tile_coordinate);
		}
	}

	if (this->toolbox)
	{
		this->toolbox->on_key_down(event);
	}
}

void plato::Application::on_key_up(const principle::KeyboardEvent& event)
{
	if (event.get_key_code() == ALLEGRO_KEY_LSHIFT ||
		event.get_key_code() == ALLEGRO_KEY_RSHIFT)
	{
		--this->is_multiselection;
	}

	if (this->toolbox)
	{
		this->toolbox->on_key_up(event);
	}
}

void plato::Application::on_start_trace()
{
	this->visible_tile_state.clear();
	clear_tiles();
}

void plato::Application::on_finish_trace()
{
	load_tiles();
}

void plato::Application::remove_sector(const thoreau::SectorCoordinate& sector_coordinate)
{
	auto& map = get_game_manager().get_map();
	auto tile_coordinate = thoreau::tile_from_sector(sector_coordinate);
	std::map<thoreau::TileCoordinate, thoreau::Tile> tiles;
	for (auto i = 0; i < thoreau::SECTOR_SIZE; ++i)
	{
		for (auto j = 0; j < thoreau::SECTOR_SIZE; ++j)
		{
			auto t = tile_coordinate;
			t.x += i;
			t.y += j;

			if (!map.has_tile(t.x, t.y))
			{
				continue;
			}

			int num_layers = map.get_num_layers(t.x, t.y);
			for (int k = 0; k < num_layers; ++k)
			{
				if (k == sector_coordinate.layer)
				{
					continue;
				}

				auto s = t;
				if (k > sector_coordinate.layer)
				{
					s.layer = k - 1;
				}
				else
				{
					s.layer = k;
				}

				auto r = t;
				r.layer = k;
				if (map.has_tile(r))
				{
					tiles.emplace(s, map.get_tile(r));
				}
			}

			map.remove_tile(t.x, t.y);
		}
	}

	this->sector_textures.erase(sector_coordinate);
	this->sector_wall_textures.erase(sector_coordinate);
	this->dirty_sectors.erase(sector_coordinate);

	for (auto& i: tiles)
	{
		this->modified_tiles.insert(i.first);
		map.add_tile(i.first) = i.second;
		this->dirty_sectors.insert(thoreau::sector_from_tile(i.first));
	}
	notify_map_update();
}

void plato::Application::add_window(principle::Window* window)
{
	this->windows.insert(window);
}

void plato::Application::remove_window(principle::Window* window)
{
	this->pending_windows.insert(window);
}

void plato::Application::change_layer(int layer)
{
	this->current_layer = layer;
	if (this->toolbox)
	{
		this->toolbox->set_current_layer(this->current_layer);
	}
}

glm::mat4 plato::Application::get_projection_matrix()
{
	return glm::ortho(
		0.0f, (float)get_display()->get_width(),
		0.0f, (float)get_display()->get_height(),
		-1.0f, 1.0f);
}

glm::mat4 plato::Application::get_view_matrix()
{
	auto translation = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(glm::floor(this->camera_offset), 0.0f));
	auto scale = glm::scale(
		glm::mat4(1.0f),
		glm::vec3(glm::floor(this->camera_scale), glm::floor(this->camera_scale), 1.0f));
	auto from_origin = glm::translate(
		glm::mat4(1.0f),
		glm::vec3(get_display()->get_width() / 2.0f, get_display()->get_height() / 2.0f, 0.0f));
	auto to_origin = glm::translate(
		glm::mat4(1.0f),
		-glm::vec3(get_display()->get_width() / 2.0f, get_display()->get_height() / 2.0f, 0.0f));

	return from_origin * scale * to_origin * translation;
}

void plato::Application::load_tiles()
{
	auto& map = get_game_manager().get_map();
	this->bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	for (auto& i: this->visible_tile_state)
	{
		auto tile_coordinate = i.first;
		this->dirty_sectors.insert(thoreau::sector_from_tile(tile_coordinate));

		if (!map.has_tile(tile_coordinate))
		{
			continue;
		}

		auto& tile = map.get_tile(tile_coordinate);
		if (!tile.has_elevation())
		{
			continue;
		}

		glm::vec3 position;
		position.x = i.first.x * thoreau::TILE_SIZE;
		position.y = tile.get_elevation();
		position.z = i.first.y * thoreau::TILE_SIZE;
		this->bounds.expand(position);
	}

	auto center = this->bounds.compute_center();
	this->camera_offset.x = -(center.x / thoreau::TILE_SIZE);
	this->camera_offset.x += get_display()->get_width() / 2.0f;
	this->camera_offset.y = -(center.z / thoreau::TILE_SIZE);
	this->camera_offset.y += get_display()->get_height() / 2.0f;

	for (auto& i: this->visible_tile_state)
	{
		load_tile(i.first);
		this->visible_sectors.insert(thoreau::sector_from_tile(i.first));
	}

	notify_map_update();
}

void plato::Application::load_tile(const thoreau::TileCoordinate& tile_coordinate)
{
	auto sector_coordinate = thoreau::sector_from_tile(tile_coordinate);
	if (this->sector_textures.count(sector_coordinate) != 0)
	{
		return;
	}

	std::string path;
	{
		const char* root = std::getenv("NECRONOMICON_DATA_PATH");
		if (root == nullptr)
		{
			path = "./";
		}
		else
		{
			path = root;
			path += "/../samples/map/";
		}

		char filename[128];
		std::snprintf(
			filename, sizeof(filename), "%03dx%03dx%02d",
			sector_coordinate.x, sector_coordinate.y, sector_coordinate.layer);
		path += filename;
	}

	const std::size_t TILE_LENGTH = 64;
	const std::size_t SECTOR_LENGTH = thoreau::SECTOR_SIZE * thoreau::SECTOR_SIZE;
	const std::size_t IMAGE_LENGTH = TILE_LENGTH * thoreau::SECTOR_SIZE;
	principle::ImageStore<glm::u8vec4, SECTOR_LENGTH, TILE_LENGTH, TILE_LENGTH> image_store;
	if (boost::filesystem::exists(path + "-color.bin"))
	{
		image_store.read(path + "-color.bin");
	}
	else
	{
		image_store.clear(glm::u8vec4(0));
	}

	auto texture = std::make_unique<kvncer::Texture>(
		kvncer::Texture::component_format_rgba, false,
		IMAGE_LENGTH, IMAGE_LENGTH);

	std::size_t index = 0;
	for (int j = 0; j < thoreau::SECTOR_SIZE; ++j)
	{
		for (int i = 0; i < thoreau::SECTOR_SIZE; ++i)
		{
			texture->update(
				i * TILE_LENGTH, j * TILE_LENGTH,
				(int)TILE_LENGTH, (int)TILE_LENGTH,
				kvncer::Texture::component_format_rgba,
				kvncer::Texture::component_type_unsigned_byte,
				&image_store.get(index)->x);
			++index;
		}
	}

	this->sector_textures[thoreau::sector_from_tile(tile_coordinate)] = std::move(texture);
}

void plato::Application::clear_tiles()
{
	this->sector_textures.clear();
}

void plato::Application::notify_map_update()
{
	for (auto sector: this->dirty_sectors)
	{
		update_sector_elevations_texture(sector);
		update_sector_walls_texture(sector);
	}

	this->dirty_sectors.clear();
}

void plato::Application::update_sector_elevations_texture(
	const thoreau::SectorCoordinate& sector_coordinate)
{
	auto& map = get_game_manager().get_map();
	
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
		if (!tile.has_elevation())
		{
			continue;
		}

		tiles[tile_coordinate] = tile;
	}

	float min_elevation = this->bounds.min.y;
	float max_elevation = this->bounds.max.y;

	const int TILE_SIZE = 16;
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

void plato::Application::update_sector_walls_texture(
	const thoreau::SectorCoordinate& sector_coordinate)
{
	auto& map = get_game_manager().get_map();

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
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	this->sector_wall_textures[sector_coordinate] = std::move(texture);
}

void plato::Application::draw_sector(
	const thoreau::SectorCoordinate& sector_coordinate,
	const glm::mat4& view, const glm::mat4& projection)
{
	auto sector = this->sector_textures.find(sector_coordinate);
	if (sector == this->sector_textures.end())
	{
		return;
	}

	sector->second->bind(0);

	this->quad_shader->use();
	this->quad_shader->set("principle_Texture", 1);
	this->quad_shader->set("principle_View", view);
	this->quad_shader->set("principle_Projection", projection);

	this->quad.draw();
}

void plato::Application::draw_sector_elevations(
	const thoreau::SectorCoordinate& sector_coordinate,
	const glm::mat4& view, const glm::mat4& projection)
{
	auto texture = this->sector_elevation_textures.find(sector_coordinate);
	if (texture == this->sector_elevation_textures.end())
	{
		return;
	}

	texture->second->bind(0);

	this->quad_shader->use();
	this->quad_shader->set("principle_Texture", 1);
	this->quad_shader->set("principle_View", view);
	this->quad_shader->set("principle_Projection", projection);

	this->quad.draw();
}

void plato::Application::draw_sector_walls(
	const thoreau::SectorCoordinate& sector_coordinate,
	const glm::mat4& view, const glm::mat4& projection)
{
	auto texture = this->sector_wall_textures.find(sector_coordinate);
	if (texture == this->sector_wall_textures.end())
	{
		return;
	}

	texture->second->bind(0);

	this->quad_shader->use();
	this->quad_shader->set("principle_Texture", 1);
	this->quad_shader->set("principle_View", view);
	this->quad_shader->set("principle_Projection", projection);

	this->quad.draw();
}

void plato::Application::load_map(const std::string& filename)
{
	auto& map = get_game_manager().get_map();
	map.clear();

	std::ifstream stream(filename);
	std::size_t count = map.load(stream);

	if (stream.bad())
	{
		std::fprintf(stderr, "couldn't load map '%s'\n", filename.c_str());
	}
	else
	{
		std::fprintf(stderr, "loaded %lu tiles from map '%s'\n", count, filename.c_str());
		get_game_manager().get_runtime_map().merge(map);
	}
}

#include "plato/gui/itemResource.hpp"
#include "plato/gui/locationResource.hpp"
#include "plato/gui/shopResource.hpp"

void plato::Application::load_brochure(const std::string& filename)
{
	this->brochure = std::make_unique<twoflower::Brochure>(filename);
	this->atlas = std::make_unique<mapp::Atlas>(*this->brochure);
	this->game = std::make_unique<mapp::Game>(*this->brochure);
	this->toolbox = std::make_unique<Toolbox>(
		this->tool_factory,
		get_game_manager().get_map(),
		*this->brochure,
		*this->atlas);
	this->preview_tool = std::make_unique<PreviewTool>(*this->toolbox);

	this->brochure_widget_manager = std::make_unique<BrochureWidgetManager>(*this->brochure, *this->atlas, *this->game, *this->toolbox);
	this->brochure_widget_manager->add("location", new LocationResourceView());
	this->brochure_widget_manager->add("location", new LocationResourceEditor());
	this->brochure_widget_manager->add("item", new ItemResourceView());
	this->brochure_widget_manager->add("item", new ItemResourceEditor());
	this->brochure_widget_manager->add("shop", new ShopResourceView());
	this->brochure_widget_manager->add("shop", new ShopResourceEditor());
}

void plato::Application::add_menu_option(
	const std::string& label, const std::function<void()>& callback)
{
	this->menu.emplace_back(label, callback);
}

void plato::Application::clear_menu()
{
	this->menu.clear();
}

plato::Application::ApplicationToolRenderer::ApplicationToolRenderer(Application* application)
{
	this->application = application;
}

glm::vec2 plato::Application::ApplicationToolRenderer::get_camera_offset() const
{
	return this->application->camera_offset;
}

float plato::Application::ApplicationToolRenderer::get_camera_scale() const
{
	return this->application->camera_scale;
}

int plato::Application::ApplicationToolRenderer::get_viewport_width() const
{
	return this->application->get_display()->get_width();
}

int plato::Application::ApplicationToolRenderer::get_viewport_height() const
{
	return this->application->get_display()->get_height();
}

glm::mat4 plato::Application::ApplicationToolRenderer::get_view_matrix() const
{
	return this->application->get_view_matrix();
}

glm::mat4 plato::Application::ApplicationToolRenderer::get_projection_matrix() const
{
	return this->application->get_projection_matrix();
}

void plato::Application::ApplicationToolRenderer::render(ImDrawList& draw_list)
{
	this->application->get_gui_manager().render(draw_list);
}

plato::Application::ApplicationToolFactory::ApplicationToolFactory(Application* application)
{
	this->application = application;
}

void plato::Application::ApplicationToolFactory::add_new_location()
{
	auto tool = new ComplexShapeTool(
		*this->application->toolbox,
		[this](auto shape)
		{
			mapp::LocationDefinition definition;
			definition.name = "New Location";
			definition.shape = shape;

			auto result = this->application->atlas->add(definition);
			this->edit(result.resource);
		});

	this->application->toolbox->use(tool);
}

void plato::Application::ApplicationToolFactory::add_new_child_location(
	const mapp::Location& parent)
{
	auto tool = new ComplexShapeTool(
		*this->application->toolbox,
		[this, parent](auto shape)
		{
			mapp::LocationDefinition definition;
			definition.name = "New Location";
			definition.shape = shape;

			auto result = this->application->atlas->add(definition, parent);
			this->edit(result.resource);
		});

	this->application->toolbox->use(tool);
}

void plato::Application::ApplicationToolFactory::add_new_child_location()
{
	this->application->find_location_window.open(
		[this](auto parent)
		{
			this->add_new_child_location(parent);
		});
}

void plato::Application::ApplicationToolFactory::add_new_waypoint(
	const mapp::Location& parent,
	const thoreau::TileCoordinate& tile_coordinate)
{
	mapp::LocationDefinition definition;
	definition.name = "New Waypoint";
	definition.shape.add_point(tile_coordinate);

	auto location = this->application->atlas->add(definition, parent);
	this->application->brochure->builder().add_tag(location.resource, "mapp-waypoint");

	edit(location.resource);
}

void plato::Application::ApplicationToolFactory::add_new_link(const mapp::Location& from)
{
	// TODO
}

void plato::Application::ApplicationToolFactory::find_location(const std::function<void(mapp::Location)>& callback)
{
	this->application->find_location_window.open(
		[this, callback](auto parent)
		{
			callback(parent);
		});
}

void plato::Application::ApplicationToolFactory::find_resource(
	const std::string& type,
	const std::function<void(twoflower::Resource)>& callback)
{
	this->application->find_resource_window.open(
		type,
		[this, callback](auto parent)
		{
			callback(parent);
		});
}

void plato::Application::ApplicationToolFactory::view(const twoflower::Resource& resource)
{
	this->application->add_window(new ViewResourceWindow(resource, this->application));
}

void plato::Application::ApplicationToolFactory::edit(const twoflower::Resource& resource)
{
	this->application->add_window(new EditResourceWindow(resource, this->application));
}

void plato::Application::ApplicationToolFactory::view(const twoflower::Action& action)
{
	this->application->add_window(new ViewActionWindow(action, this->application));
}

void plato::Application::ApplicationToolFactory::edit(const twoflower::Action& action)
{
	this->application->add_window(new EditActionWindow(action, this->application));
}
