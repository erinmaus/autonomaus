// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_SCENE_MATERIAL_HPP
#define PRINCIPLE_SCENE_MATERIAL_HPP

#include <memory>
#include <string>
#include "kvncer/graphics/shader.hpp"

namespace principle
{
	class Material
	{
	public:
		Material() = default;
		virtual ~Material() = default;

		kvncer::Shader* get_shader() const;
		void set_shader(kvncer::Shader* shader);

		virtual void use();
		virtual bool get_is_enabled() const;

		static kvncer::Shader* load_shader_from_file(
			const std::string& vertex_source_filename,
			const std::string& fragment_source_filename);

	protected:
		virtual void build() = 0;

	private:
		kvncer::Shader* shader = nullptr;
	};
}

#endif
