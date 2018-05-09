// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_STATE_RESOURCE_HPP
#define TOUCHSTONE_GL_STATE_RESOURCE_HPP

#include <map>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <glm/glm.hpp>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/enums.hpp"

namespace touchstone
{
	class State
	{
	public:
		State();
		~State() = default;

		void set_blend_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
		glm::vec4 get_blend_color() const;

		void set_blend_equation(GLenum color, GLenum alpha);
		GLenum get_blend_equation_color() const;
		GLenum get_blend_equation_alpha() const;

		void set_blend_func(GLenum source_color, GLenum source_alpha,GLenum destination_color, GLenum destination_alpha);
		GLenum get_destination_color_blend_func() const;
		GLenum get_destination_alpha_blend_func() const;
		GLenum get_source_color_blend_func() const;
		GLenum get_source_alpha_blend_func() const;

		void set_clamp_color(GLenum value);
		GLenum get_clamp_color() const;

		void set_clear_color(GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha);
		glm::vec4 get_clear_color() const;

		void set_clear_depth(GLfloat value);
		GLfloat get_clear_depth() const;

		void set_clear_stencil(GLuint value);
		GLuint get_clear_stencil() const;

		void set_color_mask(GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
		void set_color_mask(GLuint index,GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha);
		glm::bvec4 get_color_mask() const;
		glm::bvec4 get_color_mask(GLuint index) const;

		void set_cull_face(GLenum value);
		GLenum get_cull_face() const;

		void set_depth_func(GLenum value);
		GLenum get_depth_func() const;

		void set_depth_mask(GLboolean value);
		GLboolean get_depth_mask() const;

		void set_depth_range(GLfloat near, GLfloat far);
		GLfloat get_depth_range_near() const;
		GLfloat get_depth_range_far() const;

		void set_draw_buffer(GLenum value);
		GLenum get_draw_buffer() const;

		void set_draw_buffers(std::size_t count, const GLenum values[]);
		std::size_t get_num_draw_buffers() const;
		GLenum get_draw_buffer(std::size_t index) const;

		void set_front_face(GLenum value);
		GLenum get_front_face() const;

		void set_line_width(GLfloat value);
		GLfloat get_line_width() const;

		void set_logic_op(GLenum value);
		GLenum get_logic_op() const;

		void set_pixel_store_parameter(GLenum pname, GLfloat value);
		bool has_pixel_store_parameter(GLenum pname) const;
		GLfloat get_pixel_store_parameter(GLenum pname) const;

		void set_point_size(GLfloat value);
		GLfloat get_point_size() const;

		void set_polygon_mode(GLenum face, GLenum mode);
		GLenum get_polygon_mode(GLenum face) const;

		void set_polygon_offset(GLfloat factor, GLfloat units);
		GLfloat get_polygon_offset_factor() const;
		GLfloat get_polygon_offset_units() const;

		void set_primitive_restart_index(GLuint value);
		GLuint get_primitive_restart_index() const;

		void set_provoking_vertex(GLenum value);
		GLenum get_provoking_vertex() const;

		void set_read_buffer(GLenum value);
		GLenum get_read_buffer() const;

		void set_sample_coverage(GLfloat value, GLboolean invert);
		GLfloat get_sample_coverage_value() const;
		GLboolean get_sample_coverage_invert() const;

		void set_sample_mask(GLuint index, GLuint mask);
		GLuint get_sample_mask(GLuint index) const;

		void set_scissor(GLint x, GLint y, GLsizei width, GLsizei height);
		bool get_scissor_set() const;
		GLint get_scissor_x() const;
		GLint get_scissor_y() const;
		GLsizei get_scissor_width() const;
		GLsizei get_scissor_height() const;

		void set_stencil_func(GLenum face, GLenum func, GLint ref, GLuint mask);
		GLenum get_stencil_func(GLenum face) const;
		GLint get_stencil_func_ref(GLenum face) const;
		GLuint get_stencil_func_mask(GLenum face) const;

		void set_stencil_mask(GLenum face, GLuint value);
		GLuint get_stencil_mask(GLenum face) const;

		void set_stencil_op(GLenum face, GLenum stencil_fail, GLenum depth_fail, GLenum depth_pass);
		GLenum get_stencil_op_stencil_fail(GLenum face) const;
		GLenum get_stencil_op_depth_fail(GLenum face) const;
		GLenum get_stencil_op_depth_pass(GLenum face) const;

		void set_viewport(GLint x, GLint y, GLsizei width, GLsizei height);
		bool get_viewport_set() const;
		GLint get_viewport_x() const;
		GLint get_viewport_y() const;
		GLsizei get_viewport_width() const;
		GLsizei get_viewport_height() const;

		void enable(GLenum option);
		void enable(GLuint index, GLenum option);
		void disable(GLenum option);
		void disable(GLuint index, GLenum option);
		bool enabled(GLenum option) const;
		bool enabled(GLuint index, GLenum option) const;

	private:
		glm::vec4 blend_color_value = glm::vec4(0.0f);
		GLenum blend_equation_color_value = GL_FUNC_ADD;
		GLenum blend_equation_alpha_value = GL_FUNC_ADD;
		GLenum blend_source_color_func_value = GL_ONE;
		GLenum blend_source_alpha_func_value = GL_ONE;
		GLenum blend_destination_color_func_value = GL_ZERO;
		GLenum blend_destination_alpha_func_value = GL_ZERO;
		GLenum clamp_color_value = GL_FIXED_ONLY;

		glm::vec4 clear_color_value = glm::vec4(0.0f);
		GLfloat clear_depth_value = 1.0f;
		GLuint clear_stencil_value = 0;

		std::unordered_map<GLuint, glm::bvec4> indexed_color_mask_values;
		glm::bvec4 color_mask_value = glm::bvec4(GL_TRUE);
		GLboolean depth_mask_value = GL_TRUE;

		GLenum cull_face_value = GL_BACK;

		GLenum depth_func_value = GL_LESS;
		GLfloat depth_range_near_value = 0.0f;
		GLfloat depth_range_far_value = 1.0f;

		GLenum draw_buffer_value = GL_BACK;
		std::vector<GLenum> draw_buffer_values;
		GLenum read_buffer_value = GL_BACK;

		GLenum front_face_value = GL_CCW;

		GLfloat line_width_value = 1.0f;
		GLfloat point_size_value = 1.0f;

		GLenum logic_op_value = GL_COPY;

		std::unordered_map<GLenum, GLfloat> pixel_store_parameters;

		std::unordered_map<GLenum, GLenum> polygon_mode_values;
		GLfloat polygon_offset_factor_value = 0.0f;
		GLfloat polygon_offset_units_value = 0.0f;
		GLuint primitive_restart_index_value = 0;

		GLenum provoking_vertex_value = GL_LAST_VERTEX_CONVENTION;

		GLfloat sample_coverage_value = 1.0f;
		GLboolean sample_coverage_invert_value = GL_FALSE;
		std::unordered_map<GLuint, GLuint> sample_mask_values;

		bool scissor_value_set = false;
		GLint scissor_x_value = 0;
		GLint scissor_y_value = 0;
		GLsizei scissor_width_value = 0;
		GLsizei scissor_height_value = 0;

		typedef std::tuple<GLenum, GLint, GLuint> StencilFuncValue;
		std::unordered_map<GLenum, StencilFuncValue> stencil_func_values;
		std::unordered_map<GLenum, GLuint> stencil_mask_values;
		typedef std::tuple<GLenum, GLenum, GLenum> StencilOpValue;
		std::unordered_map<GLenum, StencilOpValue> stencil_op_values;

		bool viewport_value_set = false;
		GLint viewport_x_value = 0;
		GLint viewport_y_value = 0;
		GLsizei viewport_width_value = 0;
		GLsizei viewport_height_value = 0;

		std::unordered_map<GLenum, GLboolean> enable_values;

		typedef std::tuple<GLenum, GLuint> IndexedEnableKey;
		std::map<IndexedEnableKey, GLboolean> indexed_enable_values;
	};
}

#endif
