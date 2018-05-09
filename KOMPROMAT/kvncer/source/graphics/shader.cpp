// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <iostream>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include "kvncer/graphics/shader.hpp"

static void build_shader(GLuint shader, const std::string& source)
{
	const char* s = source.c_str();
	glShaderSource(shader, 1, &s, nullptr);
	glCompileShader(shader);

	GLint success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint length;
		glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		auto log = std::make_unique<char[]>(length);
		glGetShaderInfoLog(shader, length, nullptr, log.get());

		std::cerr << "failed to compile shader:" << std::endl;
		std::cerr << log.get() << std::endl;;
	}
}

kvncer::Shader::Shader(
	const std::string& vertex_source,
	const std::string& fragment_source)
{
	this->vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	build_shader(this->vertex_shader, vertex_source);

	this->fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
	build_shader(this->fragment_shader, fragment_source);

	this->program = glCreateProgram();
	glAttachShader(this->program, this->vertex_shader);
	glAttachShader(this->program, this->fragment_shader);
}

void kvncer::Shader::use()
{
	if (this->is_dirty)
	{
		link();
		this->is_dirty = false;
	}
	glUseProgram(this->program);
}

GLuint kvncer::Shader::get_program() const
{
	return this->program;
}

kvncer::Shader::~Shader()
{
	glDeleteShader(this->vertex_shader);
	glDeleteShader(this->fragment_shader);
	glDeleteProgram(this->program);
}

void kvncer::Shader::bind_input(const std::string& attribute, int index)
{
	glBindAttribLocation(this->program, index, attribute.c_str());
	this->is_dirty = true;
}

void kvncer::Shader::link()
{
	this->uniform_locations.clear();
	glLinkProgram(this->program);
}

void kvncer::Shader::set(const std::string& uniform, float value)
{
	glUniform1fv(get_uniform_location(uniform), 1, &value);
}

void kvncer::Shader::set(const std::string& uniform, int value)
{
	glUniform1iv(get_uniform_location(uniform), 1, &value);
}

void kvncer::Shader::set(const std::string& uniform, const glm::vec2& value)
{
	glUniform2fv(get_uniform_location(uniform), 1, glm::value_ptr(value));
}

void kvncer::Shader::set(const std::string& uniform, const glm::vec3& value)
{
	glUniform3fv(get_uniform_location(uniform), 1, glm::value_ptr(value));
}

void kvncer::Shader::set(const std::string& uniform, const glm::vec4& value)
{
	glUniform4fv(get_uniform_location(uniform), 1, glm::value_ptr(value));
}

void kvncer::Shader::set(const std::string& uniform, const glm::mat4& value)
{
	glUniformMatrix4fv(
		get_uniform_location(uniform), 1, GL_TRUE, glm::value_ptr(value));
}

void kvncer::Shader::set(
	const std::string& uniform, const float* value, std::size_t count)
{
	glUniform1fv(get_uniform_location(uniform), count, value);
}

void kvncer::Shader::set(
	const std::string& uniform, const glm::vec2* value, std::size_t count)
{
	glUniform2fv(get_uniform_location(uniform), count, (const float*)value);
}

void kvncer::Shader::set(
	const std::string& uniform, const glm::vec3* value, std::size_t count)
{
	glUniform3fv(get_uniform_location(uniform), count, (const float*)value);
}

void kvncer::Shader::set(
	const std::string& uniform, const glm::vec4* value, std::size_t count)
{
	glUniform4fv(get_uniform_location(uniform), count, (const float*)value);
}

void kvncer::Shader::set(
	const std::string& uniform, const glm::mat4* value, std::size_t count)
{
	glUniformMatrix4fv(
		get_uniform_location(uniform), count, GL_TRUE, (const float*)value);
}

GLint kvncer::Shader::get_uniform_location(const std::string& uniform)
{
	auto iter = this->uniform_locations.find(uniform);
	if (iter == this->uniform_locations.end())
	{
		GLint result = glGetUniformLocation(this->program, uniform.c_str());
		this->uniform_locations[uniform] = result;

		return result;
	}

	return iter->second;
}
