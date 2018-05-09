// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/trace/call.hpp"

void touchstone::Context::update(const Call* call)
{
	switch (call->get_token())
	{
#include "context.cpp.inl"
	}
}

void touchstone::Context::ActiveTexture(const call::ActiveTexture* call)
{
	this->texture_binding_manager.set_active_unit(call->get_parameter_texture());
}

void touchstone::Context::AttachShader(const call::AttachShader* call)
{
	auto& program = this->program_resource_manager.get(
		call->get_parameter_program());
	program.attach_shader(call->get_parameter_shader());
}

void touchstone::Context::BindAttribLocation(const call::BindAttribLocation* call)
{
	auto& program = this->program_resource_manager.get(
		call->get_parameter_program());
	program.set_attrib_location(call->get_parameter_name(), call->get_parameter_index());
}

void touchstone::Context::BindBuffer(const call::BindBuffer* call)
{
	set_current_resource<BufferResource>(
		call->get_parameter_buffer(), call->get_parameter_target());

	if (call->get_parameter_target() == GL_ELEMENT_ARRAY_BUFFER &&
		this->vertex_array_binding_manager.is_resource_bound())
	{
		auto& vertex_array = get_current_resource<VertexArrayResource>();
		vertex_array.set_element_array_buffer(call->get_parameter_buffer());
	}
}

void touchstone::Context::BindBufferBase(const call::BindBufferBase* call)
{
	set_current_resource<BufferResource>(
		call->get_parameter_buffer(), call->get_parameter_target(), call->get_parameter_index());
}

void touchstone::Context::BindBufferRange(const call::BindBufferRange* call)
{
	set_current_resource<BufferResource>(
		call->get_parameter_buffer(), call->get_parameter_target(), call->get_parameter_index());
}

void touchstone::Context::BindFragDataLocation(const call::BindFragDataLocation* call)
{
	auto& program = get_resource<ProgramResource>(call->get_parameter_program());
	program.set_frag_location(
		call->get_parameter_name(), call->get_parameter_color());
}

void touchstone::Context::BindFragDataLocationIndexed(const call::BindFragDataLocationIndexed* call)
{
	auto& program = this->program_resource_manager.get(
		call->get_parameter_program());
	program.set_frag_location(
		call->get_parameter_name(), call->get_parameter_color_number());
}

void touchstone::Context::BindFramebuffer(const call::BindFramebuffer* call)
{
	set_current_resource<FrameBufferResource>(
		call->get_parameter_framebuffer(), call->get_parameter_target());
}

void touchstone::Context::BindFramebufferEXT(const call::BindFramebufferEXT* call)
{
	set_current_resource<FrameBufferResource>(
		call->get_parameter_framebuffer(), call->get_parameter_target());
}

void touchstone::Context::BindRenderbuffer(const call::BindRenderbuffer* call)
{
	set_current_resource<RenderBufferResource>(
		call->get_parameter_renderbuffer());
}

void touchstone::Context::BindRenderbufferEXT(const call::BindRenderbufferEXT* call)
{
	set_current_resource<RenderBufferResource>(
		call->get_parameter_renderbuffer());
}

void touchstone::Context::BindTexture(const call::BindTexture* call)
{
	set_current_resource<TextureResource>(
		call->get_parameter_texture(), call->get_parameter_target());
}

void touchstone::Context::BindVertexArray(const call::BindVertexArray* call)
{
	set_current_resource<VertexArrayResource>(call->get_parameter_array());
}

void touchstone::Context::BlendColor(const call::BlendColor* call)
{
	this->state.set_blend_color(
		call->get_parameter_red(),
		call->get_parameter_green(),
		call->get_parameter_blue(),
		call->get_parameter_alpha());
}

void touchstone::Context::BlendEquation(const call::BlendEquation* call)
{
	this->state.set_blend_equation(
		call->get_parameter_mode(), call->get_parameter_mode());
}

void touchstone::Context::BlendEquationSeparate(const call::BlendEquationSeparate* call)
{
	this->state.set_blend_equation(
		call->get_parameter_mode_r_g_b(),
		call->get_parameter_mode_alpha());
}

void touchstone::Context::BlendFunc(const call::BlendFunc* call)
{
	this->state.set_blend_func(
		call->get_parameter_sfactor(), call->get_parameter_sfactor(),
		call->get_parameter_dfactor(), call->get_parameter_dfactor());
}

void touchstone::Context::BlendFuncSeparate(const call::BlendFuncSeparate* call)
{
	this->state.set_blend_func(
		call->get_parameter_sfactor_r_g_b(), call->get_parameter_sfactor_alpha(),
		call->get_parameter_dfactor_r_g_b(), call->get_parameter_dfactor_alpha());
}

void touchstone::Context::BlitFramebuffer(const call::BlitFramebuffer* call)
{
	assert(false);
}

void touchstone::Context::BufferData(const call::BufferData* call)
{
	auto& buffer = get_current_resource<BufferResource>(call->get_parameter_target());

	buffer.set_size(call->get_parameter_size());
}

void touchstone::Context::BufferSubData(const call::BufferSubData* call)
{
	// Nothing.
}

void touchstone::Context::ClampColor(const call::ClampColor* call)
{
	this->state.set_clamp_color(call->get_parameter_clamp());
}

void touchstone::Context::Clear(const call::Clear* call)
{
	// Nothing.
}

void touchstone::Context::ClearBufferfi(const call::ClearBufferfi* call)
{
	switch (call->get_parameter_buffer())
	{
		case GL_COLOR:
			break;
		case GL_DEPTH_STENCIL:
			break;
		case GL_STENCIL:
			break;
	}
}

void touchstone::Context::ClearBufferfv(const call::ClearBufferfv* call)
{
	switch (call->get_parameter_buffer())
	{
		case GL_COLOR:
			break;
		case GL_DEPTH:
			break;
	}
}

void touchstone::Context::ClearBufferiv(const call::ClearBufferiv* call)
{
	switch (call->get_parameter_buffer())
	{
		case GL_COLOR:
			break;
		case GL_DEPTH_STENCIL:
			break;
		case GL_DEPTH:
			break;
		case GL_STENCIL:
			break;
	}
}

void touchstone::Context::ClearBufferuiv(const call::ClearBufferuiv* call)
{
	switch (call->get_parameter_buffer())
	{
		case GL_COLOR:
			break;
		case GL_STENCIL:
			break;
	}
}

void touchstone::Context::ClearColor(const call::ClearColor* call)
{
	this->state.set_clear_color(
		call->get_parameter_red(),
		call->get_parameter_green(),
		call->get_parameter_blue(),
		call->get_parameter_alpha());
}

void touchstone::Context::ClearDepth(const call::ClearDepth* call)
{
	this->state.set_clear_depth(call->get_parameter_depth());
}

void touchstone::Context::ClearStencil(const call::ClearStencil* call)
{
	this->state.set_clear_stencil(call->get_parameter_s());
}

void touchstone::Context::ColorMask(const call::ColorMask* call)
{
	this->state.set_color_mask(
		call->get_parameter_red(),
		call->get_parameter_green(),
		call->get_parameter_blue(),
		call->get_parameter_alpha());
}

void touchstone::Context::ColorMaski(const call::ColorMaski* call)
{
	this->state.set_color_mask(
		call->get_parameter_index(),
		call->get_parameter_r(),
		call->get_parameter_g(),
		call->get_parameter_b(),
		call->get_parameter_a());
}

void touchstone::Context::CompileShader(const call::CompileShader* call)
{
	// Nothing.
}

void touchstone::Context::CompressedTexImage1D(const call::CompressedTexImage1D* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		call->get_parameter_level(),
		call->get_parameter_internal_format(),
		call->get_parameter_width(), 1, 1);
}

void touchstone::Context::CompressedTexImage2D(const call::CompressedTexImage2D* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		call->get_parameter_level(),
		call->get_parameter_internal_format(),
		call->get_parameter_width(), call->get_parameter_height(), 1);
}

void touchstone::Context::CompressedTexImage3D(const call::CompressedTexImage3D* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		call->get_parameter_level(),
		call->get_parameter_internal_format(),
		call->get_parameter_width(), call->get_parameter_height(), call->get_parameter_depth());
}

void touchstone::Context::CompressedTexSubImage1D(const call::CompressedTexSubImage1D* call)
{
	// Nothing.
}

void touchstone::Context::CompressedTexSubImage2D(const call::CompressedTexSubImage2D* call)
{
	// Nothing.
}

void touchstone::Context::CompressedTexSubImage3D(const call::CompressedTexSubImage3D* call)
{
	// Nothing.
}

void touchstone::Context::CopyBufferSubData(const call::CopyBufferSubData* call)
{
	// Nothing.
}

void touchstone::Context::CopyImageSubData(const call::CopyImageSubData* call)
{
	// Nothing.
}

void touchstone::Context::CopyTexImage1D(const call::CopyTexImage1D* call)
{
	// Nothing.
}

void touchstone::Context::CopyTexImage2D(const call::CopyTexImage2D* call)
{
	// Nothing.
}

void touchstone::Context::CopyTexSubImage1D(const call::CopyTexSubImage1D* call)
{
	// Nothing.
}

void touchstone::Context::CopyTexSubImage2D(const call::CopyTexSubImage2D* call)
{
	// Nothing.
}

void touchstone::Context::CopyTexSubImage3D(const call::CopyTexSubImage3D* call)
{
	// Nothing.
}

void touchstone::Context::CreateProgram(const call::CreateProgram* call)
{
	ProgramResource program(this, call->get_result());

	this->program_resource_manager.add(program);
}

void touchstone::Context::CreateShader(const call::CreateShader* call)
{
	ShaderResource shader(this, call->get_result(), call->get_parameter_type());

	this->shader_resource_manager.add(shader);
}

void touchstone::Context::CullFace(const call::CullFace* call)
{
	this->state.set_cull_face(call->get_parameter_mode());
}

void touchstone::Context::DeleteBuffers(const call::DeleteBuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_buffers()[i];
		this->buffer_binding_manager.unbind_all(name);
		this->buffer_resource_manager.remove(name);
	}
}

void touchstone::Context::DeleteFramebuffers(const call::DeleteFramebuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_framebuffers()[i];
		this->frame_buffer_binding_manager.unbind_all(name);
		this->frame_buffer_resource_manager.remove(name);
	}
}

void touchstone::Context::DeleteFramebuffersEXT(const call::DeleteFramebuffersEXT* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_framebuffers()[i];
		this->frame_buffer_binding_manager.unbind_all(name);
		this->frame_buffer_resource_manager.remove(name);
	}
}

void touchstone::Context::DeleteProgram(const call::DeleteProgram* call)
{
	auto name = call->get_parameter_program();
	this->program_binding_manager.unbind_all(name);
	this->program_resource_manager.remove(name);
}

void touchstone::Context::DeleteRenderbuffers(const call::DeleteRenderbuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_renderbuffers()[i];
		this->render_buffer_binding_manager.unbind_all(name);
		this->render_buffer_resource_manager.remove(name);
	}
}

void touchstone::Context::DeleteRenderbuffersEXT(const call::DeleteRenderbuffersEXT* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_renderbuffers()[i];
		this->render_buffer_binding_manager.unbind_all(name);
		this->render_buffer_resource_manager.remove(name);
	}
}

void touchstone::Context::DeleteShader(const call::DeleteShader* call)
{
	auto name = call->get_parameter_shader();
	this->shader_resource_manager.remove(name);
}

void touchstone::Context::DeleteTextures(const call::DeleteTextures* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_textures()[i];
		this->texture_binding_manager.unbind_all(name);
		this->texture_resource_manager.remove(name);
	}
}

void touchstone::Context::DeleteVertexArrays(const call::DeleteVertexArrays* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_arrays()[i];
		this->vertex_array_binding_manager.unbind_all(name);
		this->vertex_array_resource_manager.remove(name);
	}
}

void touchstone::Context::DepthFunc(const call::DepthFunc* call)
{
	this->state.set_depth_func(call->get_parameter_func());
}

void touchstone::Context::DepthMask(const call::DepthMask* call)
{
	this->state.set_depth_mask(call->get_parameter_flag());
}

void touchstone::Context::DepthRange(const call::DepthRange* call)
{
	this->state.set_depth_range(
		call->get_parameter_near(),
		call->get_parameter_far());
}

void touchstone::Context::DetachShader(const call::DetachShader* call)
{
	auto& program = this->program_resource_manager.get(call->get_parameter_program());
	program.detach_shader(call->get_parameter_shader());
}

void touchstone::Context::Disable(const call::Disable* call)
{
	this->state.disable(call->get_parameter_cap());
}

void touchstone::Context::DisableVertexAttribArray(const call::DisableVertexAttribArray* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.disable(call->get_parameter_index());
}

void touchstone::Context::Disablei(const call::Disablei* call)
{
	this->state.disable(
		call->get_parameter_index(),
		call->get_parameter_target());
}

void touchstone::Context::DrawArrays(const call::DrawArrays* call)
{
	// Nothing.
}

void touchstone::Context::DrawArraysInstanced(const call::DrawArraysInstanced* call)
{
	// Nothing.
}

void touchstone::Context::DrawBuffer(const call::DrawBuffer* call)
{
	this->state.set_draw_buffer(call->get_parameter_buf());
}

void touchstone::Context::DrawBuffers(const call::DrawBuffers* call)
{
	this->state.set_draw_buffers(
		call->get_parameter_n(),
		call->get_parameter_bufs());
}

void touchstone::Context::DrawElements(const call::DrawElements* call)
{
	// Nothing.
}

void touchstone::Context::DrawElementsBaseVertex(const call::DrawElementsBaseVertex* call)
{
	// Nothing.
}

void touchstone::Context::DrawElementsInstanced(const call::DrawElementsInstanced* call)
{
	// Nothing.
}

void touchstone::Context::DrawElementsInstancedBaseVertex(const call::DrawElementsInstancedBaseVertex* call)
{
	// Nothing.
}

void touchstone::Context::DrawRangeElements(const call::DrawRangeElements* call)
{
	// Nothing.
}

void touchstone::Context::DrawRangeElementsBaseVertex(const call::DrawRangeElementsBaseVertex* call)
{
	// Nothing.
}

void touchstone::Context::Enable(const call::Enable* call)
{
	this->state.enable(call->get_parameter_cap());
}

void touchstone::Context::EnableVertexAttribArray(const call::EnableVertexAttribArray* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.enable(call->get_parameter_index());
}

void touchstone::Context::Enablei(const call::Enablei* call)
{
	this->state.enable(
		call->get_parameter_index(),
		call->get_parameter_target());
}

void touchstone::Context::FlushMappedBufferRange(const call::FlushMappedBufferRange* call)
{
	//assert(false);
}

void touchstone::Context::FramebufferRenderbuffer(const call::FramebufferRenderbuffer* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_render_buffer(
		call->get_parameter_attachment(),
		call->get_parameter_renderbuffer());
}

void touchstone::Context::FramebufferRenderbufferEXT(const call::FramebufferRenderbufferEXT* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_render_buffer(
		call->get_parameter_attachment(),
		call->get_parameter_renderbuffer());
}

void touchstone::Context::FramebufferTexture(const call::FramebufferTexture* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), 0);
}

void touchstone::Context::FramebufferTexture1D(const call::FramebufferTexture1D* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), 0);
}

void touchstone::Context::FramebufferTexture1DEXT(const call::FramebufferTexture1DEXT* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), 0);
}

void touchstone::Context::FramebufferTexture2D(const call::FramebufferTexture2D* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), 0);
}

void touchstone::Context::FramebufferTexture2DEXT(const call::FramebufferTexture2DEXT* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), 0);
}

void touchstone::Context::FramebufferTexture3D(const call::FramebufferTexture3D* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), call->get_parameter_zoffset());
}

void touchstone::Context::FramebufferTexture3DEXT(const call::FramebufferTexture3DEXT* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), call->get_parameter_zoffset());
}

void touchstone::Context::FramebufferTextureLayer(const call::FramebufferTextureLayer* call)
{
	auto& frame_buffer = get_current_resource<FrameBufferResource>(
		call->get_parameter_target(), GL_DRAW_FRAMEBUFFER);
	frame_buffer.attach_texture(
		call->get_parameter_attachment(),
		call->get_parameter_texture(),
		call->get_parameter_level(), call->get_parameter_layer());
}

void touchstone::Context::FrontFace(const call::FrontFace* call)
{
	state.set_front_face(call->get_parameter_mode());
}

void touchstone::Context::GenBuffers(const call::GenBuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_buffers()[i];
		BufferResource buffer(this, name);
		this->buffer_resource_manager.add(buffer);
	}
}

void touchstone::Context::GenFramebuffers(const call::GenFramebuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_framebuffers()[i];
		FrameBufferResource frame_buffer(this, name);
		this->frame_buffer_resource_manager.add(frame_buffer);
	}
}

void touchstone::Context::GenFramebuffersEXT(const call::GenFramebuffersEXT* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_framebuffers()[i];
		FrameBufferResource frame_buffer(this, name);
		this->frame_buffer_resource_manager.add(frame_buffer);
	}
}

void touchstone::Context::GenRenderbuffers(const call::GenRenderbuffers* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_renderbuffers()[i];
		RenderBufferResource render_buffer(this, name);
		this->render_buffer_resource_manager.add(render_buffer);
	}
}

void touchstone::Context::GenRenderbuffersEXT(const call::GenRenderbuffersEXT* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_renderbuffers()[i];
		RenderBufferResource render_buffer(this, name);
		this->render_buffer_resource_manager.add(render_buffer);
	}
}

void touchstone::Context::GenTextures(const call::GenTextures* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_textures()[i];
		TextureResource texture(this, name);
		this->texture_resource_manager.add(texture);
	}
}

void touchstone::Context::GenVertexArrays(const call::GenVertexArrays* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		auto name = call->get_parameter_arrays()[i];
		VertexArrayResource vertex_array(this, name);
		this->vertex_array_resource_manager.add(vertex_array);
	}
}

void touchstone::Context::GetAttribLocation(const call::GetAttribLocation* call)
{
	auto& program = this->program_resource_manager.get(call->get_parameter_program());
	program.set_uniform_location(call->get_parameter_name(), call->get_result());
}

void touchstone::Context::GetFragDataLocation(const call::GetFragDataLocation* call)
{
	auto& program = this->program_resource_manager.get(call->get_parameter_program());
	program.set_frag_location(call->get_parameter_name(), call->get_result());
}

void touchstone::Context::GetUniformLocation(const call::GetUniformLocation* call)
{
	if (call->get_result() != -1)
	{
		auto& program = this->program_resource_manager.get(call->get_parameter_program());
		program.set_uniform_location(call->get_parameter_name(), call->get_result());
	}
}

void touchstone::Context::GetUniformBlockIndex(const call::GetUniformBlockIndex* call)
{
	if (call->get_result() != GL_INVALID_INDEX)
	{
		auto& program = this->program_resource_manager.get(call->get_parameter_program());
		program.set_uniform_block_name(call->get_result(), call->get_parameter_uniform_block_name());
	}
}

void touchstone::Context::LineWidth(const call::LineWidth* call)
{
	this->state.set_line_width(call->get_parameter_width());
}

void touchstone::Context::LinkProgram(const call::LinkProgram* call)
{
	// Nothing.
}

void touchstone::Context::LogicOp(const call::LogicOp* call)
{
	this->state.set_logic_op(call->get_parameter_opcode());
}

void touchstone::Context::MapBuffer(const call::MapBuffer* call)
{
	//assert(false);
}

void touchstone::Context::MapBufferRange(const call::MapBufferRange* call)
{
	//assert(false);
}

void touchstone::Context::MultiDrawArrays(const call::MultiDrawArrays* call)
{
	// Nothing.
}

void touchstone::Context::MultiDrawElements(const call::MultiDrawElements* call)
{
	// Nothing.
}

void touchstone::Context::MultiDrawElementsBaseVertex(const call::MultiDrawElementsBaseVertex* call)
{
	// Nothing.
}

void touchstone::Context::PixelStoref(const call::PixelStoref* call)
{
	this->state.set_pixel_store_parameter(
		call->get_parameter_pname(), call->get_parameter_param());
}

void touchstone::Context::PixelStorei(const call::PixelStorei* call)
{
	this->state.set_pixel_store_parameter(
		call->get_parameter_pname(), call->get_parameter_param());
}

void touchstone::Context::PointParameterf(const call::PointParameterf* call)
{
	// Nothing.
}

void touchstone::Context::PointParameterfv(const call::PointParameterfv* call)
{
	// Nothing.
}

void touchstone::Context::PointParameteri(const call::PointParameteri* call)
{
	// Nothing.
}

void touchstone::Context::PointParameteriv(const call::PointParameteriv* call)
{
	// Nothing.
}

void touchstone::Context::PointSize(const call::PointSize* call)
{
	this->state.set_point_size(call->get_parameter_size());
}

void touchstone::Context::PolygonMode(const call::PolygonMode* call)
{
	this->state.set_polygon_mode(call->get_parameter_face(), call->get_parameter_mode());
}

void touchstone::Context::PolygonOffset(const call::PolygonOffset* call)
{
	this->state.set_polygon_offset(
		call->get_parameter_factor(), call->get_parameter_units());
}

void touchstone::Context::PrimitiveRestartIndex(const call::PrimitiveRestartIndex* call)
{
	this->state.set_primitive_restart_index(call->get_parameter_index());
}

void touchstone::Context::ProvokingVertex(const call::ProvokingVertex* call)
{
	this->state.set_provoking_vertex(call->get_parameter_mode());
}

void touchstone::Context::ReadBuffer(const call::ReadBuffer* call)
{
	this->state.set_read_buffer(call->get_parameter_src());
}

void touchstone::Context::RenderbufferStorage(const call::RenderbufferStorage* call)
{
	auto& render_buffer = get_current_resource<RenderBufferResource>();
	render_buffer.storage(
		call->get_parameter_internal_format(), 0,
		call->get_parameter_width(), call->get_parameter_height());
}

void touchstone::Context::RenderbufferStorageEXT(const call::RenderbufferStorageEXT* call)
{
	auto& render_buffer = get_current_resource<RenderBufferResource>();
	render_buffer.storage(
		call->get_parameter_internal_format(), 0,
		call->get_parameter_width(), call->get_parameter_height());
}

void touchstone::Context::RenderbufferStorageMultisample(const call::RenderbufferStorageMultisample* call)
{
	auto& render_buffer = get_current_resource<RenderBufferResource>();
	render_buffer.storage(
		call->get_parameter_internal_format(), call->get_parameter_samples(),
		call->get_parameter_width(), call->get_parameter_height());
}

void touchstone::Context::SampleCoverage(const call::SampleCoverage* call)
{
	this->state.set_sample_coverage(
		call->get_parameter_value(),
		call->get_parameter_invert());
}

void touchstone::Context::SampleMaski(const call::SampleMaski* call)
{
	this->state.set_sample_mask(
		call->get_parameter_mask_number(),
		call->get_parameter_mask());
}

void touchstone::Context::Scissor(const call::Scissor* call)
{
	this->state.set_scissor(
		call->get_parameter_x(), call->get_parameter_y(),
		call->get_parameter_width(), call->get_parameter_height());
}

void touchstone::Context::ShaderSource(const call::ShaderSource* call)
{
	std::string source;
	for (auto i = 0; i < call->get_parameter_count_string(); ++i)
	{
		source += call->get_parameter_string(i);
	}

	auto& shader = get_resource<ShaderResource>(call->get_parameter_shader());
	shader.set_source(source);
}

void touchstone::Context::StencilFunc(const call::StencilFunc* call)
{
	this->state.set_stencil_func(
		GL_FRONT_AND_BACK,
		call->get_parameter_func(),
		call->get_parameter_ref(),
		call->get_parameter_mask());
}

void touchstone::Context::StencilFuncSeparate(const call::StencilFuncSeparate* call)
{
	this->state.set_stencil_func(
		call->get_parameter_face(),
		call->get_parameter_func(),
		call->get_parameter_ref(),
		call->get_parameter_mask());
}

void touchstone::Context::StencilMask(const call::StencilMask* call)
{
	this->state.set_stencil_mask(
		GL_FRONT_AND_BACK,
		call->get_parameter_mask());
}

void touchstone::Context::StencilMaskSeparate(const call::StencilMaskSeparate* call)
{
	this->state.set_stencil_mask(
		call->get_parameter_face(),
		call->get_parameter_mask());
}

void touchstone::Context::StencilOp(const call::StencilOp* call)
{
	this->state.set_stencil_op(
		GL_FRONT_AND_BACK,
		call->get_parameter_fail(),
		call->get_parameter_zfail(),
		call->get_parameter_zpass());
}

void touchstone::Context::StencilOpSeparate(const call::StencilOpSeparate* call)
{
	this->state.set_stencil_op(
		call->get_parameter_face(),
		call->get_parameter_sfail(),
		call->get_parameter_dpfail(),
		call->get_parameter_dppass());
}

void touchstone::Context::TexImage1D(const call::TexImage1D* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		call->get_parameter_level(),
		call->get_parameter_internal_format(),
		call->get_parameter_width(), 1, 1);
}

void touchstone::Context::TexImage2D(const call::TexImage2D* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		call->get_parameter_level(),
		call->get_parameter_internal_format(),
		call->get_parameter_width(), call->get_parameter_height(), 1);
}

void touchstone::Context::TexImage2DMultisample(const call::TexImage2DMultisample* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		0,
		call->get_parameter_internal_format(),
		call->get_parameter_width(), call->get_parameter_height(), 1);
}

void touchstone::Context::TexImage3D(const call::TexImage3D* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		call->get_parameter_level(),
		call->get_parameter_internal_format(),
		call->get_parameter_width(), call->get_parameter_height(), call->get_parameter_depth());
}

void touchstone::Context::TexImage3DMultisample(const call::TexImage3DMultisample* call)
{
	auto& texture = get_current_resource<TextureResource>(call->get_parameter_target());
	texture.create_level(
		call->get_parameter_target(),
		0,
		call->get_parameter_internal_format(),
		call->get_parameter_width(), call->get_parameter_height(), call->get_parameter_depth());
}

void touchstone::Context::TexParameterIiv(const call::TexParameterIiv* call)
{
	// Nothing.
}

void touchstone::Context::TexParameterIuiv(const call::TexParameterIuiv* call)
{
	// Nothing.
}

void touchstone::Context::TexParameterf(const call::TexParameterf* call)
{
	// Nothing.
}

void touchstone::Context::TexParameterfv(const call::TexParameterfv* call)
{
	// Nothing.
}

void touchstone::Context::TexParameteri(const call::TexParameteri* call)
{
	// Nothing.
}

void touchstone::Context::TexParameteriv(const call::TexParameteriv* call)
{
	// Nothing.
}

void touchstone::Context::TexSubImage1D(const call::TexSubImage1D* call)
{
	// Nothing.
}

void touchstone::Context::TexSubImage2D(const call::TexSubImage2D* call)
{
	// Nothing.
}

void touchstone::Context::TexSubImage3D(const call::TexSubImage3D* call)
{
	// Nothing.
}

void touchstone::Context::Uniform1f(const call::Uniform1f* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform1f(
		call->get_parameter_location(),
		call->get_parameter_v0());
}

void touchstone::Context::Uniform1fv(const call::Uniform1fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform1fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform1i(const call::Uniform1i* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform1i(
		call->get_parameter_location(),
		call->get_parameter_v0());
}

void touchstone::Context::Uniform1iv(const call::Uniform1iv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform1iv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform1ui(const call::Uniform1ui* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform1ui(
		call->get_parameter_location(),
		call->get_parameter_v0());
}

void touchstone::Context::Uniform1uiv(const call::Uniform1uiv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform1uiv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform2f(const call::Uniform2f* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform2f(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1());
}

void touchstone::Context::Uniform2fv(const call::Uniform2fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform2fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform2i(const call::Uniform2i* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform2i(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1());
}

void touchstone::Context::Uniform2iv(const call::Uniform2iv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform2iv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform2ui(const call::Uniform2ui* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform2ui(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1());
}

void touchstone::Context::Uniform2uiv(const call::Uniform2uiv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform2uiv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform3f(const call::Uniform3f* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform3f(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1(), call->get_parameter_v2());
}

void touchstone::Context::Uniform3fv(const call::Uniform3fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform3fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform3i(const call::Uniform3i* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform3i(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1(), call->get_parameter_v2());
}

void touchstone::Context::Uniform3iv(const call::Uniform3iv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform3iv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform3ui(const call::Uniform3ui* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform3ui(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1(), call->get_parameter_v2());
}

void touchstone::Context::Uniform3uiv(const call::Uniform3uiv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform3uiv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform4f(const call::Uniform4f* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform4f(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1(), call->get_parameter_v2(), call->get_parameter_v3());
}

void touchstone::Context::Uniform4fv(const call::Uniform4fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform4fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform4i(const call::Uniform4i* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform4i(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1(), call->get_parameter_v2(), call->get_parameter_v3());
}

void touchstone::Context::Uniform4iv(const call::Uniform4iv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform4iv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::Uniform4ui(const call::Uniform4ui* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform4ui(
		call->get_parameter_location(),
		call->get_parameter_v0(), call->get_parameter_v1(), call->get_parameter_v2(), call->get_parameter_v3());
}

void touchstone::Context::Uniform4uiv(const call::Uniform4uiv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform4uiv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_value());
}

void touchstone::Context::UniformBlockBinding(const call::UniformBlockBinding* call)
{
	auto& program = get_resource<ProgramResource>(call->get_parameter_program());
	program.set_uniform_block_binding(
		call->get_parameter_uniform_block_index(),
		call->get_parameter_uniform_block_binding());
}

void touchstone::Context::UniformMatrix2fv(const call::UniformMatrix2fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix2fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix2x3fv(const call::UniformMatrix2x3fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix2x3fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix2x4fv(const call::UniformMatrix2x4fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix2x4fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix3fv(const call::UniformMatrix3fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix3fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix3x2fv(const call::UniformMatrix3x2fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix3x2fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix3x4fv(const call::UniformMatrix3x4fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix3x4fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix4fv(const call::UniformMatrix4fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix4fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix4x2fv(const call::UniformMatrix4x2fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix4x2fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UniformMatrix4x3fv(const call::UniformMatrix4x3fv* call)
{
	auto& program = get_current_resource<ProgramResource>();
	program.set_uniform_matrix4x3fv(
		call->get_parameter_location(),
		call->get_parameter_count(),
		call->get_parameter_transpose(),
		call->get_parameter_value());
}

void touchstone::Context::UnmapBuffer(const call::UnmapBuffer* call)
{
	//assert(false);
}

void touchstone::Context::UseProgram(const call::UseProgram* call)
{
	set_current_resource<ProgramResource>(call->get_parameter_program());
}

void touchstone::Context::VertexAttrib1d(const call::VertexAttrib1d* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_DOUBLE, GL_FALSE, false, call->get_parameter_x(), 0.0, 0.0, 0.0);
}

void touchstone::Context::VertexAttrib1dv(const call::VertexAttrib1dv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_DOUBLE, GL_FALSE, false, call->get_parameter_v()[0], 0.0, 0.0, 0.0);
}

void touchstone::Context::VertexAttrib1f(const call::VertexAttrib1f* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_FLOAT, GL_FALSE, false, call->get_parameter_x(), 0.0f, 0.0f, 0.0f);
}

void touchstone::Context::VertexAttrib1fv(const call::VertexAttrib1fv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_FLOAT, GL_FALSE, false, call->get_parameter_v()[0], 0.0f, 0.0f, 0.0f);
}

void touchstone::Context::VertexAttrib1s(const call::VertexAttrib1s* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_SHORT, GL_FALSE, false, call->get_parameter_x(), 0, 0, 0);
}

void touchstone::Context::VertexAttrib1sv(const call::VertexAttrib1sv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_SHORT, GL_FALSE, false, call->get_parameter_v()[0], 0, 0, 0);
}

void touchstone::Context::VertexAttrib2d(const call::VertexAttrib2d* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_DOUBLE, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), 0.0, 0.0);
}

void touchstone::Context::VertexAttrib2dv(const call::VertexAttrib2dv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_DOUBLE, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], 0.0, 0.0);
}

void touchstone::Context::VertexAttrib2f(const call::VertexAttrib2f* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_FLOAT, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), 0.0f, 0.0f);
}

void touchstone::Context::VertexAttrib2fv(const call::VertexAttrib2fv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_FLOAT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], 0.0f, 0.0f);
}

void touchstone::Context::VertexAttrib2s(const call::VertexAttrib2s* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_SHORT, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), 0, 0);
}

void touchstone::Context::VertexAttrib2sv(const call::VertexAttrib2sv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_SHORT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], 0, 0);
}

void touchstone::Context::VertexAttrib3d(const call::VertexAttrib3d* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_DOUBLE, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), 0.0);
}

void touchstone::Context::VertexAttrib3dv(const call::VertexAttrib3dv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_DOUBLE, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], 0.0);
}

void touchstone::Context::VertexAttrib3f(const call::VertexAttrib3f* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_FLOAT, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), 0.0f);
}

void touchstone::Context::VertexAttrib3fv(const call::VertexAttrib3fv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_FLOAT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], 0.0f);
}

void touchstone::Context::VertexAttrib3s(const call::VertexAttrib3s* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_SHORT, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), 0);
}

void touchstone::Context::VertexAttrib3sv(const call::VertexAttrib3sv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_SHORT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], 0);
}

void touchstone::Context::VertexAttrib4Nbv(const call::VertexAttrib4Nbv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_BYTE, GL_TRUE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4Niv(const call::VertexAttrib4Niv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_INT, GL_TRUE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4Nsv(const call::VertexAttrib4Nsv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_SHORT, GL_TRUE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4Nub(const call::VertexAttrib4Nub* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_BYTE, GL_TRUE, false, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), call->get_parameter_w());
}

void touchstone::Context::VertexAttrib4Nubv(const call::VertexAttrib4Nubv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_BYTE, GL_TRUE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4Nuiv(const call::VertexAttrib4Nuiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_INT, GL_TRUE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4Nusv(const call::VertexAttrib4Nusv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_SHORT, GL_TRUE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4bv(const call::VertexAttrib4bv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_BYTE, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4d(const call::VertexAttrib4d* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_DOUBLE, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), call->get_parameter_w());
}

void touchstone::Context::VertexAttrib4dv(const call::VertexAttrib4dv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_DOUBLE, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4f(const call::VertexAttrib4f* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_FLOAT, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), call->get_parameter_w());
}

void touchstone::Context::VertexAttrib4fv(const call::VertexAttrib4fv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_FLOAT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4iv(const call::VertexAttrib4iv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_INT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4s(const call::VertexAttrib4s* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_SHORT, GL_FALSE, false, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), call->get_parameter_w());
}

void touchstone::Context::VertexAttrib4sv(const call::VertexAttrib4sv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_SHORT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4ubv(const call::VertexAttrib4ubv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_BYTE, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4uiv(const call::VertexAttrib4uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_INT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttrib4usv(const call::VertexAttrib4usv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_SHORT, GL_FALSE, false, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttribDivisor(const call::VertexAttribDivisor* call)
{
	//assert(false);
}

void touchstone::Context::VertexAttribI1i(const call::VertexAttribI1i* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_INT, GL_FALSE, true, call->get_parameter_x(), 0, 0, 0);
}

void touchstone::Context::VertexAttribI1iv(const call::VertexAttribI1iv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_INT, GL_FALSE, true, call->get_parameter_v()[0], 0, 0, 0);
}

void touchstone::Context::VertexAttribI1ui(const call::VertexAttribI1ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_x(), 0u, 0u, 0u);
}

void touchstone::Context::VertexAttribI1uiv(const call::VertexAttribI1uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_v()[0], 0u, 0u, 0u);
}

void touchstone::Context::VertexAttribI2i(const call::VertexAttribI2i* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_INT, GL_FALSE, true, call->get_parameter_x(), call->get_parameter_y(), 0, 0);
}

void touchstone::Context::VertexAttribI2iv(const call::VertexAttribI2iv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_INT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], 0, 0);
}

void touchstone::Context::VertexAttribI2ui(const call::VertexAttribI2ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_x(), call->get_parameter_y(), 0u, 0u);
}

void touchstone::Context::VertexAttribI2uiv(const call::VertexAttribI2uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], 0u, 0u);
}

void touchstone::Context::VertexAttribI3i(const call::VertexAttribI3i* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_INT, GL_FALSE, true, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), 0);
}

void touchstone::Context::VertexAttribI3iv(const call::VertexAttribI3iv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_INT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], 0);
}

void touchstone::Context::VertexAttribI3ui(const call::VertexAttribI3ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), 0u);
}

void touchstone::Context::VertexAttribI3uiv(const call::VertexAttribI3uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], 0u);
}

void touchstone::Context::VertexAttribI4bv(const call::VertexAttribI4bv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, GL_BYTE, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttribI4i(const call::VertexAttribI4i* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_INT, GL_FALSE, true, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), call->get_parameter_w());
}

void touchstone::Context::VertexAttribI4iv(const call::VertexAttribI4iv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_INT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttribI4sv(const call::VertexAttribI4sv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_SHORT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttribI4ubv(const call::VertexAttribI4ubv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_BYTE, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttribI4ui(const call::VertexAttribI4ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_x(), call->get_parameter_y(), call->get_parameter_z(), call->get_parameter_w());
}

void touchstone::Context::VertexAttribI4uiv(const call::VertexAttribI4uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_INT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttribI4usv(const call::VertexAttribI4usv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, GL_UNSIGNED_SHORT, GL_FALSE, true, call->get_parameter_v()[0], call->get_parameter_v()[1], call->get_parameter_v()[2], call->get_parameter_v()[3]);
}

void touchstone::Context::VertexAttribIPointer(const call::VertexAttribIPointer* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_pointer(
		call->get_parameter_index(),
		call->get_parameter_size(), call->get_parameter_type(), GL_FALSE, true,
		call->get_parameter_stride(), (std::size_t)call->get_parameter_pointer());
}

void touchstone::Context::VertexAttribP1ui(const call::VertexAttribP1ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribP1uiv(const call::VertexAttribP1uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 1, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribP2ui(const call::VertexAttribP2ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribP2uiv(const call::VertexAttribP2uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 2, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribP3ui(const call::VertexAttribP3ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribP3uiv(const call::VertexAttribP3uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 3, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribP4ui(const call::VertexAttribP4ui* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribP4uiv(const call::VertexAttribP4uiv* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_value(
		call->get_parameter_index(), 4, call->get_parameter_type(), call->get_parameter_normalized());
}

void touchstone::Context::VertexAttribPointer(const call::VertexAttribPointer* call)
{
	auto& vertex_array = get_current_resource<VertexArrayResource>();
	vertex_array.set_vertex_pointer(
		call->get_parameter_index(),
		call->get_parameter_size(), call->get_parameter_type(), call->get_parameter_normalized(), false,
		call->get_parameter_stride(), (std::size_t)call->get_parameter_pointer());
}

void touchstone::Context::Viewport(const call::Viewport* call)
{
	this->state.set_viewport(
		call->get_parameter_x(), call->get_parameter_y(),
		call->get_parameter_width(), call->get_parameter_height());
}
