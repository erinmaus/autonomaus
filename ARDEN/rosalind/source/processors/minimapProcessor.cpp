// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/glm.hpp>
#include "rosalind/common/re.hpp"
#include "rosalind/processors/minimapProcessor.hpp"
#include "touchstone/gl/enums.hpp"

rosalind::MinimapProcessor::MinimapProcessor(
	const touchstone::Context* context,
	const kompvter::BufferManager* buffer_manager,
	const kompvter::TextureManager* texture_manager) :
		DrawProcessor(context, buffer_manager)
{
	this->context = context;
	this->buffer_manager = buffer_manager;
	this->texture_manager = texture_manager;
}

void rosalind::MinimapProcessor::after_update(const touchstone::Call* call)
{
	try_process_draw(call);
}

bool rosalind::MinimapProcessor::match() const
{
	if (!this->context->get_binding_manager<touchstone::FrameBufferResource>().is_resource_bound(GL_DRAW_FRAMEBUFFER))
	{
		return false;;
	}

	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	if (!frame_buffer.has_attachment(GL_COLOR_ATTACHMENT0) ||
		!frame_buffer.has_attachment(GL_DEPTH_ATTACHMENT))
	{
		return false;
	}

	if (!frame_buffer.is_texture_attachment(GL_COLOR_ATTACHMENT0))
	{
		return false;
	}

	auto& texture = this->context->get_resource<touchstone::TextureResource>(frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0));
	if (texture.get_internal_format() != GL_RGB)
	{
		return false;
	}

	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();
	if (!program.has_uniform_block_binding("ViewTransforms"))
	{
		return false;
	}

	auto uniform_buffer_index = program.get_uniform_block_binding("ViewTransforms");
	auto uniform_buffer_name = this->context->get_binding_manager<touchstone::BufferResource>().get_current_name(GL_UNIFORM_BUFFER, uniform_buffer_index);
	if (uniform_buffer_name == 0 || !this->buffer_manager->has_buffer(uniform_buffer_name))
	{
		return false;
	}

	auto uniform_buffer = this->buffer_manager->get_buffer(uniform_buffer_name);
	assert(uniform_buffer.get_data_size() == sizeof(re::ViewTransform));
	auto view_transform = (const re::ViewTransform*)uniform_buffer.get_data();

	float C = view_transform->uProjectionMatrix[3].w;
	const float E = 0.01f;
	if (glm::abs(C - 1.0f) > E)
	{
		return false;
	}

	return true;
}

void rosalind::MinimapProcessor::on_draw(const kompvter::DrawEvent& draw_event)
{
	kompvter::MinimapDrawEvent event(draw_event);

	auto& frame_buffer = this->context->get_current_resource<touchstone::FrameBufferResource>(GL_DRAW_FRAMEBUFFER);
	event.set_framebuffer(frame_buffer.get_name());

	auto color_attachment = frame_buffer.get_attachment_name(GL_COLOR_ATTACHMENT0);
	event.set_color_attachment_name(color_attachment);
	event.set_color_attachment(this->texture_manager->get_texture(color_attachment));

	notify(std::mem_fn(&kompvter::MinimapListener::on_draw_minimap), std::cref(event));
}
