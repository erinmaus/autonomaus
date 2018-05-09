// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_PROGRAM_RESOURCE_HPP
#define TOUCHSTONE_GL_RESOURCES_PROGRAM_RESOURCE_HPP

#include <functional>
#include <map>
#include <set>
#include <string>
#include <unordered_map>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/resource.hpp"
#include "touchstone/gl/resources/uniform.hpp"

namespace touchstone
{
	class ProgramResource : public Resource
	{
	public:
		ProgramResource(Context* context, GLuint name);
		~ProgramResource() = default;

		void attach_shader(GLuint name);
		void detach_shader(GLuint name);
		std::size_t get_num_shaders() const;
		GLuint get_shader_at(std::size_t index) const;

		void set_attrib_location(const std::string& attrib, GLuint location);
		bool has_attrib(const std::string& attrib) const;
		GLint get_attrib_location(const std::string& attrib) const;
		void for_each_attrib(
			const std::function<void(const std::string& /* attrib */, GLuint /* location */)>& func) const;

		void set_frag_location(const std::string& frag, GLuint location);
		bool has_frag(const std::string& frag) const;
		GLint get_frag_location(const std::string& frag) const;
		void for_each_frag(
			const std::function<void(const std::string& /* frag */, GLuint /* location */)>& func) const;

		void set_uniform_block_binding(GLuint index, GLuint value);
		bool has_uniform_block_binding(GLuint index) const;
		bool has_uniform_block_binding(const std::string& name) const;
		GLint get_uniform_block_binding(GLuint index) const;
		GLint get_uniform_block_binding(const std::string& name) const;

		void set_uniform_block_name(GLuint index, const std::string& name);
		bool has_uniform_block_index(const std::string& name) const;
		bool has_uniform_block_name(GLuint index) const;
		GLuint get_uniform_block_index(const std::string& name) const;
		std::string get_uniform_block_name(GLuint index) const;
		void for_each_uniform_block(
			const std::function<void(const std::string& /* name */, GLuint /* index */, GLuint /* binding */)>& func) const;

		void set_uniform1f(GLint location, GLfloat value);
		void set_uniform1fv(GLint location, GLsizei count, const GLfloat* value);
		void set_uniform1i(GLint location, GLint value);
		void set_uniform1iv(GLint location, GLsizei count, const GLint* value);
		void set_uniform1ui(GLint location, GLuint value);
		void set_uniform1uiv(GLint location, GLsizei count, const GLuint* value);
		void set_uniform2f(GLint location, GLfloat x, GLfloat y);
		void set_uniform2fv(GLint location, GLsizei count, const GLfloat* value);
		void set_uniform2i(GLint location, GLint x, GLint y);
		void set_uniform2iv(GLint location, GLsizei count, const GLint* value);
		void set_uniform2ui(GLint location, GLuint x, GLuint y);
		void set_uniform2uiv(GLint location, GLsizei count, const GLuint* value);
		void set_uniform3f(GLint location, GLfloat x, GLfloat y, GLfloat z);
		void set_uniform3fv(GLint location, GLsizei count, const GLfloat* value);
		void set_uniform3i(GLint location, GLint x, GLint y, GLint z);
		void set_uniform3iv(GLint location, GLsizei count, const GLint* value);
		void set_uniform3ui(GLint location, GLuint x, GLuint y, GLuint z);
		void set_uniform3uiv(GLint location, GLsizei count, const GLuint* value);
		void set_uniform4f(GLint location, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
		void set_uniform4fv(GLint location, GLsizei count, const GLfloat* value);
		void set_uniform4i(GLint location, GLint x, GLint y, GLint z, GLint w);
		void set_uniform4iv(GLint location, GLsizei count, const GLint* value);
		void set_uniform4ui(GLint location, GLuint x, GLuint y, GLuint z, GLuint w);
		void set_uniform4uiv(GLint location, GLsizei count, const GLuint* value);
		void set_uniform_matrix2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix2x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix2x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix3x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix3x4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix4fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix4x2fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);
		void set_uniform_matrix4x3fv(GLint location, GLsizei count, GLboolean transpose, const GLfloat* value);

		void set_uniform_location(const std::string& name, GLuint location);
		GLint get_uniform_location(const std::string& name);

		int get_uniform_sampler_type(GLint location) const;
		int get_uniform_sampler_type(const std::string& name) const;
		void set_uniform_sampler_type(GLint location, int type);
		void set_uniform_sampler_type(const std::string& name, int type);

		bool has_uniform(GLint location) const;
		bool has_uniform(const std::string& name) const;
		Uniform get_uniform(GLint location) const;
		Uniform get_uniform(const std::string& name) const;
		GLint get_uniform_location(const std::string& name) const;
		std::string get_uniform_name(GLint location) const;

	private:
		std::set<GLuint> shaders;
		std::map<std::string, GLuint> attribs;
		std::map<std::string, GLuint> frags;

		std::unordered_map<GLint, Uniform> uniforms;
		std::map<GLint, std::string> uniform_names;
		std::map<std::string, GLint> uniform_locations;

		std::unordered_map<GLuint, GLuint> uniform_block_bindings;
		std::unordered_map<GLuint, std::string> uniform_block_names;
		std::unordered_map<std::string, GLuint> uniform_block_indices;
	};
}

#endif
