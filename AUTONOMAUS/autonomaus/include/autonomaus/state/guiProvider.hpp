// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_GUI_PROVIDER_HPP
#define AUTONOMAUS_STATE_GUI_PROVIDER_HPP

#include <memory>
#include "autonomaus/state/gui.hpp"
#include "autonomaus/state/gameStateProvider.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "chomsky/gui/guiContent.hpp"
#include "chomsky/gui/guiParser.hpp"
#include "glooper/resources/resourceListener.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/listeners/frameListener.hpp"
#include "kompvter/listeners/guiListener.hpp"

namespace autonomaus
{
	class GUIProvider :
		public GameStateProvider<GUI, GUIProvider, GameStateIndependentChannel>,
		private kompvter::FrameListener,
		private kompvter::GUIListener,
		private glooper::ResourceListener
	{
	public:
		typedef GameStateProvider<GUI, GUIProvider, GameStateIndependentChannel> Base;
		using Base::Slot;

		GUIProvider() = default;
		~GUIProvider();

		void attach(GameStateManager& game_state_manager, Slot& slot) override;
		void detach() override;

		void update() override;

	private:
		void on_reload_resources() override;
		void on_draw_enabled(float timestamp) override;
		void on_draw_disabled(float timestamp) override;
		void on_draw_gui(const kompvter::GUIDrawEvent& event) override;

		bool draw_enabled = false;

		typedef std::unique_ptr<chomsky::GUIParser> ParserPointer;
		std::map<std::string, ParserPointer> parsers;
		std::unique_ptr<chomsky::GUIElementStream> element_stream;
		void load_parsers();
		void load_parser(const std::string& path);
	};
}

#endif
