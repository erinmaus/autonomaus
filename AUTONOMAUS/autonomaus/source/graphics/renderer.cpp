// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/graphics/renderer.hpp"

autonomaus::Renderer::Renderer(touchstone::ImmediateDispatch& dispatch)
{
	this->dispatch = &dispatch;
}

void autonomaus::Renderer::set_viewport(int x, int y, int width, int height)
{
	this->dispatch->Viewport(x, y, width, height);
}

void autonomaus::Renderer::begin()
{
	this->style_stack.clear();
	this->style_stack.emplace_back();

	this->dispatch->GetIntegerv(GL_CURRENT_PROGRAM, &this->original_program);
	this->current_program = this->original_program;

	this->dispatch->GetIntegerv(GL_ARRAY_BUFFER_BINDING, &this->original_array_buffer);
	this->current_array_buffer = this->original_array_buffer;

	this->dispatch->GetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING, &this->original_element_array_buffer);
	this->current_element_array_buffer = this->original_element_array_buffer;

	this->dispatch->GetIntegerv(GL_VERTEX_ARRAY_BINDING, &this->original_vertex_array);
	this->current_vertex_array = this->original_vertex_array;

 	this->dispatch->GetIntegerv(GL_ACTIVE_TEXTURE, &this->active_unit);
	this->dispatch->GetIntegerv(GL_TEXTURE_BINDING_2D, &this->original_texture);
	this->current_texture = this->original_texture;

	this->dispatch->GetBooleanv(GL_DEPTH_TEST, &this->original_depth_testing_enabled);
	this->dispatch->GetIntegerv(GL_DEPTH_FUNC, &this->original_depth_testing_func);

	this->dispatch->GetBooleanv(GL_BLEND, &this->original_blend_enabled);
	this->dispatch->GetIntegerv(GL_BLEND_DST_ALPHA, &this->original_blend_mode_destination_alpha);
	this->dispatch->GetIntegerv(GL_BLEND_DST_RGB, &this->original_blend_mode_destination_color);
	this->dispatch->GetIntegerv(GL_BLEND_SRC_ALPHA, &this->original_blend_mode_source_alpha);
	this->dispatch->GetIntegerv(GL_BLEND_SRC_RGB, &this->original_blend_mode_source_color);

	this->dispatch->GetBooleanv(GL_SCISSOR_TEST, &this->original_scissor_testing_enabled);
	this->dispatch->GetIntegerv(GL_SCISSOR_BOX, this->original_scissor_rect);

	this->dispatch->GetIntegerv(GL_VIEWPORT, this->original_viewport);

	this->dispatch->GetBooleanv(GL_CULL_FACE, &this->original_cull_face_enabled);
	this->dispatch->GetBooleanv(GL_STENCIL_TEST, &this->original_stencil_test_enabled);

	this->dispatch->GetFloatv(GL_LINE_WIDTH, &this->original_line_width);

	this->dispatch->Enable(GL_DEPTH_TEST);
	this->dispatch->DepthFunc(GL_LESS);
	this->dispatch->Enable(GL_BLEND);
	this->dispatch->BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
	this->dispatch->Disable(GL_SCISSOR_TEST);
	this->dispatch->LineWidth(1.0f);
	this->dispatch->Disable(GL_CULL_FACE);
}

void autonomaus::Renderer::end()
{
	this->dispatch->BindVertexArray(this->original_vertex_array);
	if (this->original_vertex_array != 0)
	{
		this->dispatch->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, this->original_element_array_buffer);
	}
	this->dispatch->BindBuffer(GL_ARRAY_BUFFER, this->original_array_buffer);
	this->dispatch->UseProgram(this->original_program);
	this->dispatch->BindTexture(GL_TEXTURE_2D, this->original_texture);

	if (this->original_depth_testing_enabled)
	{
		this->dispatch->Enable(GL_DEPTH_TEST);
	}
	else
	{
		this->dispatch->Disable(GL_DEPTH_TEST);
	}
	this->dispatch->DepthFunc(this->original_depth_testing_func);

	if (this->original_blend_enabled)
	{
		this->dispatch->Enable(GL_BLEND);
	}
	else
	{
		this->dispatch->Disable(GL_BLEND);
	}
	this->dispatch->BlendFuncSeparate(
		this->original_blend_mode_source_color,
		this->original_blend_mode_destination_color,
		this->original_blend_mode_source_alpha,
		this->original_blend_mode_destination_alpha);

	if (this->original_scissor_testing_enabled)
	{
		this->dispatch->Enable(GL_SCISSOR_TEST);
	}
	else
	{
		this->dispatch->Disable(GL_SCISSOR_TEST);
	}
	this->dispatch->Scissor(
		this->original_scissor_rect[0],
		this->original_scissor_rect[1],
		this->original_scissor_rect[2],
		this->original_scissor_rect[3]);

	this->dispatch->Viewport(
		this->original_viewport[0],
		this->original_viewport[1],
		this->original_viewport[2],
		this->original_viewport[3]);

	this->dispatch->LineWidth(this->original_line_width);

	if (this->original_cull_face_enabled)
	{
		this->dispatch->Enable(GL_CULL_FACE);
	}
	else
	{
		this->dispatch->Disable(GL_CULL_FACE);
	}

	if (this->original_stencil_test_enabled)
	{
		this->dispatch->Enable(GL_STENCIL_TEST);
	}
	else
	{
		this->dispatch->Disable(GL_STENCIL_TEST);
	}
}

void autonomaus::Renderer::push_style()
{
	this->style_stack.push_back(this->style_stack.back());
}

void autonomaus::Renderer::pop_style()
{
	this->style_stack.pop_back();
	auto& top = this->style_stack.back();

	set_blend_mode(top.blend_mode);

	if (top.enable_depth_testing)
	{
		enable_depth_testing();
	}
	else
	{
		disable_depth_testing();
	}

	if (top.scissor_set)
	{
		set_scissor(
			top.scissor_x, top.scissor_y,
			top.scissor_width, top.scissor_height);
	}

	set_line_thickness(top.line_thickness);
}

void autonomaus::Renderer::set_blend_mode(int mode)
{
	auto& top = this->style_stack.back();
	if (mode != top.blend_mode)
	{
		switch (mode)
		{
			case blend_none:
				this->dispatch->Disable(GL_BLEND);
				break;
			case blend_alpha:
				this->dispatch->Enable(GL_BLEND);
				this->dispatch->BlendFunc(GL_ONE, GL_ONE_MINUS_SRC_ALPHA);
				break;
			case blend_additive:
				this->dispatch->Enable(GL_BLEND);
				this->dispatch->BlendFunc(GL_ONE, GL_ONE);
				break;
			default:
				return;
		}

		top.blend_mode = mode;
	}
}

void autonomaus::Renderer::enable_depth_testing()
{
	auto& top = this->style_stack.back();
	this->dispatch->Enable(GL_DEPTH_TEST);
	top.enable_depth_testing = true;
}

void autonomaus::Renderer::disable_depth_testing()
{
	auto& top = this->style_stack.back();
	this->dispatch->Disable(GL_DEPTH_TEST);
	top.enable_depth_testing = false;
}

void autonomaus::Renderer::set_scissor(int x, int y, int width, int height)
{
	auto& top = this->style_stack.back();
	this->dispatch->Enable(GL_SCISSOR_TEST);
	top.scissor_set = true;
	this->dispatch->Scissor(x, y, width, height);
	top.scissor_x = x;
	top.scissor_y = y;
	top.scissor_width = width;
	top.scissor_height = height;
}

void autonomaus::Renderer::unset_scissor()
{
	auto& top = this->style_stack.back();
	this->dispatch->Disable(GL_SCISSOR_TEST);
	top.scissor_set = false;
}

void autonomaus::Renderer::set_line_thickness(float value)
{
	auto& top = this->style_stack.back();
	this->dispatch->LineWidth(value);
	top.line_thickness = value;
}

GLuint autonomaus::Renderer::get_active_unit()
{
	return this->active_unit - GL_TEXTURE0;
}

void autonomaus::Renderer::use_program(GLuint value)
{
	if (this->current_program != value)
	{
		this->dispatch->UseProgram(value);
		this->current_program = value;
	}
}
 	
void autonomaus::Renderer::bind_element_array_buffer(GLuint value)
{
	if (this->current_element_array_buffer != value)
	{
		this->dispatch->BindBuffer(GL_ELEMENT_ARRAY_BUFFER, value);
		this->current_element_array_buffer = value;
	}
}

void autonomaus::Renderer::bind_array_buffer(GLuint value)
{
	if (this->current_array_buffer != value)
	{
		this->dispatch->BindBuffer(GL_ARRAY_BUFFER, value);
		this->current_array_buffer = value;
	}
}

void autonomaus::Renderer::bind_vertex_array(GLuint value)
{
	if (this->current_vertex_array != value)
	{
		this->dispatch->BindVertexArray(value);
		this->current_vertex_array = value;
		this->current_element_array_buffer = 0;
	}
}

void autonomaus::Renderer::bind_texture(GLuint value)
{
	if (this->current_texture != value)
	{
		this->dispatch->BindTexture(GL_TEXTURE_2D, value);
		this->current_texture = value;
	}
}
