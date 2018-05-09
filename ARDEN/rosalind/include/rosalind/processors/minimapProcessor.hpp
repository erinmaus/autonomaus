// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_MINIMAP_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_MINIMAP_PROCESSOR_HPP

#include "touchstone/gl/context.hpp"
#include "kompvter/events/drawEvent.hpp"
#include "kompvter/events/minimapDrawEvent.hpp"
#include "kompvter/listeners/minimapListener.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "rosalind/processors/listenerProcessor.hpp"
#include "rosalind/processors/drawProcessor.hpp"

namespace rosalind
{
	class MinimapProcessor :
		public ListenerProcessor<kompvter::MinimapListener>,
		private DrawProcessor
	{
	public:
		MinimapProcessor(
			const touchstone::Context* context,
			const kompvter::BufferManager* buffer_manager,
			const kompvter::TextureManager* texture_manager);
		~MinimapProcessor() = default;

		void after_update(const touchstone::Call* call) override;

	private:
		bool match() const override;
		void on_draw(const kompvter::DrawEvent& draw_event) override;

		const touchstone::Context* context;
		const kompvter::BufferManager* buffer_manager;
		const kompvter::TextureManager* texture_manager;
	};
}

#endif
