// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/glm.hpp>
#include "rosalind/common/re.hpp"
#include "rosalind/processors/guiProcessor.hpp"
#include "touchstone/gl/enums.hpp"
#include "kompvter/events/viewportEvent.hpp"

rosalind::GUIProcessor::GUIProcessor(
	const touchstone::Context* context,
	const kompvter::BufferManager* buffer_manager,
	const kompvter::TextureManager* texture_manager) :
		DrawProcessor(context, buffer_manager)
{
	this->context = context;
	this->buffer_manager = buffer_manager;
	this->texture_manager = texture_manager;
}

void rosalind::GUIProcessor::start_frame()
{
	this->pending = true;
}

void rosalind::GUIProcessor::after_update(const touchstone::Call* call)
{
	if (!try_process_draw(call))
	{
		switch (call->get_token())
		{
			case touchstone::call::DrawArrays::token:
				on_draw_arrays((const touchstone::call::DrawArrays*)call);
			case touchstone::call::Viewport::token:
				process_viewport_after((const touchstone::call::Viewport*)call);
				break;
		}
	}
}

void rosalind::GUIProcessor::on_draw_arrays(const touchstone::call::DrawArrays*)
{
	if (!this->pending)
	{
		return;
	}

	if (this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		return;
	}

	if (!this->context->get_binding_manager<touchstone::ProgramResource>().is_resource_bound())
	{
		return;
	}
	
	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();
	if (!program.has_uniform("sSourceTex") &&
		!program.has_uniform("sSceneHDRTex"))
	{
		return;
	}

	notify(std::mem_fn(&kompvter::GUIListener::on_present));
	this->pending = false;
}

bool rosalind::GUIProcessor::match() const
{
	if (!this->context->get_binding_manager<touchstone::ProgramResource>().is_resource_bound())
	{
		return false;
	}

	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();
	if (!program.has_attrib("aVertexPosition2D") ||
		!program.has_attrib("aVertexColour") ||
		!program.has_attrib("aTextureUV") ||
		!program.has_attrib("aTextureUVAtlasMin") ||
		!program.has_attrib("aTextureUVAtlasMax"))
	{
		return false;
	}

	return true;
}

void rosalind::GUIProcessor::on_draw(const kompvter::DrawEvent& draw_event)
{
	kompvter::GUIDrawEvent event(draw_event);

	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();
	if (program.has_uniform("uDiffuseMap"))
	{
		auto uniform = program.get_uniform("uDiffuseMap");
		auto unit = GL_TEXTURE0 + uniform.values.at(0).integer;

		auto texture_name = this->context->get_binding_manager<touchstone::TextureResource>().get_current_name(GL_TEXTURE_2D, unit);
		if (texture_name != 0 && this->texture_manager->has_texture(texture_name))
		{
			event.set_atlas_name(texture_name);
			event.set_atlas(this->texture_manager->get_texture(texture_name));
		}

		if (this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_DRAW_FRAMEBUFFER))
		{
			auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
			event.set_framebuffer(frame_buffer.get_name());

			auto color_attachment = frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0);
			if (color_attachment != 0 && this->texture_manager->has_texture(color_attachment))
			{
				event.set_color_attachment_name(color_attachment);
				event.set_color_attachment(this->texture_manager->get_texture(color_attachment));
			}
		}

		notify(std::mem_fn(&kompvter::GUIListener::on_draw_gui), std::cref(event));
	}
}

void rosalind::GUIProcessor::process_viewport_after(
	const touchstone::call::Viewport* call)
{
	kompvter::ViewportEvent event;
	event.set_x(call->get_parameter_x());
	event.set_y(call->get_parameter_y());
	event.set_width(call->get_parameter_width());
	event.set_height(call->get_parameter_height());

	notify(std::mem_fn(&kompvter::GUIListener::on_set_viewport), std::cref(event));
}
