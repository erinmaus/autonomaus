// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_ITEM_ICON_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_ITEM_ICON_PROCESSOR_HPP

#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/context.hpp"
#include "kompvter/events/drawEvent.hpp"
#include "kompvter/listeners/itemIconListener.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "rosalind/processors/listenerProcessor.hpp"
#include "rosalind/processors/drawProcessor.hpp"

namespace rosalind
{
	class ItemIconProcessor :
		public ListenerProcessor<kompvter::ItemIconListener>,
		private DrawProcessor
	{
	public:
		ItemIconProcessor(
			const touchstone::Context* context,
			const kompvter::BufferManager* buffer_manager,
			const kompvter::TextureManager* texture_manager);

		void after_update(const touchstone::Call* call) override;

	private:
		bool match() const override;
		void on_draw(const kompvter::DrawEvent& draw_event) override;
		void process_draw_arrays_after(const touchstone::call::DrawArrays* call);
		void process_clear_after(const touchstone::call::Clear* call);
		void process_invalidate_tex_image(const touchstone::call::InvalidateTexImage* call);

		const touchstone::Context* context;
		const kompvter::BufferManager* buffer_manager;
		const kompvter::TextureManager* texture_manager;
	};
}

#endif
