// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "principle/scene/modelMaterial.hpp"

void principle::ModelMaterial::build()
{
	auto shader = get_shader();
	shader->bind_input("vert_Position", kvncer::Shader::vertex_input_position);
	shader->bind_input("vert_Bone", kvncer::Shader::vertex_input_bone);
	shader->bind_input("vert_TextureCoordinate", kvncer::Shader::vertex_input_texture_coordinate);
	shader->bind_input("vert_TextureMeta", kvncer::Shader::vertex_input_texture_meta);
	shader->bind_input("vert_AtlasMeta", kvncer::Shader::vertex_input_atlas_meta);
	shader->bind_input("vert_Color", kvncer::Shader::vertex_input_color);
	shader->bind_input("vert_Normal", kvncer::Shader::vertex_input_normal);
}
