// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_TEXTURE_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_TEXTURE_PROCESSOR_HPP

#include "kompvter/events/textureEvent.hpp"
#include "kompvter/listeners/textureListener.hpp"
#include "rosalind/processors/listenerProcessor.hpp"
#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/trace/call.hpp"

namespace rosalind
{
	class TextureProcessor : public ListenerProcessor<kompvter::TextureListener>
	{
		public:
			TextureProcessor(const touchstone::Context* context);
			~TextureProcessor() = default;

			void before_update(const touchstone::Call* call);
			void after_update(const touchstone::Call* call);

		private:
			void process_clear_after(const touchstone::call::Clear* call);

			void process_blit_framebuffer_after(const touchstone::call::BlitFramebuffer* call);
			void process_blit_framebuffer_after(const touchstone::call::BlitFramebufferEXT* call);

			void process_create_textures_after(const touchstone::call::GenTextures* call);
			void process_delete_textures_after(const touchstone::call::DeleteTextures* call);
			void process_update_texture_before(const touchstone::call::TexImage2D* call);
			void process_update_texture_before(const touchstone::call::TexImage2DMultisample* call);
			void process_update_texture_before(const touchstone::call::CompressedTexImage2D* call);
			void process_update_texture_before(
				GLuint name,
				GLenum component_format, GLenum component_type,
				GLint x, GLint y,
				GLsizei width, GLsizei height,
				const void* pixels);
			void process_update_texture_after(const touchstone::call::TexImage2D* call);
			void process_update_texture_after(const touchstone::call::TexSubImage2D* call);
			void process_update_texture_after(const touchstone::call::CompressedTexImage2D* call);
			void process_update_texture_after(const touchstone::call::CompressedTexSubImage2D* call);
			void process_update_texture_after(
				GLuint name,
				GLenum component_format, GLenum component_type,
				GLint x, GLint y,
				GLsizei width, GLsizei height,
				GLsizei pixels_size, const void* pixels);
			void process_copy_texture_before(const touchstone::call::CopyTexImage2D* call);
			void process_copy_texture_before(const touchstone::call::CopyTexSubImage2D* call);
			void process_copy_texture_before(const touchstone::call::CopyImageSubData* call);
			void process_texture_copy_before(
				GLuint source_name, GLuint destination_name,
				GLint source_x, GLint source_y,
				GLint destination_x, GLint destination_y,
				GLsizei width, GLsizei height);

			static bool gl_format_type_to_kompvter(
				GLenum format, GLenum type, kompvter::TextureEvent& event);
			static bool gl_internal_format_to_kompvter(
				GLenum internal_format, kompvter::TextureEvent& event);
			static bool compressed_gl_format_to_kompvter(
				GLenum format, kompvter::TextureEvent& event);

			const touchstone::Context* context;
	};
}

#endif
