// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "acropolis/application.hpp"
#include "thoreau/thoreau.hpp"

acropolis::Application::SceneView::SceneView(Application* application) :
	camera_controller(&camera)
{
	this->application = application;
	make_box();
}

void acropolis::Application::SceneView::focus(
	const thoreau::TileCoordinate& tile_coordinate)
{
	this->box_visible = false;
	float elevation = this->application->scene_bounds.compute_center().y;

	auto& map = this->application->get_game_manager().get_runtime_map();
	if (map.has_tile(tile_coordinate))
	{
		auto& tile = map.get_tile(tile_coordinate);
		if (tile.has_elevation())
		{
			elevation = tile.get_elevation();
			this->box_visible = true;
		}
	}

	glm::vec3 world_position;
	world_position.x = (tile_coordinate.x + 0.5f) * thoreau::TILE_SIZE;
	world_position.y = elevation;
	world_position.z = (tile_coordinate.y + 0.5f) * thoreau::TILE_SIZE;

	this->camera.focus(world_position + glm::vec3(0.0f, 512.0f, 0.0f));
	this->box_position = world_position;
}

void acropolis::Application::SceneView::update(float delta)
{
	if (this->application->is_camera_focus_dirty)
	{
		auto target = this->application->scene_bounds.compute_center();

		this->camera.set_target(target);
		this->light_position = target;
	}

	this->camera_controller.update(delta);
	this->camera.update(delta);
}

void acropolis::Application::SceneView::draw()
{
	apply_viewport();
	glClearColor(0.30f, 0.40f, 0.46f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	this->camera.set_viewport_width(get_size().x);
	this->camera.set_viewport_height(get_size().y);

	this->application->scene_shader->use();
	this->application->scene_shader->set("principle_LightPosition", this->light_position);
	this->application->scene_shader->set("principle_LightColor", this->light_color);
	this->application->scene_shader->set("principle_LightIntensity", this->light_intensity);
	this->application->scene_shader->set("principle_Ambience", this->light_ambience);

	glEnable(GL_CULL_FACE);
	glFrontFace(GL_CW);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	if (this->application->draw_terrain_scene)
	{
		this->application->terrain_scene.draw(&this->camera);
	}

	if (this->application->draw_models_scene)
	{
		this->application->models_scene.draw(&this->camera);
	}

	if (this->application->show_tiles == show_focus)
	{
		if (this->box_visible)
		{
			draw_box();
		}
	}
	else if (this->application->show_tiles == show_impassable_tiles)
	{
		auto& map = this->application->get_game_manager().get_runtime_map();
		for (auto tile_coordinate: this->application->impassable_tiles)
		{
			bool is_visible = false;
			float elevation;
			if (map.has_tile(tile_coordinate))
			{
				auto& tile = map.get_tile(tile_coordinate);
				if (tile.has_elevation())
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

	glFrontFace(GL_CCW);
}

void acropolis::Application::SceneView::on_mouse_button_down(
	const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_button_down(event);
}

void acropolis::Application::SceneView::on_mouse_button_up(
	const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_button_up(event);
}

void acropolis::Application::SceneView::on_mouse_move(
	const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_move(event);
}

void acropolis::Application::SceneView::on_mouse_scroll(
	const principle::MouseEvent& event)
{
	this->camera_controller.on_mouse_scroll(event);
}

void acropolis::Application::SceneView::on_key_down(
	const principle::KeyboardEvent& event)
{
	this->camera_controller.on_key_down(event);
}

void acropolis::Application::SceneView::on_key_up(
	const principle::KeyboardEvent& event)
{
	this->camera_controller.on_key_up(event);
}

void acropolis::Application::SceneView::make_box()
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
	this->box_vertex_buffer.set_buffer(vertex_buffer_data);

	kompvter::Buffer index_buffer_data;
	index_buffer_data.allocate(sizeof(indices));
	index_buffer_data.update(0, (const std::uint8_t*)indices, sizeof(indices));
	this->box_index_buffer.set_buffer(index_buffer_data);

	box_mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position,
		&this->box_vertex_buffer,
		3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position), sizeof(Vertex));
	box_mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_color,
		&this->box_vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color), sizeof(Vertex));
	box_mesh.set_indices(
		&this->box_index_buffer,
		GL_UNSIGNED_INT, 0, sizeof(indices));
}

void acropolis::Application::SceneView::draw_box(const glm::vec3& world_position)
{
	auto model_matrix = glm::translate(glm::mat4(1.0f), world_position);
	auto shader = this->application->scene_material.get_shader();

	shader->use();
	shader->set("principle_Model", model_matrix);
	shader->set("principle_View", this->camera.get_view_matrix());
	shader->set("principle_Projection", this->camera.get_projection_matrix());

	this->box_mesh.draw(GL_LINES);
}

void acropolis::Application::SceneView::draw_box()
{
	draw_box(this->box_position);
}
