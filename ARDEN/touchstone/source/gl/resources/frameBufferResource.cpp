// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <algorithm>
#include <limits>
#include "touchstone/gl/context.hpp"
#include "touchstone/gl/resources/frameBufferResource.hpp"
#include "touchstone/gl/resources/renderBufferResource.hpp"
#include "touchstone/gl/resources/textureResource.hpp"

touchstone::FrameBufferResource::FrameBufferResource(
	Context* context, GLuint name) :
		Resource(context, name)
{
	// Nothing.
}

void touchstone::FrameBufferResource::attach_texture(
	GLenum attachment, GLuint name, GLuint level, GLuint layer)
{
	TextureAttachment texture;
	texture.name = name;
	texture.level = level;
	texture.layer = layer;

	this->textures[attachment] = texture;
	this->attachments.insert(attachment);
}

void touchstone::FrameBufferResource::attach_render_buffer(GLenum attachment, GLuint name)
{
	RenderBufferAttachment render_buffer;
	render_buffer.name = name;

	this->render_buffers[attachment] = render_buffer;
	this->attachments.insert(attachment);
}

bool touchstone::FrameBufferResource::has_attachment(GLenum attachment) const
{
	return this->attachments.count(attachment) != 0;
}

bool touchstone::FrameBufferResource::is_texture_attachment(GLenum attachment) const
{
	return this->textures.count(attachment) != 0;
}

bool touchstone::FrameBufferResource::is_render_buffer_attachment(GLenum attachment) const
{
	return this->render_buffers.count(attachment) != 0;
}

GLuint touchstone::FrameBufferResource::get_attachment_name(GLenum attachment) const
{
	if (has_attachment(attachment))
	{
		if (is_texture_attachment(attachment))
		{
			return this->textures.find(attachment)->second.name;
		}
		else if (is_render_buffer_attachment(attachment))
		{
			return this->render_buffers.find(attachment)->second.name;
		}
		else
		{
			assert(false && "attachment is not texture or render buffer");
		}
	}

	return 0;
}

GLsizei touchstone::FrameBufferResource::get_width() const
{
	if (attachments.empty())
	{
		return 0;
	}

	GLsizei width = std::numeric_limits<GLsizei>::max();
	for (auto attachment: this->attachments)
	{
		auto name = get_attachment_name(attachment);
		if (is_texture_attachment(attachment))
		{
			auto& texture = get_context()->get_resource<TextureResource>(name);
			auto level = this->textures.find(attachment)->second.level;
			width = std::min(width, texture.get_width(level));
		}
		else if (is_render_buffer_attachment(attachment))
		{
			auto& render_buffer = get_context()->get_resource<RenderBufferResource>(name);
			width = std::min(width, render_buffer.get_width());
		}
	}

	return width;
}

GLsizei touchstone::FrameBufferResource::get_height() const
{
	if (attachments.empty())
	{
		return 0;
	}

	GLsizei height = std::numeric_limits<GLsizei>::max();
	for (auto attachment: this->attachments)
	{
		auto name = get_attachment_name(attachment);
		if (is_texture_attachment(attachment))
		{
			auto& texture = get_context()->get_resource<TextureResource>(name);
			auto level = this->textures.find(attachment)->second.level;
			height = std::min(height, texture.get_height(level));
		}
		else if (is_render_buffer_attachment(attachment))
		{
			auto& render_buffer = get_context()->get_resource<RenderBufferResource>(name);
			height = std::min(height, render_buffer.get_height());
		}
	}

	return height;
}

touchstone::FrameBufferResource::const_attachment_iterator
touchstone::FrameBufferResource::attachments_begin() const
{
	return this->attachments.begin();
}

touchstone::FrameBufferResource::const_attachment_iterator
touchstone::FrameBufferResource::attachments_end() const
{
	return this->attachments.end();
}
