// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_MODEL_DRAW_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_MODEL_DRAW_EVENT_BUILDER_HPP

#include <vector>
#include <glm/glm.hpp>
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompromat/events/drawEventBuilder.hpp"

namespace kompromat
{
	class ModelDrawEventBuilder : public DrawEventBuilder
	{
	public:
		ModelDrawEventBuilder(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~ModelDrawEventBuilder() = default;

		const kompvter::ModelDrawEvent& get_event() const;

		void from_draw_model(const paley::Event& e);

	private:
		void process_model_fields(
			const paley::Event& e);
		void process_model_bone_transforms(
			const float* bone_transform_elements, std::size_t count);

		kompvter::ModelDrawEvent event;
		kompvter::BufferManager& buffer_manager;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
