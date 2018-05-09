// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "thoreau/thoreau.hpp"
#include "principle/graphics/utility.hpp"
#include "principle/scene/modelNode.hpp"
#include "kvncer/render/gl.hpp"
#include "kvre/draw/indexedVertexIterator.hpp"
#include "principle/graphics/thumbnail.hpp"

principle::Thumbnail::Thumbnail(int width, int height)
{
	this->color_attachment = std::make_unique<kvncer::Texture>(
		kvncer::Texture::component_format_rgb, false, width, height);
	this->framebuffer = std::make_unique<kvncer::Framebuffer>(
		this->color_attachment.get(), true);

	load_shader_from_file(
		"model.vert.glsl", "untextured_clipped_model.frag.glsl");
	this->scene.set_camera(&this->camera);
}

int principle::Thumbnail::get_width() const
{
	return this->color_attachment->get_width();
}

int principle::Thumbnail::get_height() const
{
	return this->color_attachment->get_height();
}

void principle::Thumbnail::add_model(
	const kvre::Model* model,
	const glm::mat4& model_matrix,
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters)
{
	this->model_data.emplace_back(new ModelData());
	assert(index_mapper);

	auto m = model_data.back().get();
	auto vertex_buffer = this->model_vertex_buffers.find(model);
	if (vertex_buffer == this->model_vertex_buffers.end())
	{
		vertex_buffer = this->model_vertex_buffers.emplace(model, kvncer::MeshBuffer()).first;
	}
	generate_untextured_model_vertex_data(
		model, m->mesh, vertex_buffer->second);
	generate_index_data(
		index_mapper, draw_parameters, m->mesh, m->index_buffer);

	auto begin = kvre::IndexedVertexIterator::begin(index_mapper, draw_parameters);
	auto end = kvre::IndexedVertexIterator::end(index_mapper, draw_parameters);

	kvlc::Bounds model_bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
	model->for_each_position(
		begin, end,
		[&model_matrix, &model_bounds](auto& position, auto)
		{
			auto transformed_position = model_matrix * glm::vec4(position, 1.0f);
			model_bounds.expand(glm::vec3(transformed_position));
		});
	this->bounds.expand(model_bounds);

	auto node = this->scene.add_node<ModelNode>(&m->mesh);
	node->set_material(&this->model_material);
	node->set_model_matrix(model_matrix);
}

void principle::Thumbnail::add_terrain(
	const kvre::Terrain* terrain,
	const glm::mat4& model_matrix,
	const kompvter::DrawParameters& draw_parameters)
{
	this->model_data.emplace_back(new ModelData());
	auto m = model_data.back().get();
	auto vertex_buffer = this->terrain_vertex_buffers.find(terrain);
	if (vertex_buffer == this->terrain_vertex_buffers.end())
	{
		vertex_buffer = this->terrain_vertex_buffers.emplace(terrain, kvncer::MeshBuffer()).first;
	}
	generate_untextured_terrain_vertex_data(
		terrain, m->mesh, vertex_buffer->second);

	auto node = this->scene.add_node<ModelNode>(&m->mesh,
		GL_NONE,
		draw_parameters.get_offset_start(),
		draw_parameters.get_offset_stop());
	node->set_material(&this->model_material);
	node->set_model_matrix(model_matrix);
}

void principle::Thumbnail::clear()
{
	this->scene.reset();
	this->model_vertex_buffers.clear();
	this->model_data.clear();

	this->bounds = kvlc::Bounds(glm::vec3(HUGE_VALF), glm::vec3(-HUGE_VALF));
}

kvncer::Shader* principle::Thumbnail::get_shader()
{
	return this->shader.get();
}

void principle::Thumbnail::load_shader_from_file(
	const std::string& vertex_filename,
	const std::string& fragment_filename)
{
	auto value = Material::load_shader_from_file(
		vertex_filename, fragment_filename);
	this->shader = std::unique_ptr<kvncer::Shader>(value);
	this->model_material.set_shader(value);
}

glm::vec4 principle::Thumbnail::get_background_color() const
{
	return this->background_color;
}

void principle::Thumbnail::set_background_color(const glm::vec4& value)
{
	this->background_color = value;
}

kvncer::Texture* principle::Thumbnail::draw(const glm::mat4& model)
{
	glm::mat4 view, projection, adjusted_model;
	compute_projection_view_model(model, projection, view, adjusted_model);

	start_draw();
	{
		this->camera.set_projection_matrix(projection);
		this->camera.set_view_matrix(view * adjusted_model);
		this->model_material.use();
		this->model_material.get_shader()->set("principle_MinBounds", glm::vec3(-HUGE_VALF));
		this->model_material.get_shader()->set("principle_MaxBounds", glm::vec3(HUGE_VALF));

		this->scene.draw();
	}
	return stop_draw();
}

void principle::Thumbnail::start_draw(bool use_framebuffer)
{
	if (use_framebuffer)
	{
		this->framebuffer->bind();
		glViewport(0, 0, this->color_attachment->get_width(), this->color_attachment->get_height());
	}

	glClearColor(
		this->background_color.r,
		this->background_color.g,
		this->background_color.b,
		this->background_color.a);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
}

void principle::Thumbnail::draw_clipped_to_tile(int x, int y, const glm::mat4& model)
{
	glm::mat4 view, projection, adjusted_model;
	compute_projection_view_model(model, projection, view, adjusted_model);

	float world_x_min = x * thoreau::TILE_SIZE;
	float world_z_min = y * thoreau::TILE_SIZE;
	float world_x_max = (x + 1) * thoreau::TILE_SIZE;
	float world_z_max = (y + 1) * thoreau::TILE_SIZE;
	glm::vec3 min_bounds(world_x_min, -HUGE_VALF, world_z_min);
	glm::vec3 max_bounds(world_x_max, HUGE_VALF, world_z_max);

	this->camera.set_projection_matrix(projection);
	this->camera.set_view_matrix(view * adjusted_model);
	this->model_material.use();
	this->model_material.get_shader()->set("principle_MinBounds", min_bounds);
	this->model_material.get_shader()->set("principle_MaxBounds", max_bounds);

	this->scene.draw();
}

kvncer::Texture* principle::Thumbnail::stop_draw()
{
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	return this->color_attachment.get();
}

void principle::Thumbnail::compute_projection_view_model(
	const glm::mat4& input_model,
	glm::mat4& result_projection,
	glm::mat4& result_view,
	glm::mat4& result_model) const
{
	glm::vec3 dimensions = get_bounds().compute_size();
	glm::vec3 center = get_bounds().compute_center();
	glm::vec3 eye(center.x, center.y, center.z);
	float far_distance = glm::max(glm::max(dimensions.x, dimensions.y), dimensions.z) * 2.0f;
	eye.z += far_distance / 2.0f;

	result_projection = glm::perspective(
		(float)M_PI * 0.5f,
		(float)this->color_attachment->get_width() / (float)this->color_attachment->get_height(),
		1.0f, far_distance);
	result_view = glm::lookAt(eye, center, glm::vec3(0.0f, -1.0f, 0.0f));
	result_model =
		glm::translate(glm::mat4(1.0f), center) *
		input_model *
		glm::translate(glm::mat4(1.0f), -center);
}


kvlc::Bounds principle::Thumbnail::get_bounds() const
{
	if (this->custom_bounds_set)
	{
		return this->custom_bounds;
	}

	return this->bounds;
}

void principle::Thumbnail::set_bounds(const kvlc::Bounds& value)
{
	this->custom_bounds = value;
	this->custom_bounds_set = true;
}

void principle::Thumbnail::unset_bounds()
{
	this->custom_bounds_set = false;
}
