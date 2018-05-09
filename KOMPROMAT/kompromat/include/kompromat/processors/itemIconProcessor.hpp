// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_PROCESSORS_ITEM_ICON_PROCESSOR_HPP
#define KOMPROMAT_PROCESSORS_ITEM_ICON_PROCESSOR_HPP

#include <string>
#include "paley/event/event.hpp"
#include "kompromat/listenerProcessor.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompvter/listeners/itemIconListener.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class ItemIconProcessor : public ListenerProcessor<kompvter::ItemIconListener>
	{
	public:
		ItemIconProcessor(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~ItemIconProcessor() = default;

		void process(
			const std::string& scope,
			int name,
			const paley::Event& event) override;

	private:
		void process_draw_gui_item_icon(const paley::Event& event);
		void process_draw_item_icon(const paley::Event& event);
		void process_transfer_item_icon_texture(const paley::Event& event);
		void process_clear_item_icon_buffer(const paley::Event& event);

		kompvter::BufferManager& buffer_manager;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
