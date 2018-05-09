// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvre/profile/profile.hpp"
#include "kvncer/graphics/shader.hpp"
#include "kvncer/render/gl.hpp"
#include "principle/graphics/utility.hpp"

void principle::generate_untextured_model_vertex_data(
	const kvre::Model* model,
	kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer)
{
	struct Vertex
	{
		glm::vec3 position;
		float bone;
		glm::vec3 normal;
		glm::vec4 color;
		glm::vec4 texture_meta;
		glm::vec2 texture;
	};

	std::size_t num_vertices = model->get_num_vertices();
	kompvter::Buffer vertex_buffer_data;
	vertex_buffer_data.allocate(sizeof(Vertex) * num_vertices);
	vertex_buffer_data.clear();
	auto v = (Vertex*)vertex_buffer_data.get_data();

	for (std::size_t i = 0; i < num_vertices; ++i)
	{
		v[i].position = model->get_position(i);
		v[i].bone = model->get_bone(i);
		v[i].normal = model->get_normal(i);
		v[i].color = model->get_color(i);
		v[i].texture_meta = model->get_texture_meta(i);
		v[i].texture = model->get_texture_coordinate(i);
	}
	vertex_buffer.set_buffer(vertex_buffer_data);

	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position, &vertex_buffer,
		3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_bone, &vertex_buffer,
		1, GL_FLOAT, GL_FALSE, offsetof(Vertex, bone), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_normal, &vertex_buffer,
		3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_color, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_texture_coordinate, &vertex_buffer,
		2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texture), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_texture_meta, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, texture_meta), sizeof(Vertex));
}

void principle::generate_untextured_terrain_vertex_data(
	const kompvter::ModelDrawEvent& event,
	kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer)
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
		glm::vec4 texture_slot;
		glm::vec4 texture_size;
		glm::vec4 texture_scale;
		glm::vec4 texture_weight;
	};

	auto position = event.get_vertex_attributes().get("aVertexPosition_Spec");
	auto texture_slot = event.get_vertex_attributes().get("aTextureSlot");
	auto texture_size = event.get_vertex_attributes().get("aTextureSize");
	auto texture_scale = event.get_vertex_attributes().get("aTextureScale");
	auto texture_weight = event.get_vertex_attributes().get("aTextureWeight");
	auto color = event.get_vertex_attributes().get("aVertexColour");

	std::size_t num_vertices = position->get_num_vertices();
	kompvter::Buffer vertex_buffer_data;
	vertex_buffer_data.allocate(sizeof(Vertex) * num_vertices);
	auto v = (Vertex*)vertex_buffer_data.get_data();

	position->extract(0, num_vertices, &v->position.x, 3, sizeof(Vertex));
	texture_slot->extract(0, num_vertices, &v->texture_slot.x, 4, sizeof(Vertex));
	texture_size->extract(0, num_vertices, &v->texture_size.x, 4, sizeof(Vertex));
	texture_scale->extract(0, num_vertices, &v->texture_scale.x, 4, sizeof(Vertex));
	texture_weight->extract(0, num_vertices, &v->texture_weight.x, 4, sizeof(Vertex));
	color->extract(0, num_vertices, &v->color.x, 4, sizeof(Vertex));
	vertex_buffer.set_buffer(vertex_buffer_data);

	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position, &vertex_buffer,
		3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_normal, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, texture_weight), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_color, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_texture_coordinate, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, texture_slot), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_texture_meta, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, texture_size), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_atlas_meta, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, texture_scale), sizeof(Vertex));
}

void principle::generate_untextured_terrain_vertex_data(
	const kvre::Terrain* terrain,
	kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer)
{
	struct Vertex
	{
		glm::vec3 position;
		glm::vec4 color;
	};

	std::size_t num_vertices = terrain->get_num_vertices();
	kompvter::Buffer vertex_buffer_data;
	vertex_buffer_data.allocate(sizeof(Vertex) * num_vertices);
	auto v = (Vertex*)vertex_buffer_data.get_data();
	for (std::size_t i = 0; i < num_vertices; ++i)
	{
		v[i].position = terrain->get_position(i);
		v[i].color = terrain->get_color(i);
	}
	vertex_buffer.set_buffer(vertex_buffer_data);

	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_position, &vertex_buffer,
		3, GL_FLOAT, GL_FALSE, offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
		kvncer::Shader::vertex_input_color, &vertex_buffer,
		4, GL_FLOAT, GL_FALSE, offsetof(Vertex, color), sizeof(Vertex));
}

void principle::generate_gui_mesh(
	const kvre::FlatDrawBuffer* flat_draw_buffer,
	kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer)
{
	struct Vertex
	{
		glm::vec2 position;
		glm::vec3 texture_coordinate;
		glm::vec4 texture_meta;
		glm::vec4 color;
	};

	std::vector<Vertex> vertices;
	vertices.resize(flat_draw_buffer->get_num_vertices());
	for (std::size_t i = 0; i < flat_draw_buffer->get_num_vertices(); ++i)
	{
		vertices[i].position = flat_draw_buffer->get_position(i);

		auto texture_coordinate = flat_draw_buffer->get_texture_coordinate(i);
		float is_textured = flat_draw_buffer->is_textured(i);
		vertices[i].texture_coordinate = glm::vec3(texture_coordinate, is_textured);

		auto atlas_min = flat_draw_buffer->get_texture_atlas_min(i);
		auto atlas_max = flat_draw_buffer->get_texture_atlas_max(i);
		vertices[i].texture_meta = glm::vec4(atlas_min, atlas_max - atlas_min);

		vertices[i].color = flat_draw_buffer->get_color(i);
	}

	kompvter::Buffer vertex_buffer_data;
	vertex_buffer_data.allocate(sizeof(Vertex) * vertices.size());
	vertex_buffer_data.update(
		0, (const std::uint8_t*)&vertices[0], vertex_buffer_data.get_data_size());
	vertex_buffer.set_buffer(vertex_buffer_data);

	mesh.enable_vertex_attrib(
			kvncer::Shader::vertex_input_position,
			&vertex_buffer,
			2, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, position), sizeof(Vertex));
	mesh.enable_vertex_attrib(
			kvncer::Shader::vertex_input_texture_coordinate,
			&vertex_buffer,
			3, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, texture_coordinate), sizeof(Vertex));
	mesh.enable_vertex_attrib(
			kvncer::Shader::vertex_input_texture_meta,
			&vertex_buffer,
			4, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, texture_meta), sizeof(Vertex));
	mesh.enable_vertex_attrib(
			kvncer::Shader::vertex_input_color,
			&vertex_buffer,
			4, GL_FLOAT, GL_FALSE,
			offsetof(Vertex, color), sizeof(Vertex));
}

void principle::generate_index_data(
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters,
	kvncer::Mesh& mesh, kvncer::MeshBuffer& index_buffer)
{
	if (index_mapper)
	{
		index_buffer.set_buffer(index_mapper.get_buffer());

		mesh.set_indices(
			&index_buffer,
			kvncer::gl::marshal_index_mapper_format_from_kompvter(index_mapper.get_format()),
			draw_parameters.get_offset_start(),
			draw_parameters.get_offset_stop());
	}
	else
	{
		mesh.set_indices(
			nullptr,
			GL_NONE,
			draw_parameters.get_offset_start(),
			draw_parameters.get_offset_stop());
	}
}
