// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include "continent1/heuristics.hpp"
#include "acropolis/application.hpp"
#include "principle/graphics/utility.hpp"

acropolis::Application::Application() :
	principle::Application("acropolis"),
	acropolis_window(this),
	candidate_query_window(this),
	passable_tile_question_window(this),
	view_manager(get_display(), nullptr),
	visible_tile_pass(&get_game_manager().get_transform_state(), &get_game_manager().get_terrain_elevation_state(), &visible_tile_state),
	candidate_manager(&batched_model_manager, &get_game_manager().get_runtime_map())
{
	this->scene_shader = principle::Material::load_shader_from_file(
		"model.vert.glsl", "untextured_lit_model.frag.glsl");
	this->scene_material.set_shader(this->scene_shader);

	get_game_manager().get_world_update_manager().add_pass(
		&this->visible_tile_pass);
	get_game_manager().get_world_update_manager().add_pass(
		&this->batched_model_manager);
	get_game_manager().get_world_update_manager().add_pass(
		&this->candidate_manager);

	this->scene_view = new SceneView(this);
	this->tile_view = new TileView(this);
	add_view(scene_view);
	add_view(tile_view);

	this->view_manager.enable_vertical_divider();
	this->view_manager.set_vertical_divider(2.0f / 3.0f);
}

void acropolis::Application::update(float delta)
{
	this->view_manager.update(delta);
	this->is_camera_focus_dirty = false;
}

void acropolis::Application::draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glDisable(GL_SCISSOR_TEST);
	glViewport(0, 0, get_display()->get_width(), get_display()->get_height());
	glClearColor(0.2f, 0.2f, 0.2f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->view_manager.draw();
}

void acropolis::Application::on_mouse_button_down(
	const principle::MouseEvent& event)
{
	this->view_manager.on_mouse_button_down(event);
}

void acropolis::Application::on_mouse_button_up(
	const principle::MouseEvent& event)
{
	this->view_manager.on_mouse_button_up(event);
}

void acropolis::Application::on_mouse_move(
	const principle::MouseEvent& event)
{
	this->view_manager.on_mouse_move(event);
}

void acropolis::Application::on_mouse_scroll(
	const principle::MouseEvent& event)
{
	this->view_manager.on_mouse_scroll(event);
}

void acropolis::Application::on_key_down(
	const principle::KeyboardEvent& event)
{
	this->view_manager.on_key_down(event);
}

void acropolis::Application::on_key_up(
	const principle::KeyboardEvent& event)
{
	this->view_manager.on_key_up(event);
}

void acropolis::Application::on_start_trace()
{
	this->models_scene.reset();
	this->terrain_scene.reset();
	clear_meshes();

	this->candidate_manager.reset();

	this->passable_tiles.clear();
	this->impassable_tiles.clear();
	this->passable_tile_question_window.close();

	this->is_camera_focus_dirty = true;
	this->scene_bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
}

void acropolis::Application::on_finish_trace()
{
	auto& game = get_game_manager();
	auto& draw_manager = game.get_draw_manager();
	auto& model_manager = game.get_model_manager();
	auto& terrain_manager = game.get_terrain_manager();
	for (auto& draw: draw_manager)
	{
		if (model_manager.has(draw.first))
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

	std::string base_filename = get_trace_filename();
	base_filename += ".";
	base_filename += std::to_string(get_current_frame());
	base_filename += ".";

	this->candidate_manager.save_candidate_properties(base_filename + "candidates.csv");
	this->candidate_manager.save_tile_properties(base_filename + "tiles.csv");
}

bool acropolis::Application::add_view(principle::View* view)
{
	for (int i = 0; i < principle::ViewManager::MAX_VIEWS; ++i)
	{
		if (!this->view_manager.has_view(i))
		{
			add_view(view, i);
			return true;
		}
	}

	delete view;
	return false;
}

void acropolis::Application::add_view(
	principle::View* view, int index)
{
	auto previous_view = this->view_manager.get_view(index);
	if (previous_view != nullptr)
	{
		auto iter = this->views.find(previous_view);
		if (iter != this->views.end())
		{
			delete iter->first;
			this->views.erase(iter);
		}
	}

	this->view_manager.set_view(index, view);
	this->views.emplace(view, index);
}

void acropolis::Application::remove_view(principle::View* view)
{
	auto iter = this->views.find(view);
	assert(iter != this->views.end());
	assert(this->view_manager.get_view(iter->second) == view);

	this->view_manager.unset_view(iter->second);
	delete view;
}

void acropolis::Application::clear_meshes()
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

kvncer::Mesh* acropolis::Application::create_mesh_for_model(
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

kvncer::Mesh* acropolis::Application::create_mesh_for_terrain(
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

void acropolis::Application::add_model(
	const principle::Draw& draw,
	const kvre::Model* model,
	const kompvter::VertexIndexMapper& index_mapper)
{
	auto mesh = create_mesh_for_model(model, index_mapper);
	auto model_node = this->models_scene.add_node<principle::ModelNode>(
		mesh,
		kvncer::gl::marshal_index_mapper_format_from_kompvter(index_mapper.get_format()),
		draw.get_draw_parameters().get_offset_start(),
		draw.get_draw_parameters().get_offset_stop());
	{
		model_node->set_model_matrix(draw.get_model_matrix());
		draw.copy_bone_transforms(model_node);

		model_node->set_material(&this->scene_material);
	}

	auto& draw_parameters = draw.get_draw_parameters();
	auto begin = kvre::IndexedVertexIterator::begin(index_mapper, draw_parameters);
	auto end = kvre::IndexedVertexIterator::end(index_mapper, draw_parameters);
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

void acropolis::Application::add_terrain(
	const principle::Draw& draw,
	const kvre::Terrain* terrain)
{
	auto mesh = create_mesh_for_terrain(terrain);
	auto terrain_node = this->terrain_scene.add_node<principle::ModelNode>(
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
}

void acropolis::Application::save_tiles(const std::string& filename)
{
	std::ofstream stream(filename);

	static const std::string SEP = ", ";
	stream << "tile_x" << SEP << "tile_y" << SEP << "tile_layer" << SEP << "passable" << std::endl;

	for (auto& tile: this->passable_tiles)
	{
		stream << tile.x << SEP << tile.y << SEP << tile.layer << SEP << "TRUE" << std::endl;
	}

	for (auto& tile: this->impassable_tiles)
	{
		stream << tile.x << SEP << tile.y << SEP << tile.layer << SEP << "FALSE" << std::endl;
	}
}

void acropolis::Application::apply_tile_heuristic()
{
	auto& map = get_game_manager().get_runtime_map();
	for (auto& i: this->visible_tile_state)
	{
		bool is_impassable = false;

		if (continent1::heuristics::is_tile_impassable(i.first, this->candidate_manager))
		{
			is_impassable = true;
		}
		else
		{
			if (map.has_tile(i.first))
			{
				auto& tile = map.get_tile(i.first);
				is_impassable = tile.get_flags() & thoreau::Tile::flag_impassable;
			}
		}

		if (is_impassable)
		{
			this->impassable_tiles.insert(i.first);
		}
		else
		{
			this->passable_tiles.insert(i.first);
		}
	}
}
