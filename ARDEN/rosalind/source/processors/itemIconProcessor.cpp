// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/glm.hpp>
#include "rosalind/common/re.hpp"
#include "rosalind/processors/itemIconProcessor.hpp"
#include "touchstone/gl/enums.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/events/itemIconEvent.hpp"

rosalind::ItemIconProcessor::ItemIconProcessor(
	const touchstone::Context* context,
	const kompvter::BufferManager* buffer_manager,
	const kompvter::TextureManager* texture_manager) :
		DrawProcessor(context, buffer_manager)
{
	this->context = context;
	this->buffer_manager = buffer_manager;
	this->texture_manager = texture_manager;
}

void rosalind::ItemIconProcessor::after_update(const touchstone::Call* call)
{
	switch (call->get_token())
	{
		case touchstone::call::DrawArrays::token:
			process_draw_arrays_after((const touchstone::call::DrawArrays*)call);
			break;
		case touchstone::call::Clear::token:
			process_clear_after((const touchstone::call::Clear*)call);
			break;
		case touchstone::call::InvalidateTexImage::token:
			process_invalidate_tex_image((const touchstone::call::InvalidateTexImage*)call);
			break;
		default:
			try_process_draw(call);
			break;
	}
}

bool rosalind::ItemIconProcessor::match() const
{
	if (!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		return false;
	}

	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	if (!frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0))
	{
		return false;
	}

	if (!frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
	{
		return false;
	}

	auto& texture = this->context->get_resource<touchstone::TextureResource>(frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));
	if (texture.get_internal_format() != GL_RGBA ||
		texture.get_width() != re::ITEM_ICON_WIDTH ||
		texture.get_height() != re::ITEM_ICON_HEIGHT)
	{
		return false;
	}

	return true;
}

void rosalind::ItemIconProcessor::on_draw(const kompvter::DrawEvent& draw_event)
{
	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();

	if (program.has_uniform("uDiffuseMap"))
	{
		kompvter::GUIDrawEvent event(draw_event);

		auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
		event.set_framebuffer(frame_buffer.get_name());

		auto color_attachment = frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0);
		if (color_attachment != 0 && this->texture_manager->has_texture(color_attachment))
		{
			event.set_color_attachment_name(color_attachment);
			event.set_color_attachment(this->texture_manager->get_texture(color_attachment));
		}

		auto uniform = program.get_uniform("uDiffuseMap");
		auto unit = GL_TEXTURE0 + uniform.values.at(0).integer;
		auto texture_name = this->context->get_binding_manager<touchstone::TextureResource>().get_current_name(GL_TEXTURE_2D, unit);
		if (texture_name != 0 && this->texture_manager->has_texture(texture_name))
		{
			event.set_atlas_name(texture_name);
			event.set_atlas(this->texture_manager->get_texture(texture_name));
		}

		notify(std::mem_fn(&kompvter::ItemIconListener::on_draw_gui), event);
	}
	else
	{
		if (!draw_event.get_vertex_attributes().has("aVertexPosition_BoneLabel"))
		{
			return;
		}

		kompvter::ModelDrawEvent event(draw_event);

		auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
		event.set_framebuffer(frame_buffer.get_name());

		auto color_attachment = frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0);
		if (color_attachment != 0 && this->texture_manager->has_texture(color_attachment))
		{
			event.set_color_attachment_name(color_attachment);
			event.set_color_attachment(this->texture_manager->get_texture(color_attachment));
		}

		if (program.has_uniform("uTextureAtlasPrimary"))
		{
			auto uniform = program.get_uniform("uTextureAtlasPrimary");
			auto unit = uniform.values.at(0).integer;

			auto texture_name = this->context->get_binding_manager<touchstone::TextureResource>().get_current_name(GL_TEXTURE_2D, unit);
			if (texture_name != 0 && this->texture_manager->has_texture(texture_name))
			{
				event.set_atlas_name(texture_name);
				event.set_atlas(this->texture_manager->get_texture(texture_name));
			}
		}

		notify(std::mem_fn(&kompvter::ItemIconListener::on_draw_model), event);
	}
}

void rosalind::ItemIconProcessor::process_draw_arrays_after(
	const touchstone::call::DrawArrays* call)
{
	if (!match())
	{
		return;
	}

	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();

	int unit;
	if (program.has_uniform("sSourceTex"))
	{
		unit = GL_TEXTURE0 + program.get_uniform("sSourceTex").values.at(0).integer;
	}
	else if (program.has_uniform("sSceneHDRTex"))
	{
		unit = GL_TEXTURE0 + program.get_uniform("sSceneHDRTex").values.at(0).integer;
	}
	else if (program.has_uniform("sTexture"))
	{
		unit = GL_TEXTURE0 + program.get_uniform("sTexture").values.at(0).integer;
	}
	else
	{
		assert(false);
		return;
	}

	auto source_name = this->context->get_binding_manager<touchstone::TextureResource>().get_current_name(GL_TEXTURE_2D, unit);
	assert(source_name != 0);
	assert(this->texture_manager->has_texture(source_name));

	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	auto destination_name = frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0);
	assert(this->texture_manager->has_texture(destination_name));

	kompvter::ItemIconEvent event;
	event.set_source_texture_name(source_name);
	event.set_source_texture(this->texture_manager->get_texture(source_name));
	event.set_destination_texture_name(destination_name);
	event.set_destination_texture(this->texture_manager->get_texture(destination_name));

	notify(std::mem_fn(&kompvter::ItemIconListener::on_transfer), event);
}

void rosalind::ItemIconProcessor::process_clear_after(
	const touchstone::call::Clear* call)
{
	if (!match())
	{
		return;
	}

	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	auto name = frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0);
	assert(this->texture_manager->has_texture(name));

	kompvter::ItemIconEvent event;
	event.set_texture_name(name);
	event.set_texture(this->texture_manager->get_texture(name));

	notify(std::mem_fn(&kompvter::ItemIconListener::on_clear), event);
}

void rosalind::ItemIconProcessor::process_invalidate_tex_image(
	const touchstone::call::InvalidateTexImage* call)
{
	if (!match())
	{
		return;
	}

	kompvter::ItemIconEvent event;
	event.set_texture_name(call->get_parameter_texture());
	event.set_texture(this->texture_manager->get_texture(call->get_parameter_texture()));

	notify(std::mem_fn(&kompvter::ItemIconListener::on_clear), event);
}
