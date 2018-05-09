// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_GUI_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_GUI_PROCESSOR_HPP

#include "touchstone/gl/context.hpp"
#include "kompvter/events/drawEvent.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/listeners/guiListener.hpp"
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "rosalind/processors/listenerProcessor.hpp"
#include "rosalind/processors/drawProcessor.hpp"

namespace rosalind
{
	class GUIProcessor :
		public ListenerProcessor<kompvter::GUIListener>,
		private DrawProcessor
	{
	public:
		GUIProcessor(
			const touchstone::Context* context,
			const kompvter::BufferManager* buffer_manager,
			const kompvter::TextureManager* texture_manager);
		~GUIProcessor() = default;

		void start_frame() override;
		void after_update(const touchstone::Call* call) override;

	private:
		bool pending = true;
		void on_draw_arrays(const touchstone::call::DrawArrays* call);

		bool match() const override;
		void on_draw(const kompvter::DrawEvent& draw_event) override;
		void process_viewport_after(const touchstone::call::Viewport* call);

		const touchstone::Context* context;
		const kompvter::BufferManager* buffer_manager;
		const kompvter::TextureManager* texture_manager;
	};
}

#endif
