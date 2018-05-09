// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <fstream>
#include <sstream>
#include "principle/scene/material.hpp"

kvncer::Shader* principle::Material::get_shader() const
{
	return this->shader;
}

void principle::Material::set_shader(kvncer::Shader* value)
{
	this->shader = value;
	if (value != nullptr)
	{
		build();
	}
}

void principle::Material::use()
{
	this->shader->use();
}

bool principle::Material::get_is_enabled() const
{
	return this->shader != nullptr;
}

kvncer::Shader* principle::Material::load_shader_from_file(
	const std::string& vertex_source_filename,
	const std::string& fragment_source_filename)
{
	std::string prefix;
	{
		const char* share_path = std::getenv("HDT_SHARE");
		if (share_path == nullptr)
		{
			prefix = "";
		}
		else
		{
			prefix = std::string(share_path) + "/principle/shaders/";
		}
	}

	std::ifstream vertex_source_stream(prefix + vertex_source_filename);
	std::stringstream vertex_source;
	vertex_source << vertex_source_stream.rdbuf();

	std::ifstream fragment_source_stream(prefix + fragment_source_filename);
	std::stringstream fragment_source;
	fragment_source << fragment_source_stream.rdbuf();

	return new kvncer::Shader(vertex_source.str(), fragment_source.str());
}
