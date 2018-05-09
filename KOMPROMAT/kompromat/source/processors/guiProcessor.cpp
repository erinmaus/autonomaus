// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompromat/events/guiDrawEventBuilder.hpp"
#include "kompromat/events/viewportEventBuilder.hpp"
#include "kompromat/processors/guiProcessor.hpp"

kompromat::GUIProcessor::GUIProcessor(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager)
	: buffer_manager(buffer_manager), texture_manager(texture_manager)
{
	// Nada.
}

void kompromat::GUIProcessor::process(
	const std::string& scope, int name, const paley::Event& event)
{
	if (scope == "ARDEN")
	{
		if (event.get_name() == "draw_gui")
		{
			process_draw_gui(event);
		}
		else if (event.get_name() == "finalize_world")
		{
			process_finalize_world(event);
		}
		else if (event.get_name() == "set_viewport")
		{
			process_set_viewport(event);
		}
	}
}

void kompromat::GUIProcessor::process_draw_gui(const paley::Event& event)
{
	GUIDrawEventBuilder gui_draw_event_builder(buffer_manager, texture_manager);
	gui_draw_event_builder.from_draw_gui(event);
	auto& gui_draw_event = gui_draw_event_builder.get_event();

	for_each(
		[&gui_draw_event](kompvter::GUIListener* listener)
		{
			listener->on_draw_gui(gui_draw_event);
		});
}

void kompromat::GUIProcessor::process_finalize_world(const paley::Event& event)
{
	for_each(
		[](kompvter::GUIListener* listener)
		{
			listener->on_present();
		});
}

void kompromat::GUIProcessor::process_set_viewport(const paley::Event& event)
{
	ViewportEventBuilder viewport_event_builder;
	viewport_event_builder.from_set_viewport(event);
	auto& viewport_event = viewport_event_builder.get_event();

	for_each(
		[&viewport_event](kompvter::GUIListener* listener)
		{
			listener->on_set_viewport(viewport_event);
		});
}
