// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_PROCESSORS_MODEL_PROCESSOR_HPP
#define KOMPROMAT_PROCESSORS_MODEL_PROCESSOR_HPP

#include <string>
#include "kompromat/listenerProcessor.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompvter/listeners/modelListener.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class ModelProcessor : public ListenerProcessor<kompvter::ModelListener>
	{
	public:
		ModelProcessor(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~ModelProcessor() = default;

		void process(
			const std::string& scope,
			int name,
			const paley::Event& event) override;

	private:
		void process_draw_model(const paley::Event& event);
		void process_draw_terrain(const paley::Event& event);
		void process_draw_water(const paley::Event& event);

		kompvter::BufferManager& buffer_manager;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
