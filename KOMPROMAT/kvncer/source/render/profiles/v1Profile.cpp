// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/shader.hpp"
#include "kvncer/render/bufferStore.hpp"
#include "kvncer/render/gl.hpp"
#include "kvncer/render/profiles/v1Profile.hpp"

kvncer::V1Profile::V1Profile(BufferStore& buffer_store)
	: buffer_store(buffer_store)
{
	// Nothing.
}

void kvncer::V1Profile::make_model(
	Mesh& mesh, const kompvter::ModelDrawEvent& event)
{
	auto& attributes = event.get_vertex_attributes();
	auto position_bone = attributes.get("aVertexPosition_BoneLabel");
	auto normal = attributes.get("aVertexNormal_BatchFlags");
	auto color = attributes.get("aVertexColourUnwhitened");
	auto texture_coordinate = attributes.get("aTextureUV");
	auto texture_meta = attributes.get("aTextureMeta");
	auto atlas_meta = attributes.get("aSamplerWrap_TilePosition");

	mesh.enable_vertex_attrib(
		Shader::vertex_input_position,
		buffer_store.get_buffer(position_bone->get_buffer().get_name()),
		3, gl::marshal_vertex_fetcher_format_from_kompvter(position_bone->get_format()),
		position_bone->get_normalized(),
		position_bone->get_offset(), position_bone->get_stride());
	// The upper eight bits is the bone index, the lower 8 bits is the label.
	mesh.enable_vertex_attrib(
		Shader::vertex_input_bone,
		buffer_store.get_buffer(position_bone->get_buffer().get_name()),
		1, GL_UNSIGNED_BYTE,
		position_bone->get_normalized(),
		position_bone->get_offset() + gl::get_type_size(gl::marshal_vertex_fetcher_format_from_kompvter(position_bone->get_format())) * 3,
		position_bone->get_stride());
	mesh.enable_vertex_attrib(
		Shader::vertex_input_normal,
		buffer_store.get_buffer(normal->get_buffer().get_name()),
		3, gl::marshal_vertex_fetcher_format_from_kompvter(normal->get_format()),
		normal->get_normalized(),
		normal->get_offset(), normal->get_stride());
	mesh.enable_vertex_attrib(
		Shader::vertex_input_color,
		buffer_store.get_buffer(color->get_buffer().get_name()),
		color->get_num_components(),
		gl::marshal_vertex_fetcher_format_from_kompvter(color->get_format()),
		color->get_normalized(),
		color->get_offset(), color->get_stride());
	mesh.enable_vertex_attrib(
		Shader::vertex_input_texture_coordinate,
		buffer_store.get_buffer(texture_coordinate->get_buffer().get_name()),
		texture_coordinate->get_num_components(),
		gl::marshal_vertex_fetcher_format_from_kompvter(texture_coordinate->get_format()),
		texture_coordinate->get_normalized(),
		texture_coordinate->get_offset(), texture_coordinate->get_stride());
	mesh.enable_vertex_attrib(
		Shader::vertex_input_texture_meta,
		buffer_store.get_buffer(texture_meta->get_buffer().get_name()),
		texture_meta->get_num_components(),
		gl::marshal_vertex_fetcher_format_from_kompvter(texture_meta->get_format()),
		texture_meta->get_normalized(),
		texture_meta->get_offset(), texture_meta->get_stride());
	mesh.enable_vertex_attrib(
		Shader::vertex_input_atlas_meta,
		buffer_store.get_buffer(atlas_meta->get_buffer().get_name()),
		atlas_meta->get_num_components(),
		gl::marshal_vertex_fetcher_format_from_kompvter(atlas_meta->get_format()),
		atlas_meta->get_normalized(),
		atlas_meta->get_offset(), atlas_meta->get_stride());

	auto& index_mapper = event.get_vertex_index_mapper();
	auto& draw_parameters = event.get_draw_parameters();
	if (!index_mapper.get_buffer())
	{
		mesh.set_indices(
			nullptr, GL_NONE,
			draw_parameters.get_offset_start(), draw_parameters.get_offset_stop());
	}
	else
	{
		mesh.set_indices(
			buffer_store.get_buffer(index_mapper.get_buffer().get_name()),
			gl::marshal_index_mapper_format_from_kompvter(index_mapper.get_format()),
			draw_parameters.get_offset_start(), draw_parameters.get_offset_stop());
	}
}

void kvncer::V1Profile::make_terrain(
	Mesh& mesh, const kompvter::ModelDrawEvent & event)
{
	// NYI.
}

void kvncer::V1Profile::make_water(
	Mesh& mesh, const kompvter::ModelDrawEvent & event)
{
	// NYI.
}
