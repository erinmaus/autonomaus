// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCES_FRAME_BUFFER_RESOURCE_HPP
#define TOUCHSTONE_GL_RESOURCES_FRAME_BUFFER_RESOURCE_HPP

#include <unordered_map>
#include <unordered_set>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	class FrameBufferResource : public Resource
	{
	public:
		FrameBufferResource(Context* context, GLuint name);
		~FrameBufferResource() = default;

		void attach_texture(
			GLenum attachment, GLuint name, GLuint level, GLuint layer);
		void attach_render_buffer(GLenum attachment, GLuint name);
		void remove_attachment(GLenum attachment);

		bool has_attachment(GLenum attachment) const;
		bool is_texture_attachment(GLenum attachment) const;
		bool is_render_buffer_attachment(GLenum attachment) const;
		GLuint get_attachment_name(GLenum attachment) const;

		GLsizei get_width() const;
		GLsizei get_height() const;

	private:
		typedef std::unordered_set<GLenum> AttachmentCollection;
		AttachmentCollection attachments;

		struct TextureAttachment
		{
			GLuint name;
			GLuint level;
			GLuint layer;
		};
		std::unordered_map<GLenum, TextureAttachment> textures;

		struct RenderBufferAttachment
		{
			GLuint name;
		};
		std::unordered_map<GLenum, RenderBufferAttachment> render_buffers;

	public:
		typedef AttachmentCollection::const_iterator const_attachment_iterator;

		const_attachment_iterator attachments_begin() const;
		const_attachment_iterator attachments_end() const;
	};
}

#endif
