// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "rosalind/processors/textureProcessor.hpp"

rosalind::TextureProcessor::TextureProcessor(const touchstone::Context* context)
{
	this->context = context;
}

void rosalind::TextureProcessor::after_update(const touchstone::Call* call)
{
	switch (call->get_token())
	{
		case touchstone::call::Clear::token:
			process_clear_after((const touchstone::call::Clear*)call);
			break;
		case touchstone::call::BlitFramebuffer::token:
			process_blit_framebuffer_after((const touchstone::call::BlitFramebuffer*)call);
			break;
		case touchstone::call::BlitFramebufferEXT::token:
			process_blit_framebuffer_after((const touchstone::call::BlitFramebufferEXT*)call);
			break;
		case touchstone::call::GenTextures::token:
			process_create_textures_after((const touchstone::call::GenTextures*)call);
			break;
		case touchstone::call::DeleteTextures::token:
			process_delete_textures_after((const touchstone::call::DeleteTextures*)call);
			break;
		case touchstone::call::TexImage2D::token:
			process_update_texture_after((const touchstone::call::TexImage2D*)call);
			break;
		case touchstone::call::TexSubImage2D::token:
			process_update_texture_after((const touchstone::call::TexSubImage2D*)call);
			break;
		case touchstone::call::CompressedTexImage2D::token:
			process_update_texture_after((const touchstone::call::CompressedTexImage2D*)call);
			break;
		case touchstone::call::CompressedTexSubImage2D::token:
			process_update_texture_after((const touchstone::call::CompressedTexSubImage2D*)call);
			break;
	}
}

void rosalind::TextureProcessor::before_update(const touchstone::Call* call)
{
	switch (call->get_token())
	{
		case touchstone::call::TexImage2D::token:
			process_update_texture_before((const touchstone::call::TexImage2D*)call);
			break;
		case touchstone::call::TexImage2DMultisample::token:
			process_update_texture_before((const touchstone::call::TexImage2DMultisample*)call);
			break;
		case touchstone::call::CompressedTexImage2D::token:
			process_update_texture_before((const touchstone::call::CompressedTexImage2D*)call);
			break;
		case touchstone::call::CopyTexImage2D::token:
			process_copy_texture_before((const touchstone::call::CopyTexImage2D*)call);
			break;
		case touchstone::call::CopyTexSubImage2D::token:
			process_copy_texture_before((const touchstone::call::CopyTexSubImage2D*)call);
			break;
		case touchstone::call::CopyImageSubData::token:
			process_copy_texture_before((const touchstone::call::CopyImageSubData*)call);
			break;
	}
}

void rosalind::TextureProcessor::process_clear_after(const touchstone::call::Clear* call)
{
	if (!(call->get_parameter_mask() & GL_COLOR_BUFFER_BIT))
	{
		return;
	}

	if (!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_READ_FRAMEBUFFER) ||
		!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		return;
	}

	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	if (!frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0) || !frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
	{
		return;
	}

	kompvter::TextureEvent event;
	event.set_name(frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));

	notify(std::mem_fn(&kompvter::TextureListener::on_texture_clear), event);
}

void rosalind::TextureProcessor::process_blit_framebuffer_after(
	const touchstone::call::BlitFramebuffer* call)
{
	if (!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_READ_FRAMEBUFFER) ||
		!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		return;
	}

	auto& draw_frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	if (!draw_frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0) || !draw_frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
	{
		return;
	}

	auto& read_frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_READ_FRAMEBUFFER);
	if (!read_frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0) || !read_frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
	{
		return;
	}

	auto& source_texture = this->context->get_resource<touchstone::TextureResource>(read_frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));
	auto& destination_texture = this->context->get_resource<touchstone::TextureResource>(draw_frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));
	process_texture_copy_before(
		source_texture.get_name(), destination_texture.get_name(),
		call->get_parameter_src_x0(), call->get_parameter_src_y0(),
		call->get_parameter_dst_x0(), call->get_parameter_dst_y0(),
		call->get_parameter_dst_x1() - call->get_parameter_dst_x0(),
		call->get_parameter_dst_y1() - call->get_parameter_dst_y0());
}

void rosalind::TextureProcessor::process_blit_framebuffer_after(
	const touchstone::call::BlitFramebufferEXT* call)
{
	if (!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_READ_FRAMEBUFFER) ||
		!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		return;
	}

	auto& draw_frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	if (!draw_frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0) || !draw_frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
	{
		return;
	}

	auto& read_frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_READ_FRAMEBUFFER);
	if (!read_frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0) || !read_frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
	{
		return;
	}

	auto& source_texture = this->context->get_resource<touchstone::TextureResource>(read_frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));
	auto& destination_texture = this->context->get_resource<touchstone::TextureResource>(draw_frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));
	process_texture_copy_before(
		source_texture.get_name(), destination_texture.get_name(),
		call->get_parameter_src_x0(), call->get_parameter_src_y0(),
		call->get_parameter_dst_x0(), call->get_parameter_dst_y0(),
		call->get_parameter_dst_x1() - call->get_parameter_dst_x0(),
		call->get_parameter_dst_y1() - call->get_parameter_dst_y0());
}

void rosalind::TextureProcessor::process_create_textures_after(
	const touchstone::call::GenTextures* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		kompvter::TextureEvent event;
		event.set_name(call->get_parameter_textures()[i]);

		notify(std::mem_fn(&kompvter::TextureListener::on_texture_create), event);
	}
}

void rosalind::TextureProcessor::process_delete_textures_after(
	const touchstone::call::DeleteTextures* call)
{
	for (GLsizei i = 0; i < call->get_parameter_n(); ++i)
	{
		kompvter::TextureEvent event;
		event.set_name(call->get_parameter_textures()[i]);

		notify(std::mem_fn(&kompvter::TextureListener::on_texture_delete), event);
	}
}

void rosalind::TextureProcessor::process_update_texture_before(
	const touchstone::call::TexImage2D* call)
{
	if (call->get_parameter_level() == 0)
	{
		auto texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
		process_update_texture_before(
			texture.get_name(),
			call->get_parameter_format(), call->get_parameter_type(),
			0, 0,
			call->get_parameter_width(), call->get_parameter_height(),
			call->get_parameter_pixels());
	}
}

void rosalind::TextureProcessor::process_update_texture_before(
	const touchstone::call::TexImage2DMultisample* call)
{
	auto texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
	process_update_texture_before(
		texture.get_name(),
		call->get_parameter_internal_format(), GL_NONE,
		0, 0,
		call->get_parameter_width(), call->get_parameter_height(),
		nullptr);
}

void rosalind::TextureProcessor::process_update_texture_before(
	const touchstone::call::CompressedTexImage2D* call)
{
	if (call->get_parameter_level() == 0)
	{
		auto texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
		process_update_texture_before(
			texture.get_name(),
			call->get_parameter_internal_format(), GL_NONE,
			0, 0,
			call->get_parameter_width(), call->get_parameter_height(),
			call->get_parameter_data());
	}
}

void rosalind::TextureProcessor::process_update_texture_before(
	GLuint name,
	GLenum component_format, GLenum component_type,
	GLint x, GLint y,
	GLsizei width, GLsizei height,
	const void* pixels)
{
	auto& texture = this->context->get_resource<touchstone::TextureResource>(name);

	kompvter::TextureEvent event;
	event.set_name(name);
	event.set_width(width);
	event.set_height(height);

	if (!gl_format_type_to_kompvter(component_format, component_type, event) &&
		!compressed_gl_format_to_kompvter(component_format, event))
	{
		return;
	}

	bool is_zero_offset = (x == 0 && y == 0);
	bool is_same_dimensions = (width == texture.get_width() && height == texture.get_height());
	if ((is_zero_offset && !is_same_dimensions) || pixels == nullptr)
	{
		notify(std::mem_fn(&kompvter::TextureListener::on_texture_allocate), event);
	}
}

void rosalind::TextureProcessor::process_update_texture_after(
	const touchstone::call::TexImage2D* call)
{
	if (call->get_parameter_level() == 0)
	{
		auto texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
		process_update_texture_after(
			texture.get_name(),
			call->get_parameter_format(), call->get_parameter_type(),
			0, 0,
			call->get_parameter_width(), call->get_parameter_height(),
			call->get_parameter_size_pixels(), call->get_parameter_pixels());
	}
}

void rosalind::TextureProcessor::process_update_texture_after(
	const touchstone::call::TexSubImage2D* call)
{
	if (call->get_parameter_level() == 0)
	{
		auto texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
		process_update_texture_after(
			texture.get_name(),
			call->get_parameter_format(), call->get_parameter_type(),
			call->get_parameter_xoffset(), call->get_parameter_yoffset(),
			call->get_parameter_width(), call->get_parameter_height(),
			call->get_parameter_size_pixels(), call->get_parameter_pixels());
	}
}

void rosalind::TextureProcessor::process_update_texture_after(
	const touchstone::call::CompressedTexImage2D* call)
{
	if (call->get_parameter_level() == 0)
	{
		auto texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
		process_update_texture_after(
			texture.get_name(),
			call->get_parameter_internal_format(), GL_NONE,
			0, 0,
			call->get_parameter_width(), call->get_parameter_height(),
			call->get_parameter_size_data(), call->get_parameter_data());
	}
}

void rosalind::TextureProcessor::process_update_texture_after(
	const touchstone::call::CompressedTexSubImage2D* call)
{
	if (call->get_parameter_level() == 0)
	{
		auto texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
		process_update_texture_after(
			texture.get_name(),
			call->get_parameter_format(), GL_NONE,
			call->get_parameter_xoffset(), call->get_parameter_yoffset(),
			call->get_parameter_width(), call->get_parameter_height(),
			call->get_parameter_size_data(), call->get_parameter_data());
	}
}

void rosalind::TextureProcessor::process_update_texture_after(
	GLuint name,
	GLenum component_format, GLenum component_type,
	GLint x, GLint y,
	GLsizei width, GLsizei height,
	GLsizei pixels_size, const void* pixels)
{
	if (pixels == nullptr)
	{
		return;
	}

	kompvter::TextureEvent event;
	if (!gl_format_type_to_kompvter(component_format, component_type, event) &&
		!compressed_gl_format_to_kompvter(component_format, event))
	{
		return;
	}

	event.set_x(x);
	event.set_y(y);
	event.set_name(name);
	event.set_width(width);
	event.set_height(height);
	event.set_pixels_data((const std::uint8_t *)pixels);
	event.set_pixels_data_size(pixels_size);

	notify(std::mem_fn(&kompvter::TextureListener::on_texture_update), event);
}

void rosalind::TextureProcessor::process_copy_texture_before(
	const touchstone::call::CopyTexImage2D* call)
{
	if (call->get_parameter_level() != 0)
	{
		return;
	}

	if (!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_READ_FRAMEBUFFER))
	{
		return;
	}

	auto attachment = this->context->get_state().get_read_buffer();
	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_READ_FRAMEBUFFER);
	if (!frame_buffer.has_attachment(attachment) || !frame_buffer.is_texture_attachment(attachment))
	{
		attachment = GL_COLOR_ATTACHMENT0;
		if (!frame_buffer.has_attachment(attachment) || !frame_buffer.is_texture_attachment(attachment))
		{
			return;
		}
	}

	auto& source_texture = this->context->get_resource<touchstone::TextureResource>(frame_buffer.get_attachment_name(attachment));
	auto& destination_texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
	if (destination_texture.get_width() != call->get_parameter_width() ||
		destination_texture.get_height() != call->get_parameter_height() ||
		destination_texture.get_internal_format() != call->get_parameter_internal_format())
	{
		kompvter::TextureEvent event;
		if (!gl_internal_format_to_kompvter(call->get_parameter_internal_format(), event))
		{
			return;
		}

		event.set_name(destination_texture.get_name());
		event.set_width(call->get_parameter_width());
		event.set_height(call->get_parameter_height());

		notify(std::mem_fn(&kompvter::TextureListener::on_texture_allocate), event);
	}

	process_texture_copy_before(
		source_texture.get_name(), destination_texture.get_name(),
		call->get_parameter_x(), call->get_parameter_y(),
		0, 0,
		call->get_parameter_width(), call->get_parameter_height());
}

void rosalind::TextureProcessor::process_copy_texture_before(
	const touchstone::call::CopyTexSubImage2D* call)
{
	if (!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_READ_FRAMEBUFFER))
	{
		return;
	}

	auto attachment = this->context->get_state().get_read_buffer();
	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_READ_FRAMEBUFFER);
	if (!frame_buffer.has_attachment(attachment) || !frame_buffer.is_texture_attachment(attachment))
	{
		attachment = GL_COLOR_ATTACHMENT0;
		if (!frame_buffer.has_attachment(attachment) || !frame_buffer.is_texture_attachment(attachment))
		{
			return;
		}
	}

	auto& source_texture = this->context->get_resource<touchstone::TextureResource>(frame_buffer.get_attachment_name(attachment));
	auto& destination_texture = this->context->get_current_resource<touchstone::TextureResource>(call->get_parameter_target());
	process_texture_copy_before(
		source_texture.get_name(), destination_texture.get_name(),
		call->get_parameter_x(), call->get_parameter_y(),
		call->get_parameter_xoffset(), call->get_parameter_yoffset(),
		call->get_parameter_width(), call->get_parameter_height());
}

void rosalind::TextureProcessor::process_copy_texture_before(
	const touchstone::call::CopyImageSubData* call)
{
	process_texture_copy_before(
		call->get_parameter_src_name(), call->get_parameter_dst_name(),
		call->get_parameter_src_x(), call->get_parameter_src_y(),
		call->get_parameter_dst_x(), call->get_parameter_dst_y(),
		call->get_parameter_src_width(), call->get_parameter_src_height());
}

void rosalind::TextureProcessor::process_texture_copy_before(
	GLuint source_name, GLuint destination_name,
	GLint source_x, GLint source_y,
	GLint destination_x, GLint destination_y,
	GLsizei width, GLsizei height)
{
	kompvter::TextureEvent event;
	event.set_source_name(source_name);
	event.set_destination_name(destination_name);
	event.set_source_x(source_x);
	event.set_source_y(source_y);
	event.set_destination_x(destination_x);
	event.set_destination_y(destination_y);
	event.set_width(width);
	event.set_height(height);

	notify(std::mem_fn(&kompvter::TextureListener::on_texture_copy), event);
}

bool rosalind::TextureProcessor::gl_format_type_to_kompvter(
	GLenum format, GLenum type, kompvter::TextureEvent& event)
{
	int component_format;
	switch (format)
	{
		case GL_RED:
			component_format = kompvter::TextureEvent::component_format_r;
			break;
		case GL_RG:
			component_format = kompvter::TextureEvent::component_format_rg;
			break;
		case GL_RGB:
		case GL_BGR:
			component_format = kompvter::TextureEvent::component_format_rgb;
			break;
		case GL_RGBA:
		case GL_BGRA:
			component_format = kompvter::TextureEvent::component_format_rgba;
			break;
		default:
			return false;
	}

	int component_type;
	switch (type)
	{
		case GL_UNSIGNED_BYTE:
			component_type = kompvter::TextureEvent::component_type_unsigned_byte;
			break;
		case GL_UNSIGNED_SHORT:
			component_type = kompvter::TextureEvent::component_type_unsigned_short;
			break;
		case GL_UNSIGNED_INT:
			component_type = kompvter::TextureEvent::component_type_unsigned_integer;
			break;
		case GL_FLOAT:
			component_type = kompvter::TextureEvent::component_type_float;
			break;
		default:
			component_type = kompvter::TextureEvent::component_type_none;
			break;
	}

	event.set_component_format(component_format);
	event.set_component_type(component_type);

	return true;
}

bool rosalind::TextureProcessor::gl_internal_format_to_kompvter(
	GLenum internal_format, kompvter::TextureEvent& event)
{
	// XXX: Probably missing some. Need to test with glCopySubImage disabled.
	switch (internal_format)
	{
		case GL_RED:
			event.set_component_format(kompvter::TextureEvent::component_format_r);
			break;
		case GL_RG:
			event.set_component_format(kompvter::TextureEvent::component_format_rg);
			break;
		case GL_RGB:
		case GL_R3_G3_B2:
		case GL_RGB4:
		case GL_RGB5:
		case GL_RGB8:
		case GL_RGB10:
		case GL_RGB12:
		case GL_RGB16:
		case GL_SRGB:
		case GL_SRGB8:
			event.set_component_format(kompvter::TextureEvent::component_format_rgb);
			break;
		case GL_RGBA:
		case GL_RGBA2:
		case GL_RGBA4:
		case GL_RGB5_A1:
		case GL_RGBA8:
		case GL_RGB10_A2:
		case GL_RGBA12:
		case GL_RGBA16:
		case GL_SRGB_ALPHA:
		case GL_SRGB8_ALPHA8:
			event.set_component_format(kompvter::TextureEvent::component_format_rgba);
			break;
		default:
			return false;
	}

	// It doesn't matter.
	// TODO: Or does it?
	event.set_component_type(kompvter::TextureEvent::component_type_unsigned_byte);

	return true;
}

bool rosalind::TextureProcessor::compressed_gl_format_to_kompvter(
	GLenum format, kompvter::TextureEvent& event)
{
	switch(format)
	{
		case GL_COMPRESSED_RGB_S3TC_DXT1_EXT:
			event.set_compression(kompvter::TextureEvent::compression_rgb_dxt1);
			event.set_component_format(kompvter::TextureEvent::component_format_rgb);
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT1_EXT:
			event.set_compression(kompvter::TextureEvent::compression_rgba_dxt1);
			event.set_component_format(kompvter::TextureEvent::component_format_rgba);
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT3_EXT:
			event.set_compression(kompvter::TextureEvent::compression_rgba_dxt3);
			event.set_component_format(kompvter::TextureEvent::component_format_rgba);
			break;
		case GL_COMPRESSED_RGBA_S3TC_DXT5_EXT:
			event.set_compression(kompvter::TextureEvent::compression_rgba_dxt5);
			event.set_component_format(kompvter::TextureEvent::component_format_rgba);
			break;
		case GL_COMPRESSED_SRGB_S3TC_DXT1_EXT:
			event.set_compression(kompvter::TextureEvent::compression_srgb_dxt1);
			event.set_component_format(kompvter::TextureEvent::component_format_rgb);
			break;
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT1_EXT:
			event.set_compression(kompvter::TextureEvent::compression_srgba_dxt1);
			event.set_component_format(kompvter::TextureEvent::component_format_rgba);
			break;
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT3_EXT:
			event.set_compression(kompvter::TextureEvent::compression_srgba_dxt3);
			event.set_component_format(kompvter::TextureEvent::component_format_rgba);
			break;
		case GL_COMPRESSED_SRGB_ALPHA_S3TC_DXT5_EXT:
			event.set_compression(kompvter::TextureEvent::compression_srgba_dxt5);
			event.set_component_format(kompvter::TextureEvent::component_format_rgba);
			break;
		default:
			return false;
	}

	return true;
}
