// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "touchstone/gl/state.hpp"

touchstone::State::State()
{
	this->polygon_mode_values[GL_FRONT] = GL_FILL;
	this->polygon_mode_values[GL_BACK] = GL_FILL;

	this->stencil_func_values[GL_FRONT] = std::make_tuple(GL_ALWAYS, 0, 0xffffffff);
	this->stencil_func_values[GL_BACK] = std::make_tuple(GL_ALWAYS, 0, 0xffffffff);
	this->stencil_mask_values[GL_FRONT] = 0xffffffff;
	this->stencil_mask_values[GL_BACK] = 0xffffffff;
	this->stencil_op_values[GL_FRONT] = std::make_tuple(GL_KEEP, GL_KEEP, GL_KEEP);
	this->stencil_op_values[GL_BACK] = std::make_tuple(GL_KEEP, GL_KEEP, GL_KEEP);

	this->enable_values[GL_DITHER] = GL_TRUE;
	this->enable_values[GL_MULTISAMPLE] = GL_TRUE;
}

void touchstone::State::set_blend_color(
	GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	this->blend_color_value = glm::vec4(red, green, blue, alpha);
}

glm::vec4 touchstone::State::get_blend_color() const
{
	return this->blend_color_value;
}

void touchstone::State::set_blend_equation(
	GLenum color, GLenum alpha)
{
	this->blend_equation_color_value = color;
	this->blend_equation_alpha_value = alpha;
}

GLenum touchstone::State::get_blend_equation_color() const
{
	return this->blend_equation_color_value;
}

GLenum touchstone::State::get_blend_equation_alpha() const
{
	return this->blend_equation_alpha_value;
}

void touchstone::State::set_blend_func(
	GLenum source_color, GLenum source_alpha,
	GLenum destination_color, GLenum destination_alpha)
{
	this->blend_source_color_func_value = source_color;
	this->blend_source_alpha_func_value = source_alpha;
	this->blend_destination_color_func_value = destination_color;
	this->blend_destination_alpha_func_value = destination_alpha;
}

GLenum touchstone::State::get_destination_color_blend_func() const
{
	return this->blend_destination_color_func_value;
}

GLenum touchstone::State::get_destination_alpha_blend_func() const
{
	return this->blend_destination_alpha_func_value;
}

GLenum touchstone::State::get_source_color_blend_func() const
{
	return this->blend_source_color_func_value;
}

GLenum touchstone::State::get_source_alpha_blend_func() const
{
	return this->blend_source_alpha_func_value;
}

void touchstone::State::set_clamp_color(GLenum value)
{
	this->clamp_color_value = value;
}

GLenum touchstone::State::get_clamp_color() const
{
	return this->clamp_color_value;
}

void touchstone::State::set_clear_color(
	GLfloat red, GLfloat green, GLfloat blue, GLfloat alpha)
{
	this->clear_color_value = glm::vec4(red, green, blue, alpha);
}

glm::vec4 touchstone::State::get_clear_color() const
{
	return this->clear_color_value;
}

void touchstone::State::set_clear_depth(GLfloat value)
{
	this->clear_depth_value = value;
}

GLfloat touchstone::State::get_clear_depth() const
{
	return this->clear_depth_value;
}

void touchstone::State::set_clear_stencil(GLuint value)
{
	this->clear_stencil_value = value;
}

GLuint touchstone::State::get_clear_stencil() const
{
	return this->clear_stencil_value;
}

void touchstone::State::set_color_mask(
	GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	this->color_mask_value = glm::bvec4(red, green, blue, alpha);
}

void touchstone::State::set_color_mask(
	GLuint index,
	GLboolean red, GLboolean green, GLboolean blue, GLboolean alpha)
{
	this->indexed_color_mask_values[index] = glm::bvec4(red, green, blue, alpha);
}

glm::bvec4 touchstone::State::get_color_mask() const
{
	return this->color_mask_value;
}

glm::bvec4 touchstone::State::get_color_mask(GLuint index) const
{
	auto iter = this->indexed_color_mask_values.find(index);
	if (iter != this->indexed_color_mask_values.end())
	{
		return iter->second;
	}

	return get_color_mask();
}

void touchstone::State::set_cull_face(GLenum value)
{
	this->cull_face_value = value;
}

GLenum touchstone::State::get_cull_face() const
{
	return this->cull_face_value;
}

void touchstone::State::set_depth_func(
	GLenum value)
{
	this->depth_func_value = value;
}

GLenum touchstone::State::get_depth_func() const
{
	return this->depth_func_value;
}

void touchstone::State::set_depth_mask(
	GLboolean value)
{
	this->depth_mask_value = value;
}

GLboolean touchstone::State::get_depth_mask() const
{
	return this->depth_mask_value;
}

void touchstone::State::set_depth_range(
	GLfloat near, GLfloat far)
{
	this->depth_range_near_value = near;
	this->depth_range_far_value = far;
}

GLfloat touchstone::State::get_depth_range_near() const
{
	return this->depth_range_near_value;
}

GLfloat touchstone::State::get_depth_range_far() const
{
	return this->depth_range_far_value;
}

void touchstone::State::set_draw_buffer(GLenum value)
{
	this->draw_buffer_value = value;
}

GLenum touchstone::State::get_draw_buffer() const
{
	return this->draw_buffer_value;
}

void touchstone::State::set_draw_buffers(std::size_t count, const GLenum values[])
{
	this->draw_buffer_values.resize(count);
	for (std::size_t i = 0; i < count; ++i)
	{
		this->draw_buffer_values[i] = values[i];
	}
}

std::size_t touchstone::State::get_num_draw_buffers() const
{
	return this->draw_buffer_values.size();
}

GLenum touchstone::State::get_draw_buffer(std::size_t index) const
{
	return this->draw_buffer_values.at(index);
}

void touchstone::State::set_front_face(GLenum value)
{
	this->front_face_value = value;
}

GLenum touchstone::State::get_front_face() const
{
	return this->front_face_value;
}

void touchstone::State::set_line_width(GLfloat value)
{
	this->line_width_value = value;
}

GLfloat touchstone::State::get_line_width() const
{
	return this->line_width_value;
}

void touchstone::State::set_logic_op(GLenum value)
{
	this->logic_op_value = value;
}

GLenum touchstone::State::get_logic_op() const
{
	return this->logic_op_value;
}

void touchstone::State::set_pixel_store_parameter(
	GLenum pname, GLfloat value)
{
	this->pixel_store_parameters[pname] = value;
}

bool touchstone::State::has_pixel_store_parameter(GLenum pname) const
{
	return this->pixel_store_parameters.count(pname) != 0;
}

GLfloat touchstone::State::get_pixel_store_parameter(GLenum pname) const
{
	assert(has_pixel_store_parameter(pname));
	return this->pixel_store_parameters.find(pname)->second;
}

void touchstone::State::set_point_size(GLfloat value)
{
	this->point_size_value = value;
}

GLfloat touchstone::State::get_point_size() const
{
	return this->point_size_value;
}

void touchstone::State::set_polygon_mode(GLenum face, GLenum mode)
{
	if (face == GL_FRONT_AND_BACK)
	{
		set_polygon_mode(GL_FRONT, mode);
		set_polygon_mode(GL_BACK, mode);
	}
	else
	{
		this->polygon_mode_values[face] = mode;
	}
}

GLenum touchstone::State::get_polygon_mode(GLenum face) const
{
	assert(this->polygon_mode_values.find(face) != this->polygon_mode_values.end());
	return this->polygon_mode_values.find(face)->second;
}

void touchstone::State::set_polygon_offset(GLfloat factor, GLfloat units)
{
	this->polygon_offset_factor_value = factor;
	this->polygon_offset_units_value = factor;
}

GLfloat touchstone::State::get_polygon_offset_factor() const
{
	return this->polygon_offset_factor_value;
}

GLfloat touchstone::State::get_polygon_offset_units() const
{
	return this->polygon_offset_units_value;
}

void touchstone::State::set_primitive_restart_index(GLuint value)
{
	this->primitive_restart_index_value = value;
}

GLuint touchstone::State::get_primitive_restart_index() const
{
	return this->primitive_restart_index_value;
}

void touchstone::State::set_provoking_vertex(GLenum value)
{
	this->provoking_vertex_value = value;
}

GLenum touchstone::State::get_provoking_vertex() const
{
	return this->provoking_vertex_value;
}

void touchstone::State::set_read_buffer(GLenum value)
{
	this->read_buffer_value = value;
}

GLenum touchstone::State::get_read_buffer() const
{
	return this->read_buffer_value;
}

void touchstone::State::set_sample_coverage(GLfloat value, GLboolean invert)
{
	this->sample_coverage_value = value;
	this->sample_coverage_invert_value = invert;
}

GLfloat touchstone::State::get_sample_coverage_value() const
{
	return this->sample_coverage_value;
}

GLboolean touchstone::State::get_sample_coverage_invert() const
{
	return this->sample_coverage_invert_value;
}

void touchstone::State::set_sample_mask(GLuint index, GLuint mask)
{
	this->sample_mask_values[index] = mask;
}

GLuint touchstone::State::get_sample_mask(GLuint index) const
{
	auto iter = this->sample_mask_values.find(index);
	if (iter != this->sample_mask_values.end())
	{
		return iter->second;
	}

	return 0xffffffff;
}

void touchstone::State::set_scissor(
	GLint x, GLint y, GLsizei width, GLsizei height)
{
	this->scissor_value_set = true;
	this->scissor_x_value = x;
	this->scissor_y_value = y;
	this->scissor_width_value = width;
	this->scissor_height_value = height;
}

bool touchstone::State::get_scissor_set() const
{
	return this->scissor_value_set;
}

GLint touchstone::State::get_scissor_x() const
{
	return this->scissor_x_value;
}

GLint touchstone::State::get_scissor_y() const
{
	return this->scissor_y_value;
}

GLsizei touchstone::State::get_scissor_width() const
{
	return this->scissor_width_value;
}

GLsizei touchstone::State::get_scissor_height() const
{
	return this->scissor_height_value;
}

void touchstone::State::set_stencil_func(
	GLenum face, GLenum func, GLint ref, GLuint mask)
{
	if (face == GL_FRONT_AND_BACK)
	{
		set_stencil_func(GL_FRONT, func, ref, mask);
		set_stencil_func(GL_BACK, func, ref, mask);
	}
	else
	{
		this->stencil_func_values[face] = std::make_tuple(func, ref, mask);
	}
}

GLenum touchstone::State::get_stencil_func(GLenum face) const
{
	auto iter = this->stencil_func_values.find(face);
	assert(iter != this->stencil_func_values.end());

	return std::get<0>(iter->second);
}

GLint touchstone::State::get_stencil_func_ref(GLenum face) const
{
	auto iter = this->stencil_func_values.find(face);
	assert(iter != this->stencil_func_values.end());

	return std::get<1>(iter->second);
}

GLuint touchstone::State::get_stencil_func_mask(GLenum face) const
{
	auto iter = this->stencil_func_values.find(face);
	assert(iter != this->stencil_func_values.end());

	return std::get<2>(iter->second);
}

void touchstone::State::set_stencil_mask(GLenum face, GLuint value)
{
	if (face == GL_FRONT_AND_BACK)
	{
		set_stencil_mask(GL_FRONT, value);
		set_stencil_mask(GL_BACK, value);
	}
	else
	{
		this->stencil_mask_values[face] = value;
	}
}

GLuint touchstone::State::get_stencil_mask(GLenum face) const
{
	auto iter = this->stencil_mask_values.find(face);
	assert(iter != this->stencil_mask_values.end());

	return iter->second;
}

void touchstone::State::set_stencil_op(
	GLenum face, GLenum stencil_fail, GLenum depth_fail, GLenum depth_pass)
{
	if (face == GL_FRONT_AND_BACK)
	{
		set_stencil_op(GL_FRONT, stencil_fail, depth_fail, depth_pass);
		set_stencil_op(GL_BACK, stencil_fail, depth_fail, depth_pass);
	}
	else
	{
		this->stencil_op_values[face] = std::make_tuple(stencil_fail, depth_fail, depth_pass);
	}
}

GLenum touchstone::State::get_stencil_op_stencil_fail(GLenum face) const
{
	auto iter = this->stencil_op_values.find(face);
	assert(iter != this->stencil_op_values.end());

	return std::get<0>(iter->second);
}

GLenum touchstone::State::get_stencil_op_depth_fail(GLenum face) const
{
	auto iter = this->stencil_op_values.find(face);
	assert(iter != this->stencil_op_values.end());

	return std::get<1>(iter->second);
}

GLenum touchstone::State::get_stencil_op_depth_pass(GLenum face) const
{
	auto iter = this->stencil_op_values.find(face);
	assert(iter != this->stencil_op_values.end());

	return std::get<2>(iter->second);
}

void touchstone::State::set_viewport(
	GLint x, GLint y, GLsizei width, GLsizei height)
{
	this->viewport_value_set = true;
	this->viewport_x_value = x;
	this->viewport_y_value = y;
	this->viewport_width_value = width;
	this->viewport_height_value = height;
}

bool touchstone::State::get_viewport_set() const
{
	return this->viewport_value_set;
}

GLint touchstone::State::get_viewport_x() const
{
	return this->viewport_x_value;
}

GLint touchstone::State::get_viewport_y() const
{
	return this->viewport_y_value;
}

GLsizei touchstone::State::get_viewport_width() const
{
	return this->viewport_width_value;
}

GLsizei touchstone::State::get_viewport_height() const
{
	return this->viewport_height_value;
}

void touchstone::State::enable(GLenum option)
{
	this->enable_values[option] = GL_TRUE;
}

void touchstone::State::enable(GLuint index, GLenum option)
{
	auto key = std::make_tuple(option, index);
	this->indexed_enable_values[key] = GL_TRUE;
}

void touchstone::State::disable(GLenum option)
{
	this->enable_values[option] = GL_FALSE;
}

void touchstone::State::disable(GLuint index, GLenum option)
{
	auto key = std::make_tuple(option, index);
	this->indexed_enable_values[key] = GL_FALSE;
}

bool touchstone::State::enabled(GLenum option) const
{
	auto iter = this->enable_values.find(option);
	if (iter != this->enable_values.end())
	{
		return iter->second;
	}

	return GL_FALSE;
}

bool touchstone::State::enabled(GLuint index, GLenum option) const
{
	auto key = std::make_tuple(option, index);
	auto iter = this->indexed_enable_values.find(key);
	if (iter != this->indexed_enable_values.end())
	{
		return iter->second;
	}

	return GL_FALSE;
}
