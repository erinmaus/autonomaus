// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include <fstream>
#include <boost/filesystem.hpp>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "thoreau/thoreau.hpp"
#include "thoreau/worldCoordinate.hpp"
#include "kommon/graphics/image.hpp"
#include "kompvter/graphics/buffer.hpp"
#include "kvncer/render/gl.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "principle/common/mouseEvent.hpp"
#include "principle/common/keyboardEvent.hpp"
#include "principle/deps/imgui/imgui.h"
#include "principle/graphics/utility.hpp"
#include "principle/graphics/imageStore.hpp"
#include "principle/scene/maxTileElevationsNode.hpp"
#include "principle/scene/minTileElevationsNode.hpp"
#include "principle/scene/tilePointCloudNode.hpp"
#include "principle/scene/modelNode.hpp"
#include "zeitgeist/application.hpp"

const int zeitgeist::Application::THUMBNAIL_WIDTH;
const int zeitgeist::Application::THUMBNAIL_HEIGHT;

static void make_sphere(kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer, kvncer::MeshBuffer& index_buffer)
{
	static const int NUM_STEPS = 16;
	static const int NUM_VERTICES = (NUM_STEPS + 1) * (NUM_STEPS + 1) * 2;
	static const int NUM_INDICES = NUM_STEPS * (NUM_STEPS + 1) * 6;
	std::uint32_t indices[NUM_INDICES];

	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};
	Vertex vertices[NUM_VERTICES];

	std::uint32_t vertex_offset = 0;
	std::uint32_t index_offset = 0;
	std::uint32_t current_index = 0;
	for (int i = 0; i <= NUM_STEPS; ++i)
	{
		float lat0 = M_PI * (-0.5 + (float)(i - 1) / NUM_STEPS);
		float z0 = std::sin(lat0);
		float zr0 = std::cos(lat0);

		float lat1 = M_PI * (-0.5 + (float)i / NUM_STEPS);
		float z1 = std::sin(lat1);
		float zr1 = std::cos(lat1);

		for (int j = 0; j <= NUM_STEPS; ++j)
		{
			auto lng = 2 * M_PI * (float)(j - 1) / NUM_STEPS;
			float x = std::cos(lng);
			float y = std::sin(lng);

			vertices[vertex_offset].color = glm::vec4(1.0f, 0.5f, 0.0f, 0.5f);
			vertices[vertex_offset].position = glm::vec3(x * zr0, z0, y * zr0) * glm::vec3(512.0f, 512.0f, 512.0f);
			++vertex_offset;
			vertices[vertex_offset].color = glm::vec4(1.0f, 0.5f, 0.0f, 0.5f);
			vertices[vertex_offset].position = glm::vec3(x * zr1, z1, y * zr1) * glm::vec3(512.0f, 512.0f, 512.0f);
			++vertex_offset;
		}

		indices[index_offset++] = current_index + 0;
		indices[index_offset++] = current_index + 1;
		indices[index_offset++] = current_index + 2;
		current_index += 3;
		for (int j = 0; j <= (NUM_STEPS - 1) * 2; ++j)
		{
			if (j % 2 != 0)
			{
				indices[index_offset++] = current_index + j - 2;
				indices[index_offset++] = current_index + j - 1;
				indices[index_offset++] = current_index + j - 0;
			}
			else
			{
				indices[index_offset++] = current_index + j - 1;
				indices[index_offset++] = current_index + j - 2;
				indices[index_offset++] = current_index + j - 0;
			}
		}
		current_index += NUM_STEPS * 2 - 1;
	}

	kompvter::Buffer vertex_buffer_data;
	vertex_buffer_data.allocate(NUM_VERTICES * sizeof(Vertex));
	vertex_buffer_data.update(0, (const std::uint8_t*)vertices, vertex_buffer_data.get_data_size());
	vertex_buffer.set_buffer(vertex_buffer_data);
	mesh.enable_vertex_attrib(
			kvncer::Shader::vertex_input_position,
			&vertex_buffer,
			3, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
			kvncer::Shader::vertex_input_color,
			&vertex_buffer,
			4, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, color), sizeof(Vertex));

	kompvter::Buffer index_buffer_data;
	index_buffer_data.allocate(NUM_INDICES * sizeof(std::uint32_t));
	index_buffer_data.update(0, (const std::uint8_t*)indices, index_buffer_data.get_data_size());
	index_buffer.set_buffer(index_buffer_data);
	mesh.set_indices(&index_buffer, GL_UNSIGNED_INT, 0, NUM_INDICES * 4);
}

zeitgeist::Application::Application() :
	principle::Application("zeitgeist"),
	proxy_pass(this),
	visibility_filter_window(this),
	model_draw_list_window(this),
	selection_window(this),
	actor_draw_list_window(this),
	animation_builder_window(this),
	point_cloud_match_builder_window(this),
	materials_set_window(this),
	material_set_match_builder_window(this),
	query_results_window(this),
	visible_tile_pass(&get_game_manager().get_transform_state(), &get_game_manager().get_terrain_elevation_state(), &this->visible_tile_state),
	camera_controller(&this->camera)
{
	this->models_scene.set_camera(&this->camera);
	this->models_shader = principle::Material::load_shader_from_file(
		"skinned_model.vert.glsl", "textured_model.frag.glsl");

	this->terrain_scene.set_camera(&this->camera);
	this->terrain_shader = principle::Material::load_shader_from_file(
		"terrain.vert.glsl", "terrain.frag.glsl");
	this->terrain_material.set_shader(this->terrain_shader);

	this->min_terrain_y.set_camera(&this->camera);
	this->min_terrain_y_shader = principle::Material::load_shader_from_file(
		"point.vert.glsl", "constant_colored_point.frag.glsl");
	this->min_terrain_y_material.set_shader(this->min_terrain_y_shader);
	this->min_terrain_y_material.set_point_size(4.0f);
	this->min_terrain_y_material.set_color(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	this->max_terrain_y.set_camera(&this->camera);
	this->max_terrain_y_shader = principle::Material::load_shader_from_file(
		"point.vert.glsl", "constant_colored_point.frag.glsl");
	this->max_terrain_y_material.set_shader(this->max_terrain_y_shader);
	this->max_terrain_y_material.set_point_size(4.0f);
	this->max_terrain_y_material.set_color(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	this->min_scenery_y.set_camera(&this->camera);
	this->min_scenery_y_shader = principle::Material::load_shader_from_file(
		"point.vert.glsl", "constant_colored_point.frag.glsl");
	this->min_scenery_y_material.set_shader(this->min_scenery_y_shader);
	this->min_scenery_y_material.set_point_size(4.0f);
	this->min_scenery_y_material.set_color(glm::vec4(0.0f, 1.0f, 0.0f, 1.0f));

	this->max_scenery_y.set_camera(&this->camera);
	this->max_scenery_y_shader = principle::Material::load_shader_from_file(
		"point.vert.glsl", "constant_colored_point.frag.glsl");
	this->max_scenery_y_material.set_shader(this->max_scenery_y_shader);
	this->max_scenery_y_material.set_point_size(4.0f);
	this->max_scenery_y_material.set_color(glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));

	this->tile_point_clouds.set_camera(&this->camera);
	this->tile_point_clouds_shader = principle::Material::load_shader_from_file(
		"point.vert.glsl", "untextured_model.frag.glsl");
	this->tile_point_clouds_material.set_shader(this->tile_point_clouds_shader);
	this->tile_point_clouds_material.set_point_size(6.0f);

	this->model_thumbnail = std::make_unique<principle::Thumbnail>(
		THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);
	this->selection_thumbnail = std::make_unique<principle::Thumbnail>(
		THUMBNAIL_WIDTH, THUMBNAIL_HEIGHT);

	make_sphere(this->sphere_mesh, this->sphere_vertex_buffer, this->sphere_index_buffer);

	auto& world_update_manager = get_game_manager().get_world_update_manager();
	world_update_manager.add_pass(&this->visible_tile_pass);
	world_update_manager.add_pass(&this->proxy_pass);

	get_game_manager().get_runtime_map().merge(get_game_manager().get_resource_manager().get_map());
}

zeitgeist::Application::~Application()
{
	delete this->models_shader;
	delete this->terrain_shader;
	delete this->min_terrain_y_shader;
	delete this->max_terrain_y_shader;
	delete this->min_scenery_y_shader;
	delete this->max_scenery_y_shader;
	delete this->tile_point_clouds_shader;

	clear_meshes();
}

void zeitgeist::Application::update(float delta)
{
	this->camera_controller.update(delta);
	this->camera.update(delta);

	this->thumbnail_time += delta;
}

void zeitgeist::Application::update_gui()
{
	principle::Application::update_gui();
	if (!this->thumbnail_was_visible)
	{
		this->thumbnail_time = 0.0f;
	}
}

void zeitgeist::Application::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	principle::Application::draw();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);

	this->camera.set_viewport_width(get_display()->get_width());
	this->camera.set_viewport_height(get_display()->get_height());

	if (this->draw_terrain)
	{
		this->terrain_scene.draw();
	}

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	if (this->draw_models)
	{
		if (this->draw_unclassified)
		{
			this->unclassified_material.set_shader(this->models_shader);
		}
		else
		{
			this->unclassified_material.set_shader(nullptr);
		}

		if (this->draw_scenery)
		{
			this->scenery_material.set_shader(this->models_shader);
		}
		else
		{
			this->scenery_material.set_shader(nullptr);
		}

		if (this->draw_props)
		{
			this->prop_material.set_shader(this->models_shader);
		}
		else
		{
			this->prop_material.set_shader(nullptr);
		}

		if (this->draw_actors)
		{
			this->actor_material.set_shader(this->models_shader);
		}
		else
		{
			this->actor_material.set_shader(nullptr);
		}

		this->models_scene.draw();
	}
	glDisable(GL_BLEND);

	glEnable(GL_BLEND);
	glDepthMask(GL_FALSE);
	glDepthFunc(GL_GEQUAL);
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE);
	this->terrain_shader->use();
	this->camera.apply(this->terrain_shader);

	auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(1.0f, 1.0f, 1.0f));
	auto translate = glm::translate(glm::mat4(1.0f), this->world_position);
	this->terrain_shader->set("principle_Model", scale * translate);
	sphere_mesh.draw();
	glDepthMask(GL_TRUE);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	auto& map = get_game_manager().get_runtime_map();
	for (auto tile_match: this->query_results_window.tile_matches)
	{
		if (!map.has_tile(tile_match))
		{
			continue;
		}

		glm::vec3 world_position;
		world_position.x = (tile_match.x + 0.5f) * thoreau::TILE_SIZE;
		world_position.y = map.get_tile(tile_match).get_max_elevation() + 512.0f;
		world_position.z = (tile_match.y + 0.5f) * thoreau::TILE_SIZE;

		auto scale = glm::scale(glm::mat4(1.0f), glm::vec3(0.5f, 0.5f, 0.5f));
		auto translate = glm::translate(glm::mat4(1.0f), world_position);

		this->terrain_shader->set("principle_Model", translate * scale);
		this->sphere_mesh.draw();
	}

	if (this->draw_min_terrain_y)
	{
		this->min_terrain_y.draw();
	}

	if (this->draw_max_terrain_y)
	{
		this->max_terrain_y.draw();
	}

	if (this->draw_min_scenery_y)
	{
		this->min_scenery_y.draw();
	}

	if (this->draw_max_scenery_y)
	{
		this->max_scenery_y.draw();
	}

	if (this->draw_tile_point_clouds)
	{
		this->tile_point_clouds.draw();
	}

	if (this->draw_selection)
	{
		this->selection_manager.draw(this->camera);
	}
}

void zeitgeist::Application::on_mouse_button_down(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_button_down(event);

	if (event.get_button() == 1)
	{
		auto display = get_display();
		auto& model_picker = get_game_manager().get_model_picker();
		auto& draw_manager = get_game_manager().get_draw_manager();

		auto view_matrix = this->camera.get_view_matrix();
		auto projection_matrix = this->camera.get_projection_matrix();
		auto viewport = glm::vec4(0, 0, display->get_width(), display->get_height());
		auto mouse_position = event.get_position();
		mouse_position.y = display->get_height() - mouse_position.y;

		auto results = model_picker.pick(view_matrix, projection_matrix, viewport, mouse_position);
		if (!results.empty())
		{
			for (auto i: results)
			{
				auto& draw = draw_manager.get(i);
				if (draw.get_draw_type() != principle::Draw::draw_type_terrain)
				{
					select(i);
					break;
				}
			}
		}
	}
}

void zeitgeist::Application::on_mouse_button_up(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_button_up(event);
}

void zeitgeist::Application::on_mouse_move(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_move(event);

	auto display = get_display();
	auto& model_picker = get_game_manager().get_model_picker();
	auto view_matrix = this->camera.get_view_matrix();
	auto projection_matrix = this->camera.get_projection_matrix();
	auto viewport = glm::vec4(0, 0, display->get_width(), display->get_height());
	auto mouse_position = event.get_position();
	mouse_position.y = display->get_height() - mouse_position.y;

	std::map<int, glm::vec3> positions;
	auto results = model_picker.pick(view_matrix, projection_matrix, viewport, mouse_position, positions);
	if (results.size() > 0)
	{
		this->world_position = positions[results[0]];
	}
}

void zeitgeist::Application::on_mouse_scroll(const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_scroll(event);
}

void zeitgeist::Application::on_key_down(const principle::KeyboardEvent& event)
{
	this->camera_controller.on_key_down(event);

	if (event.get_key_code() == ALLEGRO_KEY_M || event.get_key_code() == ALLEGRO_KEY_V)
	{
		auto& materials = get_game_manager().get_scenery_materials_state();
		auto& map = get_game_manager().get_runtime_map();

		auto tile_coordinate = thoreau::tile_from_world(this->world_position);
		if (map.try_get_tile_layer(tile_coordinate.x, tile_coordinate.y, this->world_position.y, tile_coordinate.layer) &&
			materials.has(tile_coordinate))
		{
			if (event.get_key_code() == ALLEGRO_KEY_V)
			{
				this->materials_set_window.open(materials.get(tile_coordinate));
			}
			else if (event.get_key_code() == ALLEGRO_KEY_M)
			{
				this->material_set_match_builder_window.add(materials.get(tile_coordinate));
			}
		}
	}
}

void zeitgeist::Application::on_key_up(const principle::KeyboardEvent& event)
{
	this->camera_controller.on_key_up(event);
}

void zeitgeist::Application::prepare_snapshot()
{
	this->models_scene.reset();
	this->terrain_scene.reset();
	this->min_terrain_y.reset();
	this->max_terrain_y.reset();
	this->min_scenery_y.reset();
	this->max_scenery_y.reset();
	this->tile_point_clouds.reset();
	clear_meshes();

	this->materials_set_window.close();
	this->actor_draw_list_window.clear();

	this->selection_manager.unselect_all();

	this->scene_bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
}

void zeitgeist::Application::finish_snapshot()
{
	auto& game = get_game_manager();

	auto min_terrain_y_node = this->min_terrain_y.add_node<principle::MinTileElevationsNode>(
		&game.get_terrain_elevation_state());
	min_terrain_y_node->set_material(&this->min_terrain_y_material);

	auto max_terrain_y_node = this->max_terrain_y.add_node<principle::MaxTileElevationsNode>(
		&game.get_terrain_elevation_state());
	max_terrain_y_node->set_material(&this->max_terrain_y_material);

	for (auto& i: game.get_static_point_cloud_state())
	{
		float x, z;
		{
			x = i.first.x * thoreau::TILE_SIZE;
			z = i.first.y * thoreau::TILE_SIZE;
		}

		float y;
		{
			thoreau::TileCoordinate tile_without_elevation(i.first.x, i.first.y);
			if (!game.get_terrain_elevation_state().has(i.first))
			{
				continue;
			}

			auto& elevation = game.get_terrain_elevation_state().get(
				tile_without_elevation);
			y = elevation.get_min(i.first.layer);
		}

		auto node = this->tile_point_clouds.add_node<principle::TilePointCloudNode>(
			i.second);
		node->set_model_matrix(
			glm::translate(glm::mat4(1.0f), glm::vec3(x, y, z)));
		node->set_material(&this->tile_point_clouds_material);
	}

	auto& draw_manager = game.get_draw_manager();
	auto& model_manager = game.get_model_manager();
	auto& texture_manager = game.get_texture_resource_manager();
	auto& terrain_manager = game.get_terrain_manager();
	for (auto& i: draw_manager)
	{
		auto& tag = i.first;
		auto& draw = i.second;
		if (model_manager.has(tag))
		{
			if (model_manager.tag_has_vertex_index_mapper(tag))
			{
				add_model(
					draw,
					model_manager.get(tag),
					texture_manager.get_texture(draw.get_event().get_atlas_name()),
					model_manager.get_tag_vertex_index_mapper(tag));
			}
			else
			{
				add_model(
					draw,
					model_manager.get(tag),
					texture_manager.get_texture(draw.get_event().get_atlas_name()));
			}
		}
		else if (terrain_manager.has(tag))
		{
			add_terrain(draw, terrain_manager.get(tag), texture_manager.get_texture(draw.get_event().get_atlas_name()));
		}
	}

	this->camera.set_target(
		this->scene_bounds.compute_center());

	auto& minimap = get_game_manager().get_minimap();
	if (minimap.get_is_visible())
	{
		auto sector_coordinate = thoreau::sector_from_tile(minimap.get_center_tile());
		sector_coordinate.layer = this->export_tile_layer;

		export_tile_snapshot(sector_coordinate);
	}
	else
	{
		std::set<thoreau::SectorCoordinate> visible_sectors;
		if (this->export_tile_snapshots)
		{
			for (auto& i: this->visible_tile_state)
			{
				visible_sectors.insert(thoreau::sector_from_tile(i.first));
			}
		}

		for (auto i: visible_sectors)
		{
			export_tile_snapshot(i);
		}
	}
}

void zeitgeist::Application::show_model_thumbnail(
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper)
{
	kompvter::DrawParameters draw_parameters(
		kompvter::DrawParameters::primitive_triangles,
		0, index_mapper.get_num_indices());
	show_model_thumbnail(model, index_mapper, draw_parameters);
}

void zeitgeist::Application::show_model_thumbnail(
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters)
{
	if (this->model_thumbnail_current_model != model ||
		this->model_thumbnail_current_index_mapper_buffer != index_mapper.get_buffer().get_name() ||
		this->model_thumbnail_current_index_offset_start != draw_parameters.get_offset_start() ||
		this->model_thumbnail_current_index_offset_stop != draw_parameters.get_offset_stop())
	{
		this->model_thumbnail_current_model = model;
		this->model_thumbnail_current_index_mapper_buffer = index_mapper.get_buffer().get_name();
		this->model_thumbnail_current_index_offset_start = draw_parameters.get_offset_start();
		this->model_thumbnail_current_index_offset_stop = draw_parameters.get_offset_stop();
		this->model_thumbnail_current_tag = 0;

		this->model_thumbnail->clear();
		this->model_thumbnail->add_model(
			model, glm::mat4(1.0f), index_mapper, draw_parameters);
	}

	show_thumbnail(this->model_thumbnail.get());
}

void zeitgeist::Application::show_model_thumbnail(int tag)
{
	if (this->model_thumbnail_current_tag != tag)
	{
		this->model_thumbnail_current_tag = tag;
		this->model_thumbnail_current_model = nullptr;

		auto& game = get_game_manager();
		auto& draw_manager = game.get_draw_manager();
		auto& model_manager = game.get_model_manager();

		auto& draw = draw_manager.get(tag);
		auto model = model_manager.get(tag);
		auto& index_mapper = model_manager.get_tag_vertex_index_mapper(tag);

		this->model_thumbnail->clear();
		this->model_thumbnail->add_model(
			model,
			draw.get_model_matrix(),
			index_mapper,
			draw.get_draw_parameters());
	}

	show_thumbnail(this->model_thumbnail.get());
}

void zeitgeist::Application::show_selection_thumbnail()
{
	show_thumbnail(this->selection_thumbnail.get());
}

void zeitgeist::Application::show_thumbnail(principle::Thumbnail* thumbnail)
{
	ImGui::BeginTooltip();
	{
		auto model_matrix = compute_thumbnail_rotation_matrix();
		auto image = thumbnail->draw(model_matrix);
		ImGui::Image(
			image,
			ImVec2(image->get_width(), image->get_height()));
	}
	ImGui::EndTooltip();

	this->thumbnail_was_visible = true;
}

glm::mat4 zeitgeist::Application::compute_thumbnail_rotation_matrix() const
{
	return glm::rotate(
		glm::mat4(1.0f),
		(float)(this->thumbnail_time * M_PI / 2.0),
		glm::vec3(0.0f, 1.0f, 0.0f));
}

void zeitgeist::Application::clear_meshes()
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

kvncer::Mesh* zeitgeist::Application::create_mesh_for_model(
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

kvncer::Mesh* zeitgeist::Application::create_mesh_for_terrain(
	const principle::Draw& draw)
{
	auto mesh = new kvncer::Mesh();
	auto vertex_buffer = new kvncer::MeshBuffer();
	this->meshes.push_back(mesh);
	this->mesh_buffers.push_back(vertex_buffer);

	principle::generate_untextured_terrain_vertex_data(
		draw.get_event(), *mesh, *vertex_buffer);

	return mesh;
}

void zeitgeist::Application::add_model(
	const principle::Draw& draw,
	const kvre::Model* model,
	kvncer::Texture* texture,
	const kompvter::VertexIndexMapper& index_mapper)
{
	/// XXX: Hack to prevent the skybox from rendering.
	if (glm::ivec4(model->get_color(0) * 255.0f) == glm::ivec4(90, 236, 47, 255))
	{
		return;
	}

	auto mesh = create_mesh_for_model(model, index_mapper);
	auto model_node = this->models_scene.add_node<principle::ModelNode>(
		mesh,
		kvncer::gl::marshal_index_mapper_format_from_kompvter(index_mapper.get_format()),
		draw.get_draw_parameters().get_offset_start(),
		draw.get_draw_parameters().get_offset_stop());
	{
		model_node->set_model_matrix(draw.get_model_matrix());

		if (draw.get_draw_type() == principle::Draw::draw_type_actor)
		{
			draw.copy_bone_transforms(model_node);
		}

		switch (draw.get_draw_type())
		{
			case principle::Draw::draw_type_unclassified:
				model_node->set_material(&this->unclassified_material);
				break;
			case principle::Draw::draw_type_scenery:
				model_node->set_material(&this->scenery_material);
				break;
			case principle::Draw::draw_type_prop:
				model_node->set_material(&this->prop_material);
				break;
			case principle::Draw::draw_type_actor:
				model_node->set_material(&this->actor_material);
				break;
		}
		model_node->set_texture(texture);
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

	this->scene_bounds.expand(bounds);
}

void zeitgeist::Application::add_terrain(
	const principle::Draw& draw,
	const kvre::Terrain* terrain,
	kvncer::Texture* texture)
{
	auto mesh = create_mesh_for_terrain(draw);
	auto terrain_node = this->terrain_scene.add_node<principle::ModelNode>(
		mesh,
		GL_NONE,
		draw.get_draw_parameters().get_offset_start(),
		draw.get_draw_parameters().get_offset_stop());
	{
		terrain_node->set_model_matrix(draw.get_model_matrix());
		terrain_node->set_material(&this->terrain_material);
		terrain_node->set_texture(texture);
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
}

principle::ModelSelection* zeitgeist::Application::select(int tag)
{
	auto& draw_manager = get_game_manager().get_draw_manager();
	auto& model_manager = get_game_manager().get_model_manager();

	if (draw_manager.has(tag) && model_manager.has(tag))
	{
		auto model = model_manager.get(tag);
		auto& draw = draw_manager.get(tag);

		principle::ModelSelection* selection;
		if (model_manager.tag_has_vertex_index_mapper(draw.get_tag()))
		{
			selection = this->selection_manager.select(
				model,
				model_manager.get_tag_vertex_index_mapper(draw.get_tag()));
		}
		else
		{
			selection = this->selection_manager.select(
				model, kompvter::VertexIndexMapper());
		}

		selection->set_index_offset_start(draw.get_draw_parameters().get_offset_start());
		selection->set_index_offset_stop(draw.get_draw_parameters().get_offset_stop());
		selection->set_model_matrix(draw.get_model_matrix());
		selection->set_tag(draw.get_tag());

		this->selection_window.open();

		return selection;
	}

	return nullptr;
}

void zeitgeist::Application::export_tile_snapshot(
	const thoreau::SectorCoordinate& sector_coordinate)
{
	const int TILE_WIDTH = 64;
	const int TILE_HEIGHT = 64;
	const int TEXTURE_WIDTH = TILE_WIDTH * thoreau::SECTOR_SIZE;
	const int TEXTURE_HEIGHT = TILE_HEIGHT * thoreau::SECTOR_SIZE;
	kvncer::Texture color_attachment(
		kvncer::Texture::component_format_rgba,
		false,
		TEXTURE_WIDTH, TEXTURE_HEIGHT);
	kvncer::Framebuffer framebuffer(&color_attachment, true);

	framebuffer.bind();

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glDisable(GL_CULL_FACE);
	glViewport(0, 0, TEXTURE_WIDTH, TEXTURE_HEIGHT);
	glClearColor(0, 0, 0, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	principle::StaticCamera camera;
	glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), (float)M_PI / 2.0f, glm::vec3(-1, 0, 0));
	camera.set_view_matrix(rotation);

	glm::mat4 projection = glm::ortho<float>(
		(sector_coordinate.x * thoreau::SECTOR_SIZE) * thoreau::TILE_SIZE,
		(sector_coordinate.x * thoreau::SECTOR_SIZE + thoreau::SECTOR_SIZE) * thoreau::TILE_SIZE,
		(sector_coordinate.y * thoreau::SECTOR_SIZE) * thoreau::TILE_SIZE,
		(sector_coordinate.y * thoreau::SECTOR_SIZE + thoreau::SECTOR_SIZE) * thoreau::TILE_SIZE,
		std::numeric_limits<std::int16_t>::max(),
		std::numeric_limits<std::int16_t>::min());
	camera.set_projection_matrix(projection);
	{
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glDisable(GL_CULL_FACE);

		this->terrain_scene.draw(&camera);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		this->unclassified_material.set_shader(this->models_shader);
		this->scenery_material.set_shader(this->models_shader);
		this->prop_material.set_shader(this->models_shader);
		this->actor_material.set_shader(nullptr);

		this->models_scene.draw(&camera);
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
		std::snprintf(filename, sizeof(filename), "%03dx%03dx%02d", 
			sector_coordinate.x, 
			sector_coordinate.y, 
			sector_coordinate.layer);
		path += filename;
	}

	const std::size_t SECTOR_LENGTH = thoreau::SECTOR_SIZE * thoreau::SECTOR_SIZE;
	principle::ImageStore<float, SECTOR_LENGTH, TILE_WIDTH, TILE_HEIGHT> depth_store;
	if (boost::filesystem::exists(path + "-depth.bin"))
	{
		depth_store.read(path + "-depth.bin");
	}
	else
	{
		depth_store.clear(1.0f);
		depth_store.write(path + "-depth.bin");
	}

	principle::ImageStore<glm::u8vec4, SECTOR_LENGTH, TILE_WIDTH, TILE_HEIGHT> image_store;
	if (!boost::filesystem::exists(path + "-color.bin"))
	{
		image_store.clear(glm::u8vec4(0));
		image_store.write(path + "-color.bin");
	}

	std::unique_ptr<float[]> current_depth_values = std::make_unique<float[]>(TILE_WIDTH * TILE_HEIGHT);
	std::size_t current_index = 0;
	for (int j = 0; j < thoreau::SECTOR_SIZE; ++j)
	{
		for (int i = 0; i < thoreau::SECTOR_SIZE; ++i)
		{
			std::size_t index = current_index++;
			thoreau::TileCoordinate tile_coordinate = thoreau::tile_from_sector(sector_coordinate);
			tile_coordinate.x += i;
			tile_coordinate.y += j;

			thoreau::TileCoordinate layer_zero_tile_coordinate = tile_coordinate;
			layer_zero_tile_coordinate.layer = 0;

			if (!this->visible_tile_state.has(tile_coordinate) &&
				!this->visible_tile_state.has(layer_zero_tile_coordinate))
			{
				//continue;
			}

			glReadPixels(
				i * TILE_WIDTH, j * TILE_HEIGHT,
				TILE_WIDTH, TILE_HEIGHT,
				GL_DEPTH_COMPONENT, GL_FLOAT,
				current_depth_values.get());
			{
				auto a = current_depth_values.get();
				auto b = depth_store.get(index);
				int less = 0;
				for (int j = 0; j < TILE_HEIGHT; ++j)
				{
					for (int i = 0; i < TILE_WIDTH; ++i)
					{
						if (*a < *b)
						{
							++less;
						}

						++a;
						++b;
					}
				}

				if (less < 32)
				{
					continue;
				}

				std::memcpy(
					depth_store.get(index),
					current_depth_values.get(),
					TILE_WIDTH * TILE_HEIGHT * sizeof(float));
				depth_store.write(path + "-depth.bin", index);

				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, color_attachment.get_texture());
				glReadPixels(
					i * TILE_WIDTH, j * TILE_HEIGHT,
					TILE_WIDTH, TILE_HEIGHT,
					GL_RGBA, GL_UNSIGNED_BYTE,
					&image_store.get(index)->x);
				image_store.write(path + "-color.bin", index);
			}
		}
	}
}

zeitgeist::Application::ProxyPass::ProxyPass(Application* application)
{
	this->application = application;
}

void zeitgeist::Application::ProxyPass::start()
{
	this->application->prepare_snapshot();
}

void zeitgeist::Application::ProxyPass::stop()
{
	this->application->finish_snapshot();
	this->application->visible_tile_state.clear();
	this->application->animation_builder_window.on_frame();
}
