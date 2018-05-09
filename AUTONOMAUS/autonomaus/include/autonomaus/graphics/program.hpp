// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_GRAPHICS_PROGRAM_HPP
#define AUTONOMAUS_GRAPHICS_PROGRAM_HPP

#include <map>
#include <string>
#include <glm/glm.hpp>
#include "autonomaus/graphics/renderer.hpp"

namespace autonomaus
{
	class Program
	{
	public:
		enum
		{
			vertex_input_position = 0,
			vertex_input_texture = 1,
			vertex_input_color = 2
		};

		Program(
			Renderer& renderer,
			const std::string& vertex_source,
			const std::string& fragment_source);
		~Program();

		void use();

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

	private:
		Renderer* renderer;
		touchstone::ImmediateDispatch* gl;

		void link();
		GLint get_uniform_location(const std::string& uniform);

		GLuint vertex_shader, fragment_shader;
		GLuint program;

		std::map<std::string, GLint> uniform_locations;
	};
}

#endif
