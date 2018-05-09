// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/graphics/texture.hpp"

kompvter::ModelDrawEvent::ModelDrawEvent(const DrawEvent& base_draw_event) :
	DrawEvent(base_draw_event)
{
	// No.
}

const kompvter::Texture& kompvter::ModelDrawEvent::get_atlas() const
{
	return this->atlas;
}

void kompvter::ModelDrawEvent::set_atlas(const kompvter::Texture& value)
{
	this->atlas = value;
}

int kompvter::ModelDrawEvent::get_atlas_name() const
{
	return this->atlas_name;
}

void kompvter::ModelDrawEvent::set_atlas_name(int value)
{
	this->atlas_name = value;
}

int kompvter::ModelDrawEvent::get_framebuffer() const
{
	return this->framebuffer;
}

void kompvter::ModelDrawEvent::set_framebuffer(int value)
{
	this->framebuffer = value;
}

const kompvter::Texture& kompvter::ModelDrawEvent::get_color_attachment() const
{
	return this->color_attachment;
}

void kompvter::ModelDrawEvent::set_color_attachment(const Texture& value)
{
	this->color_attachment = value;
}

int kompvter::ModelDrawEvent::get_color_attachment_name() const
{
	return this->color_attachment_name;
}

void kompvter::ModelDrawEvent::set_color_attachment_name(int value)
{
	this->color_attachment_name = value;
}


bool kompvter::ModelDrawEvent::has_bone_transforms() const
{
	return this->bone_transforms.size() > 0;
}

bool kompvter::ModelDrawEvent::are_bone_transforms_identity() const
{
	if (this->bone_transforms_dirty)
	{
		glm::mat4 identity_matrix(1.0f);
		this->bone_transforms_identity = true;
		for (auto bone_transform: this->bone_transforms)
		{
			if (bone_transform != glm::mat4(1.0f))
			{
				this->bone_transforms_identity = false;
				break;
			}
		}

		this->bone_transforms_dirty = false;
	}

	return this->bone_transforms_identity;
}

std::size_t kompvter::ModelDrawEvent::get_num_bone_transforms() const
{
	return this->bone_transforms.size();
}

void kompvter::ModelDrawEvent::set_num_bone_transforms(std::size_t value)
{
	this->bone_transforms.resize(value, glm::mat4(1.0f));
	if (value == 0)
	{
		this->bone_transforms_identity = true;
	}
}

glm::mat4 kompvter::ModelDrawEvent::get_bone_transform(std::size_t index) const
{
	return this->bone_transforms.at(index);
}

void kompvter::ModelDrawEvent::set_bone_transform(
	std::size_t index, const glm::mat4& value)
{
	this->bone_transforms_dirty = true;
	this->bone_transforms.at(index) = value;
}
