// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_MODEL_DRAW_EVENT_HPP
#define KOMPVTER_EVENTS_MODEL_DRAW_EVENT_HPP

#include <vector>
#include <glm/glm.hpp>
#include "kompvter/events/drawEvent.hpp"
#include "kompvter/graphics/texture.hpp"

namespace kompvter
{
	class ModelDrawEvent : public DrawEvent
	{
	public:
		ModelDrawEvent(const DrawEvent& base_draw_event);
		ModelDrawEvent() = default;
		~ModelDrawEvent() = default;

		const Texture& get_atlas() const;
		void set_atlas(const Texture& value);

		int get_atlas_name() const;
		void set_atlas_name(int value);

		int get_framebuffer() const;
		void set_framebuffer(int value);

		const Texture& get_color_attachment() const;
		void set_color_attachment(const Texture& value);

		int get_color_attachment_name() const;
		void set_color_attachment_name(int value);

		bool has_bone_transforms() const;
		bool are_bone_transforms_identity() const;

		std::size_t get_num_bone_transforms() const;
		void set_num_bone_transforms(std::size_t value);

		glm::mat4 get_bone_transform(std::size_t index) const;
		void set_bone_transform(std::size_t index, const glm::mat4& value);

	private:
		Texture atlas;
		int atlas_name = 0;

		int framebuffer = 0;
		Texture color_attachment;
		int color_attachment_name = 0;

		std::vector<glm::mat4> bone_transforms;
		mutable bool bone_transforms_identity = true;
		mutable bool bone_transforms_dirty = true;
	};
}

#endif
