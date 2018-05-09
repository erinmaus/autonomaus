// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_GRAPHICS_SHADER_HPP
#define KVNCER_GRAPHICS_SHADER_HPP

#include <map>
#include <string>
#include <glm/glm.hpp>
#include "kvncer/render/gl.hpp"

namespace kvncer
{
	class Shader
	{
	public:
		enum
		{
			vertex_input_position = 0,
			vertex_input_normal = 1,
			vertex_input_texture_coordinate = 2,
			vertex_input_color = 3,
			vertex_input_bone = 4,
			vertex_input_texture_meta = 5,
			vertex_input_atlas_meta = 6,
			vertex_input_atlas_min = 5,
			vertex_input_atlas_max = 6,
		};

		Shader(
			const std::string& vertex_source,
			const std::string& fragment_source);
		~Shader();

		void use();

		void bind_input(const std::string& name, int input);

		void set(const std::string& uniform, float value);
		void set(const std::string& uniform, int value);
		void set(const std::string& uniform, const glm::vec2& value);
		void set(const std::string& uniform, const glm::vec3& value);
		void set(const std::string& uniform, const glm::vec4& value);
		void set(const std::string& uniform, const glm::mat4& value);
		void set(
			const std::string& uniform,
			const float* value, std::size_t count);
		void set(
			const std::string& uniform,
			const glm::vec2* value, std::size_t count);
		void set(
			const std::string& uniform,
			const glm::vec3* value, std::size_t count);
		void set(
			const std::string& uniform,
			const glm::vec4* value, std::size_t count);
		void set(
			const std::string& uniform,
			const glm::mat4* value, std::size_t count);

		GLuint get_program() const;

	private:
		void link();
		GLint get_uniform_location(const std::string& uniform);

		GLuint vertex_shader, fragment_shader;
		GLuint program;

		bool is_dirty = true;

		std::map<std::string, GLint> uniform_locations;
	};
}

#endif
