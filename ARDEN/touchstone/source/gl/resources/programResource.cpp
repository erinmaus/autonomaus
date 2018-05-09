// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <algorithm>
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/resources/shaderResource.hpp"
#include "touchstone/gl/resources/programResource.hpp"

touchstone::ProgramResource::ProgramResource(Context* context, GLuint name) :
	Resource(context, name)
{
	// NOP
}

void touchstone::ProgramResource::attach_shader(GLuint name)
{
	this->shaders.insert(name);
}

void touchstone::ProgramResource::detach_shader(GLuint name)
{
	this->shaders.erase(name);
}

std::size_t touchstone::ProgramResource::get_num_shaders() const
{
	return this->shaders.size();
}

GLuint touchstone::ProgramResource::get_shader_at(
	std::size_t index) const
{
	assert(!this->shaders.empty());

	auto i = this->shaders.begin();
	std::size_t current = 0;
	while (current < index)
	{
		++i;
		assert(i != this->shaders.end() && "shader index out of bounds");

		++current;
	}

	return *i;
}

void touchstone::ProgramResource::set_attrib_location(
	const std::string& attrib, GLuint location)
{
	this->attribs[attrib] = location;
}

bool touchstone::ProgramResource::has_attrib(const std::string& attrib) const
{
	return this->attribs.count(attrib) != 0;
}

GLint touchstone::ProgramResource::get_attrib_location(
	const std::string& attrib) const
{
	auto iter = this->attribs.find(attrib);
	if (iter != this->attribs.end())
	{
		return iter->second;
	}

	return -1;
}

void touchstone::ProgramResource::for_each_attrib(
	const std::function<void(const std::string&, GLuint)>& func) const
{
	for (auto attrib: this->attribs)
	{
		func(attrib.first, attrib.second);
	}
}

void touchstone::ProgramResource::set_frag_location(
	const std::string& frag, GLuint location)
{
	this->frags[frag] = location;
}

bool touchstone::ProgramResource::has_frag(const std::string& frag) const
{
	return this->frags.count(frag) != 0;
}

GLint touchstone::ProgramResource::get_frag_location(
	const std::string& frag) const
{
	auto iter = this->frags.find(frag);
	if (iter != this->frags.end())
	{
		return iter->second;
	}

	return -1;
}

void touchstone::ProgramResource::for_each_frag(
	const std::function<void(const std::string&, GLuint)>& func) const
{
	for (auto frag: this->frags)
	{
		func(frag.first, frag.second);
	}
}


void touchstone::ProgramResource::set_uniform_block_binding(
	GLuint index, GLuint value)
{
	this->uniform_block_bindings[index] = value;
}

bool touchstone::ProgramResource::has_uniform_block_binding(GLuint index) const
{
	return this->uniform_block_bindings.count(index) != 0;
}

bool touchstone::ProgramResource::has_uniform_block_binding(const std::string& name) const
{
	if (has_uniform_block_index(name))
	{
		auto index = get_uniform_block_index(name);
		return has_uniform_block_binding(index);
	}

	return false;
}

GLint touchstone::ProgramResource::get_uniform_block_binding(GLuint index) const
{
	auto iter = this->uniform_block_bindings.find(index);
	if (iter != this->uniform_block_bindings.end())
	{
		return iter->second;
	}

	return -1;
}

GLint touchstone::ProgramResource::get_uniform_block_binding(
	const std::string& name) const
{
	if (has_uniform_block_index(name))
	{
		auto index = get_uniform_block_index(name);
		return get_uniform_block_binding(index);
	}

	return -1;
}

void touchstone::ProgramResource::set_uniform_block_name(
	GLuint index, const std::string& name)
{
	this->uniform_block_names[index] = name;
	this->uniform_block_indices[name] = index;
}

bool touchstone::ProgramResource::has_uniform_block_name(GLuint index) const
{
	return this->uniform_block_names.count(index) != 0;
}

bool touchstone::ProgramResource::has_uniform_block_index(
	const std::string& name) const
{
	return this->uniform_block_indices.count(name) != 0;
}

GLuint touchstone::ProgramResource::get_uniform_block_index(
	const std::string& name) const
{
	auto iter = this->uniform_block_indices.find(name);
	if (iter != this->uniform_block_indices.end())
	{
		return iter->second;
	}

	return GL_INVALID_INDEX;
}

std::string touchstone::ProgramResource::get_uniform_block_name(GLuint index) const
{
	auto iter = this->uniform_block_names.find(index);
	if (iter != this->uniform_block_names.end())
	{
		return iter->second;
	}

	return std::string();
}

void touchstone::ProgramResource::for_each_uniform_block(
	const std::function<void(const std::string&, GLuint, GLuint)>& func) const
{
	for (auto i: this->uniform_block_bindings)
	{
		auto name = this->uniform_block_names.find(i.second);

		if (name != this->uniform_block_names.end())
		{
			func(name->second, i.first, i.second);
		}
	}
}

void touchstone::ProgramResource::set_uniform1f(
	GLint location, GLfloat value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.single = value;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform1fv(
	GLint location, GLsizei count, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single;
	uniform.values.clear();
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.single = value[i];
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform1i(
	GLint location, GLint value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.integer = value;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform1iv(
	GLint location, GLsizei count, const GLint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer;
	uniform.values.clear();
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.integer = value[i];
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform1ui(
	GLint location, GLuint value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer = value;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform1uiv(
	GLint location, GLsizei count, const GLuint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer;
	uniform.values.clear();
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer = value[i];
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform2f(
	GLint location, GLfloat x, GLfloat y)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single_vec2;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.single_vec2.x = x;
		v.single_vec2.y = y;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform2fv(
	GLint location, GLsizei count, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single_vec2;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.single_vec2 = *(const glm::vec2*)current;
		uniform.values.push_back(v);
		current += 2;
	}
}

void touchstone::ProgramResource::set_uniform2i(
	GLint location, GLint x, GLint y)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer_vec2;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.integer_vec2.x = x;
		v.integer_vec2.y = y;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform2iv(
	GLint location, GLsizei count, const GLint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer_vec2;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.integer_vec2 = *(const glm::ivec2*)current;
		uniform.values.push_back(v);
		current += 2;
	}
}

void touchstone::ProgramResource::set_uniform2ui(
	GLint location, GLuint x, GLuint y)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer_vec2;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer_vec2.x = x;
		v.unsigned_integer_vec2.y = y;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform2uiv(
	GLint location, GLsizei count, const GLuint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer_vec2;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer_vec2 = *(const glm::uvec2*)current;
		uniform.values.push_back(v);
		current += 2;
	}
}

void touchstone::ProgramResource::set_uniform3f(
	GLint location, GLfloat x, GLfloat y, GLfloat z)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single_vec3;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.single_vec3.x = x;
		v.single_vec3.y = y;
		v.single_vec3.z = z;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform3fv(
	GLint location, GLsizei count, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single_vec3;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.single_vec3 = *(const glm::vec3*)current;
		uniform.values.push_back(v);
		current += 3;
	}
}

void touchstone::ProgramResource::set_uniform3i(
	GLint location, GLint x, GLint y, GLint z)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer_vec3;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.integer_vec3.x = x;
		v.integer_vec3.y = y;
		v.integer_vec3.z = z;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform3iv(
	GLint location, GLsizei count, const GLint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer_vec3;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.integer_vec3 = *(const glm::ivec3*)current;
		uniform.values.push_back(v);
		current += 3;
	}
}

void touchstone::ProgramResource::set_uniform3ui(
	GLint location, GLuint x, GLuint y, GLuint z)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer_vec3;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer_vec3.x = x;
		v.unsigned_integer_vec3.y = y;
		v.unsigned_integer_vec3.z = z;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform3uiv(
	GLint location, GLsizei count, const GLuint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer_vec3;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer_vec3 = *(const glm::uvec3*)current;
		uniform.values.push_back(v);
		current += 3;
	}
}

void touchstone::ProgramResource::set_uniform4f(
	GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single_vec4;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.single_vec4.x = x;
		v.single_vec4.y = y;
		v.single_vec4.z = z;
		v.single_vec4.w = w;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform4fv(
	GLint location, GLsizei count, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_single_vec4;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.single_vec4 = *(const glm::vec4*)current;
		uniform.values.push_back(v);
		current += 4;
	}
}

void touchstone::ProgramResource::set_uniform4i(
	GLint location, GLint x, GLint y, GLint z, GLint w)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer_vec4;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.integer_vec4.x = x;
		v.integer_vec4.y = y;
		v.integer_vec4.z = z;
		v.integer_vec4.w = w;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform4iv(
	GLint location, GLsizei count, const GLint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_integer_vec4;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.integer_vec4 = *(const glm::ivec4*)current;
		uniform.values.push_back(v);
		current += 4;
	}
}

void touchstone::ProgramResource::set_uniform4ui(
	GLint location, GLuint x, GLuint y, GLuint z, GLuint w)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer_vec4;
	uniform.values.clear();
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer_vec4.x = x;
		v.unsigned_integer_vec4.y = y;
		v.unsigned_integer_vec4.z = z;
		v.unsigned_integer_vec4.w = w;
		uniform.values.push_back(v);
	}
}

void touchstone::ProgramResource::set_uniform4uiv(
	GLint location, GLsizei count, const GLuint* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_unsigned_integer_vec4;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.unsigned_integer_vec4 = *(const glm::uvec4*)current;
		uniform.values.push_back(v);
		current += 4;
	}
}

void touchstone::ProgramResource::set_uniform_matrix2fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix2;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix2 = *(const glm::mat2*)current;
		uniform.values.push_back(v);
		current += 4;
	}
}

void touchstone::ProgramResource::set_uniform_matrix2x3fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix2x3;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix2x3 = *(const glm::mat2x3*)current;
		uniform.values.push_back(v);
		current += 6;
	}
}

void touchstone::ProgramResource::set_uniform_matrix2x4fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix2x4;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix2x4 = *(const glm::mat2x4*)current;
		uniform.values.push_back(v);
		current += 8;
	}
}

void touchstone::ProgramResource::set_uniform_matrix3fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix3;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix3 = *(const glm::mat3*)current;
		uniform.values.push_back(v);
		current += 9;
	}
}

void touchstone::ProgramResource::set_uniform_matrix3x2fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix3x2;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix3x2 = *(const glm::mat3x2*)current;
		uniform.values.push_back(v);
		current += 6;
	}
}

void touchstone::ProgramResource::set_uniform_matrix3x4fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix3x4;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix3x4 = *(const glm::mat3x4*)current;
		uniform.values.push_back(v);
		current += 12;
	}
}

void touchstone::ProgramResource::set_uniform_matrix4fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix4;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix4 = *(const glm::mat4*)current;
		uniform.values.push_back(v);
		current += 16;
	}
}

void touchstone::ProgramResource::set_uniform_matrix4x2fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix4x2;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix4x2 = *(const glm::mat4x2*)current;
		uniform.values.push_back(v);
		current += 8;
	}
}

void touchstone::ProgramResource::set_uniform_matrix4x3fv(
	GLint location, GLsizei count, GLboolean transpose, const GLfloat* value)
{
	auto& uniform = this->uniforms[location];
	uniform.type = Uniform::type_matrix4x3;
	uniform.values.clear();
	auto current = value;
	for (GLsizei i = 0; i < count; ++i)
	{
		Uniform::Value v = { 0 };
		v.matrix4x3 = *(const glm::mat4x3*)current;
		uniform.values.push_back(v);
		current += 12;
	}
}

void touchstone::ProgramResource::set_uniform_location(
	const std::string& name, GLuint location)
{
	this->uniform_locations[name] = location;
	this->uniform_names[location] = name;
}

GLint touchstone::ProgramResource::get_uniform_location(
	const std::string& name)
{
	auto iter = this->uniform_locations.find(name);
	if (iter != this->uniform_locations.end())
	{
		return iter->second;
	}

	return -1;
}

int touchstone::ProgramResource::get_uniform_sampler_type(GLint location) const
{
	auto iter = this->uniforms.find(location);
	if (iter != this->uniforms.end())
	{
		return iter->second.sampler_type;
	}

	return Uniform::sampler_none;
}

int touchstone::ProgramResource::get_uniform_sampler_type(
	const std::string& name) const
{
	return get_uniform_sampler_type(get_uniform_location(name));
}

void touchstone::ProgramResource::set_uniform_sampler_type(
	GLint location, int type)
{
	if (location >= 0)
	{
		auto& uniform = this->uniforms[location];
		uniform.sampler_type = type;
	}
}

void touchstone::ProgramResource::set_uniform_sampler_type(
	const std::string& name, int type)
{
	auto location = get_uniform_location(name);
	if (has_uniform(location))
	{
		auto& uniform = this->uniforms[location];
		uniform.sampler_type = type;
	}
}

bool touchstone::ProgramResource::has_uniform(GLint location) const
{
	return this->uniforms.count(location) != 0;
}

bool touchstone::ProgramResource::has_uniform(const std::string& name) const
{
	return has_uniform(get_uniform_location(name));
}

touchstone::Uniform touchstone::ProgramResource::get_uniform(GLint location) const
{
	auto iter = this->uniforms.find(location);
	if (iter != this->uniforms.end())
	{
		return iter->second;
	}

	Uniform result;
	result.type = Uniform::type_none;

	return result;
}

touchstone::Uniform touchstone::ProgramResource::get_uniform(
	const std::string& name) const
{
	auto iter = this->uniform_locations.find(name);
	if (iter == this->uniform_locations.end())
	{
		Uniform result;
		result.type = Uniform::type_none;

		return result;
	}

	return get_uniform(iter->second);
}

GLint touchstone::ProgramResource::get_uniform_location(
	const std::string& name) const
{
	auto iter = this->uniform_locations.find(name);
	if (iter != this->uniform_locations.end())
	{
		return iter->second;
	}

	return -1;
}

std::string touchstone::ProgramResource::get_uniform_name(GLint location) const
{
	auto iter = this->uniform_names.find(location);
	if (iter != this->uniform_names.end())
	{
		return iter->second;
	}

	return std::string();
}
