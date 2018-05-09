// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/time.hpp"
#include "autonomaus/graphics/worldDrawHook.hpp"

autonomaus::WorldDrawHook::WorldDrawHook(
	touchstone::ImmediateDispatch& dispatch,
	DrawState& draw_state) :
		DrawHook(dispatch, draw_state)
{
	// Nothing.
}

void autonomaus::WorldDrawHook::before_call(
	const touchstone::Context& context,
	std::uint32_t token)
{
	switch (token)
	{
		case touchstone::call::DrawArrays::token:
		case touchstone::call::DrawRangeElements::token:
		case touchstone::call::MultiDrawArrays::token:
		case touchstone::call::MultiDrawElements::token:
			if (this->cleared_frame_buffer &&
				this->updated_view_transforms &&
				match_world_frame_buffer(context) &&
				has_depth_buffer(context))
			{
				get_draw_state().set_camera_view_matrix(view_transforms.uViewMatrix);
				get_draw_state().set_camera_projection_matrix(view_transforms.uProjectionMatrix);

				this->world_viewport_x = context.get_state().get_viewport_x();
				this->world_viewport_y = context.get_state().get_viewport_y();
				this->world_viewport_width = context.get_state().get_viewport_width();
				this->world_viewport_height = context.get_state().get_viewport_height();

				draw_before();
				this->cleared_frame_buffer = false;
				this->updated_view_transforms = false;
				this->is_drawing = true;
			}
			break;
	}
}

void autonomaus::WorldDrawHook::after_call(
	const touchstone::Context& context,
	const touchstone::Call* call)
{
	switch (call->get_token())
	{
		case touchstone::call::BindFramebuffer::token:
		case touchstone::call::BindFramebufferEXT::token:
			if (this->is_drawing &&
				match_world_frame_buffer(context) &&
				!has_depth_buffer(context))
			{
				int frame_buffer;
				if (call->get_token() == touchstone::call::BindFramebuffer::token)
				{
					auto c = (touchstone::call::BindFramebuffer*)call;
					frame_buffer = c->get_parameter_framebuffer();
				}
				else
				{
					auto c = (touchstone::call::BindFramebufferEXT*)call;
					frame_buffer = c->get_parameter_framebuffer();
				}

				this->is_drawing = false;
				draw_after(frame_buffer);
			}
			break;
		case touchstone::call::Clear::token:
			if (match_world_frame_buffer(context))
			{
				auto c = (const touchstone::call::Clear*)call;
				if (c->get_parameter_mask() & GL_COLOR_BUFFER_BIT)
				{
					this->cleared_frame_buffer = true;
					this->updated_view_transforms = false;
				}
			}
			break;
		case touchstone::call::BindBuffer::token:
			if (this->cleared_frame_buffer &&
				!this->updated_view_transforms &&
				match_world_frame_buffer(context) &&
				has_depth_buffer(context))
			{
				auto c = (const touchstone::call::BindBuffer*)call;
				if (c->get_parameter_target() == GL_UNIFORM_BUFFER)
				{
					this->updated_view_transforms = true;
					this->view_transforms_uniform_buffer = c->get_parameter_buffer();
					this->world_frame_buffer = context.get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER).get_name();
				}
			}
			break;
		case touchstone::call::BufferSubData::token:
			if (this->cleared_frame_buffer &&
				this->updated_view_transforms &&
				!this->is_drawing &&
				match_world_frame_buffer(context) &&
				has_depth_buffer(context))
			{
				auto c = (const touchstone::call::BufferSubData*)call;
				if (c->get_parameter_target() == GL_UNIFORM_BUFFER)
				{
					auto uniform_buffer = context.get_current_resource<touchstone::BufferResource>(GL_UNIFORM_BUFFER);
					if (uniform_buffer.get_name() == this->view_transforms_uniform_buffer &&
						c->get_parameter_offset() + c->get_parameter_size() <= sizeof(rosalind::re::ViewTransform))
					{
						auto output = (std::uint8_t*)&this->view_transforms;
						std::memcpy(
							output + c->get_parameter_offset(),
							c->get_parameter_data(),
							c->get_parameter_size());
					}
				}
			}
			break;
	}
}

void autonomaus::WorldDrawHook::on_flip(const touchstone::Context& context)
{
	this->cleared_frame_buffer = false;
	this->updated_view_transforms = false;
	this->is_drawing = false;
}

void autonomaus::WorldDrawHook::draw_before()
{
	get_renderer().begin();
	{
		before().draw(get_current_time());
	}
	get_renderer().end();
}

void autonomaus::WorldDrawHook::draw_after(int current_frame_buffer)
{
	get_renderer().begin();
	{
		get_renderer().set_viewport(
			this->world_viewport_x, this->world_viewport_y,
			this->world_viewport_width, this->world_viewport_height);

		get_dispatch().BindFramebufferEXT(
			GL_DRAW_FRAMEBUFFER,
			this->world_frame_buffer);

		after().draw(get_current_time());

		get_dispatch().BindFramebufferEXT(
			GL_DRAW_FRAMEBUFFER,
			current_frame_buffer);
	}
	get_renderer().end();
}

bool autonomaus::WorldDrawHook::match_world_frame_buffer(
	const touchstone::Context& context)
{
	auto binding_manager = context.get_binding_manager<touchstone::FrameBufferResource>();
	if (binding_manager.is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		auto& frame_buffer = context.get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
		if (frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0) &&
			frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
		{
			if (!context.get_resource_manager<touchstone::TextureResource>().has(frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0)))
			{
				return false;
			}

			auto& texture = context.get_resource<touchstone::TextureResource>(frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));
			if (texture.get_internal_format() == GL_R11F_G11F_B10F)
			{
				return true;
			}
		}
	}

	return false;
}

bool autonomaus::WorldDrawHook::has_depth_buffer(
	const touchstone::Context& context)
{
	auto binding_manager = context.get_binding_manager<touchstone::FrameBufferResource>();
	if (binding_manager.is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		auto& frame_buffer = context.get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
		if (frame_buffer.has_attachment(GL_DEPTH_ATTACHMENT) ||
			frame_buffer.has_attachment(GL_DEPTH_STENCIL_ATTACHMENT))
		{
			return true;
		}
		else
		{
			return false;
		}
	}

	// We assume the default framebuffer (i.e., the back buffer) has a depth
	// buffer.
	return true;
}
