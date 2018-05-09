// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "chomsky/chomsky.hpp"
#include "chomsky/world/worldState.hpp"
#include "kompvter/events/itemIconEvent.hpp"
#include "kvncer/graphics/framebuffer.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "principle/graphics/utility.hpp"
#include "principle/resources/itemIconManager.hpp"
#include "principle/scene/scene.hpp"
#include "principle/scene/modelMaterial.hpp"
#include "principle/scene/modelNode.hpp"
#include "principle/scene/staticCamera.hpp"
#include "principle/scene/material.hpp"
#include "thoreau/thoreau.hpp"

principle::ItemIconManager::ItemIconManager(TextureManager& texture_resource_manager)
{
	this->texture_resource_manager = &texture_resource_manager;

	this->draw_model_shader = Material::load_shader_from_file(
		"model.vert.glsl", "untextured_model.frag.glsl");

	this->draw_gui_shader = Material::load_shader_from_file(
		"gui.vert.glsl", "gui.frag.glsl");
	this->draw_gui_shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);
	this->draw_gui_shader->bind_input("vert_Texture", kvncer::Shader::vertex_input_texture_coordinate);
	this->draw_gui_shader->bind_input("vert_TextureMeta", kvncer::Shader::vertex_input_texture_meta);
	this->draw_gui_shader->bind_input("vert_Color", kvncer::Shader::vertex_input_color);

	this->copy_shader = Material::load_shader_from_file(
		"copy.vert.glsl", "copy.frag.glsl");
	this->copy_shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);
	this->copy_shader->bind_input("vert_Texture", kvncer::Shader::vertex_input_texture_coordinate);
}

principle::ItemIconManager::~ItemIconManager()
{
	delete this->draw_model_shader;
	delete this->draw_gui_shader;
}

std::size_t principle::ItemIconManager::count() const
{
	return this->item_icons.size();
}

const principle::ItemIcon& principle::ItemIconManager::at(
	std::size_t index) const
{
	return this->item_icons.at(index);
}

void principle::ItemIconManager::reset()
{
	this->item_icons.clear();

	this->current_transfers.clear();
	this->current_model_draws.clear();
	this->current_gui_draws.clear();
}

void principle::ItemIconManager::on_draw_model(
	const kompvter::ModelDrawEvent& event)
{
	auto destination_texture = this->texture_resource_manager->get_texture(event.get_color_attachment_name());
	if (destination_texture == nullptr)
	{
		return;
	}

	kvncer::Framebuffer framebuffer(destination_texture, true);

	framebuffer.bind();
	glViewport(
		0, 0,
		destination_texture->get_width(), destination_texture->get_height());

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	draw_model(event);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->current_model_draws.emplace(event.get_color_attachment_name(), event);
}

void principle::ItemIconManager::on_draw_gui(
	const kompvter::GUIDrawEvent& event)
{
	auto destination_texture = this->texture_resource_manager->get_texture(event.get_color_attachment_name());
	if (destination_texture == nullptr)
	{
		return;
	}

	kvncer::Framebuffer framebuffer(destination_texture, false);

	framebuffer.bind();
	glViewport(
		0, 0,
		destination_texture->get_width(), destination_texture->get_height());

	glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);

	draw_gui(event);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	this->current_gui_draws.emplace(event.get_color_attachment_name(), event);
}

void principle::ItemIconManager::on_transfer(
	const kompvter::ItemIconEvent& event)
{
	transfer(
		event.get_source_texture_name(), 0, 0,
		event.get_destination_texture_name(), 0, 0,
		chomsky::config::ITEM_ICON_WIDTH, chomsky::config::ITEM_ICON_HEIGHT);
}

void principle::ItemIconManager::on_clear(const kompvter::ItemIconEvent& event)
{
	this->current_model_draws.erase(event.get_texture_name());
	this->current_gui_draws.erase(event.get_texture_name());
	this->current_transfers[event.get_texture_name()] = 0;

	auto texture = this->texture_resource_manager->get_texture(event.get_texture_name());
	if (texture != nullptr)
	{
		kvncer::Framebuffer framebuffer(texture, false);
		glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
		glClear(GL_COLOR_BUFFER_BIT);
	}
}

void principle::ItemIconManager::on_texture_copy(
	const kompvter::TextureEvent& event)
{
	finalize(event.get_source_name());
}

void principle::ItemIconManager::transfer(
	int source_name,
	int source_x, int source_y,
	int destination_name,
	int destination_x, int destination_y,
	int width, int height)
{
	auto source_texture = this->texture_resource_manager->get_texture(source_name);
	auto destination_texture = this->texture_resource_manager->get_texture(destination_name);
	if (source_texture == nullptr || destination_texture == nullptr)
	{
		return;
	}

	finalize(source_name);
	draw_transfer(
		source_name, source_texture, source_x, source_y,
		destination_name, destination_texture, destination_x, destination_y,
		width, height);
}

void principle::ItemIconManager::finalize(int texture_name)
{
	ItemIcon item_icon;

	auto model_draws = this->current_model_draws.equal_range(texture_name);
	if (model_draws.first == model_draws.second)
	{
		return;
	}

	typedef chomsky::WorldState<
		thoreau::TileCoordinate,
		chomsky::TileSpacePointCloud>
	WorkingPointCloudMatch;
	WorkingPointCloudMatch point_cloud_match;

	chomsky::ModelVertexCloud cloud;
	for (auto i = model_draws.first; i != model_draws.second; ++i)
	{
		auto begin = kvre::IndexedVertexIterator::begin(
			i->second.get_vertex_index_mapper(),
			i->second.get_draw_parameters());
		auto end = kvre::IndexedVertexIterator::end(
			i->second.get_vertex_index_mapper(),
			i->second.get_draw_parameters());
		kvre::Model model(i->second.get_vertex_attributes());
		for (auto j = begin; j != end; ++j)
		{
			auto position = model.get_position(*j);
			auto color = glm::vec3(model.get_color(*j));

			chomsky::ModelVertexCloud::Vertex vertex;
			{
				vertex.color = glm::vec3(model.get_color(*j));

				glm::vec4 transformed_position(position, 0.0f);
				transformed_position = i->second.get_view_matrix() * transformed_position;
				vertex.position = glm::vec3(transformed_position);
			}
			cloud.add(vertex);

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

			auto& tile_point_cloud = point_cloud_match.add(tile_coordinate);
			tile_point_cloud.add(tile_relative_position, color);
		}

		item_icon.draws.push_back(i->second);
	}

	std::size_t n = 0;
	for (auto& i: point_cloud_match)
	{
		n += i.second.count();
		item_icon.point_cloud_match.set(i.first, i.second);
	}

	item_icon.silhouette.project(cloud);

	auto texture = this->texture_resource_manager->get_texture(texture_name);
	if (texture != nullptr)
	{
		item_icon.texture = std::make_unique<kvncer::Texture>(
			kompvter::TextureEvent::component_format_rgba, false,
			texture->get_width(), texture->get_height());
		draw_transfer(
			texture_name, texture, 0, 0,
			0, item_icon.texture.get(), 0, 0,
			texture->get_width(), texture->get_height());
	}

	this->item_icons.push_back(std::move(item_icon));
}

void principle::ItemIconManager::draw_model(
	const kompvter::ModelDrawEvent& event)
{
	kvre::Model model(event.get_vertex_attributes());

	kvncer::Mesh mesh;
	kvncer::MeshBuffer vertex_buffer, index_buffer;
	generate_untextured_model_vertex_data(&model, mesh, vertex_buffer);
	generate_index_data(
		event.get_vertex_index_mapper(), event.get_draw_parameters(),
		mesh, index_buffer);

	StaticCamera camera;
	camera.set_view_matrix(event.get_view_matrix());
	camera.set_projection_matrix(event.get_projection_matrix());

	Scene scene;
	scene.set_camera(&camera);

	ModelMaterial material;
	material.set_shader(this->draw_model_shader);

	auto model_node = scene.add_node<ModelNode>(&mesh);
	model_node->set_model_matrix(event.get_model_matrix());
	model_node->set_material(&material);

	scene.draw();
}

void principle::ItemIconManager::draw_gui(const kompvter::GUIDrawEvent& event)
{
	kvre::FlatDrawBuffer flat_draw_buffer(event);

	kvncer::Mesh mesh;
	kvncer::MeshBuffer vertex_buffer, index_buffer;
	generate_gui_mesh(&flat_draw_buffer, mesh, vertex_buffer);
	generate_index_data(
		event.get_vertex_index_mapper(), event.get_draw_parameters(),
		mesh, index_buffer);

	this->draw_gui_shader->use();
	this->draw_gui_shader->set("principle_Texture", 1);
	auto projection = glm::ortho(
		0.0f, (float)chomsky::config::ITEM_ICON_WIDTH,
		0.0f, (float)chomsky::config::ITEM_ICON_HEIGHT,
		-1.0f, 1.0f);
	this->draw_gui_shader->set("principle_Projection", projection);

	auto texture = this->texture_resource_manager->get_texture(
		event.get_atlas_name());
	if (texture == nullptr)
	{
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, 0);
	}
	else
	{
		texture->bind(0);
	}

	mesh.draw();
}

void principle::ItemIconManager::draw_transfer(
	int source_name,
	kvncer::Texture* source_texture,
	int source_x, int source_y,
	int destination_name,
	kvncer::Texture* destination_texture,
	int destination_x, int destination_y,
	int width, int height)
{
	float texture_left = source_x;
	texture_left /= source_texture->get_width();
	float texture_right = source_x + width;
	texture_right /= source_texture->get_width();
	float texture_bottom = source_y;
	texture_bottom /= source_texture->get_height();
	float texture_top = source_y + height;
	texture_top /= source_texture->get_height();

	struct Vertex
	{
		glm::vec2 position;
		glm::vec2 texture_coordinate;
	};
	Vertex vertices[] = {
		{ { -1.0f, -1.0f },  { texture_left, texture_bottom } },
		{ { -1.0f, 1.0f },  { texture_left, texture_top } },
		{ { 1.0f, 1.0f },  { texture_right, texture_top } },
		{ { -1.0f, -1.0f },  { texture_left, texture_bottom } },
		{ { 1.0f, 1.0f },  { texture_right, texture_top } },
		{ { 1.0f, -1.0f },  { texture_right, texture_bottom } }
	};

	kvncer::Mesh mesh;
	kvncer::MeshBuffer vertex_buffer;
	{
		kompvter::Buffer vertex_buffer_data;
		vertex_buffer_data.allocate(sizeof(vertices));
		vertex_buffer_data.update(0, (const std::uint8_t*)vertices, sizeof(vertices));
		vertex_buffer.set_buffer(vertex_buffer_data);
	}

	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position, &vertex_buffer,
		2, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_texture_coordinate, &vertex_buffer,
		2, GL_FLOAT, GL_FALSE,
		offsetof(Vertex, texture_coordinate), sizeof(Vertex));

	kvncer::Framebuffer framebuffer(destination_texture, false);
	glViewport(destination_x, destination_y, width, height);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);

	if (destination_name == 0 || this->current_transfers[destination_name] == 0)
	{
		glDisable(GL_BLEND);
	}
	else
	{
		glEnable(GL_BLEND);
	}

	if (destination_name != 0)
	{
		++this->current_transfers[destination_name];
	}

	this->copy_shader->use();
	this->copy_shader->set("principle_Texture", 1);
	source_texture->bind(0);

	mesh.draw(GL_NONE, 0, 6);
}

principle::ItemIconManager::const_iterator
principle::ItemIconManager::begin() const
{
	return this->item_icons.begin();
}

principle::ItemIconManager::const_iterator
principle::ItemIconManager::end() const
{
	return this->item_icons.end();
}
