// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompromat/events/modelDrawEventBuilder.hpp"

kompromat::ModelDrawEventBuilder::ModelDrawEventBuilder(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager)
	: DrawEventBuilder(this->event),
	buffer_manager(buffer_manager),
	texture_manager(texture_manager)
{
	// Nada.
}

const kompvter::ModelDrawEvent& kompromat::ModelDrawEventBuilder::get_event() const
{
	return this->event;
}

void kompromat::ModelDrawEventBuilder::from_draw_model(const paley::Event& e)
{
	process(e, buffer_manager);
	process_model_fields(e);
}

void kompromat::ModelDrawEventBuilder::process_model_fields(
	const paley::Event& e)
{
	int atlas_name;
	{
		paley::EventField t;
		if (!e.get_root().try_get_field("texture", t) ||
			!t.try_get_integer(atlas_name))
		{
			assert(false);
			atlas_name = 0;
		}
	}
	this->event.set_atlas_name(atlas_name);
	this->event.set_atlas(texture_manager.get_texture(atlas_name));

	{
		paley::EventField b;
		if (e.get_root().try_get_field("bones", b))
		{
			const std::uint8_t* bone_transforms_data;
			std::size_t bone_transforms_data_size;

			if (!b.try_get_data(bone_transforms_data, bone_transforms_data_size))
			{
				assert(false);
				bone_transforms_data = nullptr;
				bone_transforms_data_size = 0;
			}

			auto bone_transforms_elements = (const float*)bone_transforms_data;
			std::size_t bone_transforms_elements_count =
				bone_transforms_data_size / (sizeof(float) * 12);
			process_model_bone_transforms(
				bone_transforms_elements, bone_transforms_elements_count);
		}
	}
}

void kompromat::ModelDrawEventBuilder::process_model_bone_transforms(
	const float* bone_transform_elements, std::size_t count)
{
	this->event.set_num_bone_transforms(count);

	auto current = bone_transform_elements;
	for (std::size_t i = 0; i < count; ++i)
	{
		glm::mat4 bone_transform(
			current[0], current[1], current[2], 0.0f,
			current[3], current[4], current[5], 0.0f,
			current[6], current[7], current[8], 0.0f,
			current[9], current[10], current[11], 1.0f);
		this->event.set_bone_transform(i, bone_transform);

		current += 12;
	}
}
