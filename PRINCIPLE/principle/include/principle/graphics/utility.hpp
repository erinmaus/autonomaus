// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_GRAPHICS_UTILITY_HPP
#define PRINCIPLE_GRAPHICS_UTILITY_HPP

#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvncer/graphics/mesh.hpp"
#include "kvncer/graphics/meshBuffer.hpp"
#include "kvre/model/model.hpp"
#include "kvre/terrain/terrain.hpp"
#include "kvre/ui/flatDrawBuffer.hpp"

namespace principle
{
	void generate_untextured_model_vertex_data(
		const kvre::Model* model,
		kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer);
	void generate_untextured_terrain_vertex_data(
		const kompvter::ModelDrawEvent& Event,
		kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer);
	void generate_untextured_terrain_vertex_data(
		const kvre::Terrain* terrain,
		kvncer::Mesh& mesh, kvncer::MeshBuffer& vertex_buffer);
	void generate_gui_mesh(
		const kvre::FlatDrawBuffer* flat_draw_buffer,
		kvncer::Mesh& mesh,
		kvncer::MeshBuffer& vertex_buffer);

	void generate_index_data(
		const kompvter::VertexIndexMapper& index_mapper,
		const kompvter::DrawParameters& draw_parameters,
		kvncer::Mesh& mesh, kvncer::MeshBuffer& index_buffer);
}

#endif
