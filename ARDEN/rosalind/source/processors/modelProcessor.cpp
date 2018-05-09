// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <glm/gtc/matrix_transform.hpp>
#include "rosalind/processors/modelProcessor.hpp"
#include "touchstone/gl/enums.hpp"

rosalind::ModelProcessor::ModelProcessor(
	const touchstone::Context* context,
	const kompvter::BufferManager* buffer_manager,
	const kompvter::TextureManager* texture_manager) :
		DrawProcessor(context, buffer_manager)
{
	this->context = context;
	this->buffer_manager = buffer_manager;
	this->texture_manager = texture_manager;
}

void rosalind::ModelProcessor::after_update(const touchstone::Call* call)
{
	try_process_draw(call);
}

bool rosalind::ModelProcessor::match() const
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
	if (texture.get_internal_format() != GL_R11F_G11F_B10F)
	{
		return false;
	}

	return true;
}

void rosalind::ModelProcessor::on_draw(const kompvter::DrawEvent& draw_event)
{
	kompvter::ModelDrawEvent event(draw_event);

	auto& attributes = draw_event.get_vertex_attributes();
	auto& program = this->context->get_current_resource<touchstone::ProgramResource>();

	if (program.has_uniform("uTextureAtlasPrimary"))
	{
		auto uniform = program.get_uniform("uTextureAtlasPrimary");
		auto unit = GL_TEXTURE0 + uniform.values.at(0).integer;

		auto texture_name = this->context->get_binding_manager<touchstone::TextureResource>().get_current_name(GL_TEXTURE_2D, unit);
		if (texture_name != 0 && this->texture_manager->has_texture(texture_name))
		{
			event.set_atlas_name(texture_name);
			event.set_atlas(this->texture_manager->get_texture(texture_name));
		}
	}

	if (program.has_uniform("uBoneTransforms"))
	{
		auto uniform = program.get_uniform("uBoneTransforms");
		auto num_bones = uniform.values.size() / 3;

		event.set_num_bone_transforms(num_bones);

		std::size_t current_index = 0;
		for (std::size_t i = 0; i < num_bones; ++i)
		{
			glm::vec4 a = uniform.values.at(current_index).single_vec4;
			glm::vec4 b = uniform.values.at(current_index + 1).single_vec4;
			glm::vec4 c = uniform.values.at(current_index + 2).single_vec4;
			glm::mat4 bone_transform(
				a.x, a.y, a.z, 0.0f,
				a.w, b.x, b.y, 0.0f,
				b.z, b.w, c.x, 0.0f,
				c.y, c.z, c.w, 1.0f);
			event.set_bone_transform(i, bone_transform);
			current_index += 3;
		}
	}

	if (program.has_uniform("uVertexScale"))
	{
		auto scale = program.get_uniform("uVertexScale").values[0].single;
		auto scale_matrix = glm::scale(event.get_model_matrix(), glm::vec3(scale));
		event.set_model_matrix(scale_matrix);
	}

	if (attributes.has("aMaterialProperties"))
	{
		notify(std::mem_fn(&kompvter::ModelListener::on_draw_terrain), std::cref(event));
	}
	else if (attributes.has("aWaterPositionAndDepth"))
	{
		notify(std::mem_fn(&kompvter::ModelListener::on_draw_water), std::cref(event));
	}
	else if (attributes.has("aVertexPosition_BoneLabel"))
	{
		notify(std::mem_fn(&kompvter::ModelListener::on_draw_model), std::cref(event));
	}
	else if (attributes.has("aBillboardMeta") || attributes.has("aParticleTextureMeta"))
	{
		notify(std::mem_fn(&kompvter::ModelListener::on_draw_particle), std::cref(event));
	}
}
