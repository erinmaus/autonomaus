// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_PROCESSORS_MINIMAP_PROCESSOR_HPP
#define KOMPROMAT_PROCESSORS_MINIMAP_PROCESSOR_HPP

#include <string>
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompvter/listeners/minimapListener.hpp"
#include "kompromat/listenerProcessor.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class MinimapProcessor : public ListenerProcessor<kompvter::MinimapListener>
	{
	public:
		MinimapProcessor(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~MinimapProcessor() = default;

		void process(
			const std::string& scope,
			int name,
			const paley::Event& event) override;

	private:
		void process_draw_minimap(const paley::Event& event);

		kompvter::BufferManager& buffer_manager;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
