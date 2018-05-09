// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <iostream>
#include <memory>
#include <glm/gtc/type_ptr.hpp>
#include "autonomaus/graphics/program.hpp"

static void build_shader(
	touchstone::ImmediateDispatch* gl,
	GLuint shader, const std::string& source)
{
	const char* s = source.c_str();
	gl->ShaderSource(shader, 1, &s, nullptr);
	gl->CompileShader(shader);

	GLint success;
	gl->GetShaderiv(shader, GL_COMPILE_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint length;
		gl->GetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);

		auto log = std::make_unique<char[]>(length);
		gl->GetShaderInfoLog(shader, length, nullptr, log.get());

#ifndef NDEBUG
		std::cout << "failed to compile shader:" << std::endl;
		std::cout << log.get() << std::endl;;
		std::exit(1);
#endif
	}
}

autonomaus::Program::Program(
	Renderer& renderer,
	const std::string& vertex_source,
	const std::string& fragment_source)
{
	this->renderer = &renderer;
	this->gl = this->renderer->dispatch;

	this->vertex_shader = gl->CreateShader(GL_VERTEX_SHADER);
	build_shader(gl, this->vertex_shader, vertex_source);

	this->fragment_shader = gl->CreateShader(GL_FRAGMENT_SHADER);
	build_shader(gl, this->fragment_shader, fragment_source);

	this->program = gl->CreateProgram();
	gl->AttachShader(this->program, this->vertex_shader);
	gl->AttachShader(this->program, this->fragment_shader);

	gl->BindAttribLocation(this->program, vertex_input_position, "vert_Position");
	gl->BindAttribLocation(this->program, vertex_input_texture, "vert_Texture");
	gl->BindAttribLocation(this->program, vertex_input_color, "vert_Color");

	link();
}

autonomaus::Program::~Program()
{
	gl->DeleteShader(this->vertex_shader);
	gl->DeleteShader(this->fragment_shader);
	gl->DeleteProgram(this->program);
}

void autonomaus::Program::use()
{
	this->renderer->use_program(this->program);
}

void autonomaus::Program::link()
{
	this->uniform_locations.clear();
	gl->LinkProgram(this->program);

	GLint success;
	gl->GetProgramiv(this->program, GL_LINK_STATUS, &success);
	if (success == GL_FALSE)
	{
		GLint length;
		gl->GetProgramiv(this->program, GL_INFO_LOG_LENGTH, &length);

		auto log = std::make_unique<char[]>(length);
		gl->GetProgramInfoLog(this->program, length, nullptr, log.get());

#ifndef NDEBUG
		std::cout << "failed to link program:" << std::endl;
		std::cout << log.get() << std::endl;;
		std::exit(1);
#endif
	}
}

void autonomaus::Program::set(const std::string& uniform, float value)
{
	use();
	gl->Uniform1fv(get_uniform_location(uniform), 1, &value);
}

void autonomaus::Program::set(const std::string& uniform, int value)
{
	use();
	gl->Uniform1iv(get_uniform_location(uniform), 1, &value);
}

void autonomaus::Program::set(const std::string& uniform, const glm::vec2& value)
{
	use();
	gl->Uniform2fv(get_uniform_location(uniform), 1, glm::value_ptr(value));
}

void autonomaus::Program::set(const std::string& uniform, const glm::vec3& value)
{
	use();
	gl->Uniform3fv(get_uniform_location(uniform), 1, glm::value_ptr(value));
}

void autonomaus::Program::set(const std::string& uniform, const glm::vec4& value)
{
	use();
	gl->Uniform4fv(get_uniform_location(uniform), 1, glm::value_ptr(value));
}

void autonomaus::Program::set(const std::string& uniform, const glm::mat4& value)
{
	use();
	gl->UniformMatrix4fv(
		get_uniform_location(uniform), 1, GL_TRUE, glm::value_ptr(value));
}

void autonomaus::Program::set(
	const std::string& uniform, const float* value, std::size_t count)
{
	use();
	gl->Uniform1fv(get_uniform_location(uniform), count, value);
}

void autonomaus::Program::set(
	const std::string& uniform, const glm::vec2* value, std::size_t count)
{
	use();
	gl->Uniform2fv(get_uniform_location(uniform), count, (const float*)value);
}

void autonomaus::Program::set(
	const std::string& uniform, const glm::vec3* value, std::size_t count)
{
	use();
	gl->Uniform3fv(get_uniform_location(uniform), count, (const float*)value);
}

void autonomaus::Program::set(
	const std::string& uniform, const glm::vec4* value, std::size_t count)
{
	use();
	gl->Uniform4fv(get_uniform_location(uniform), count, (const float*)value);
}

void autonomaus::Program::set(
	const std::string& uniform, const glm::mat4* value, std::size_t count)
{
	use();
	gl->UniformMatrix4fv(
		get_uniform_location(uniform), count, GL_TRUE, (const float*)value);
}

GLint autonomaus::Program::get_uniform_location(const std::string& uniform)
{
	auto iter = this->uniform_locations.find(uniform);
	if (iter == this->uniform_locations.end())
	{
		GLint result = gl->GetUniformLocation(this->program, uniform.c_str());
		this->uniform_locations[uniform] = result;

		return result;
	}

	return iter->second;
}
