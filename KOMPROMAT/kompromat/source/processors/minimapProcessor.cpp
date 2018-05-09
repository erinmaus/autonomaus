// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompromat/events/minimapDrawEventBuilder.hpp"
#include "kompromat/processors/minimapProcessor.hpp"

kompromat::MinimapProcessor::MinimapProcessor(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager)
	: buffer_manager(buffer_manager), texture_manager(texture_manager)
{
	// Nada.
}

void kompromat::MinimapProcessor::process(
	const std::string& scope, int name, const paley::Event& event)
{
	if (scope == "ARDEN")
	{
		if (event.get_name() == "draw_minimap")
		{
			process_draw_minimap(event);
		}
	}
}

void kompromat::MinimapProcessor::process_draw_minimap(const paley::Event& event)
{
	MinimapDrawEventBuilder minimap_draw_event_builder(
		this->buffer_manager, this->texture_manager);
	auto& minimap_draw_event = minimap_draw_event_builder.get_event();

	for_each(
		[&minimap_draw_event](kompvter::MinimapListener* listener)
		{
			listener->on_draw_minimap(minimap_draw_event);
		});
}
