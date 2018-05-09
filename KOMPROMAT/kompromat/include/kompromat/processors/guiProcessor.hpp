// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_PROCESSORS_GUI_PROCESSOR_HPP
#define KOMPROMAT_PROCESSORS_GUI_PROCESSOR_HPP

#include <string>
#include "kompromat/listenerProcessor.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompvter/listeners/guiListener.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class GUIProcessor : public ListenerProcessor<kompvter::GUIListener>
	{
	public:
		GUIProcessor(
			kompvter::BufferManager& buffer_manager,
			kompvter::TextureManager& texture_manager);
		~GUIProcessor() = default;

		void process(
			const std::string& scope,
			int name,
			const paley::Event& event) override;

	private:
		void process_draw_gui(const paley::Event& event);
		void process_finalize_world(const paley::Event& event);
		void process_set_viewport(const paley::Event& event);

		kompvter::BufferManager& buffer_manager;
		kompvter::TextureManager& texture_manager;
	};
}

#endif
