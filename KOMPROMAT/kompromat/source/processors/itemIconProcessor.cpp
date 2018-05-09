// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompromat/events/guiDrawEventBuilder.hpp"
#include "kompromat/events/itemIconEventBuilder.hpp"
#include "kompromat/events/modelDrawEventBuilder.hpp"
#include "kompromat/processors/itemIconProcessor.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/itemIconEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"

kompromat::ItemIconProcessor::ItemIconProcessor(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager)
	: buffer_manager(buffer_manager),
	texture_manager(texture_manager)
{
	// Nada.
}

void kompromat::ItemIconProcessor::process(
	const std::string& scope, int name, const paley::Event& event)
{
	if (scope == "ARDEN")
	{
		if (event.get_name() == "draw_item_icon")
		{
			process_draw_item_icon(event);
		}
		else if (event.get_name() == "draw_gui_item_icon")
		{
			process_draw_gui_item_icon(event);
		}
		else if (event.get_name() == "transfer_item_icon_texture")
		{
			process_transfer_item_icon_texture(event);
		}
		else if (event.get_name() == "clear_item_icon_buffer")
		{
			process_clear_item_icon_buffer(event);
		}
	}
}

void kompromat::ItemIconProcessor::process_draw_gui_item_icon(
	const paley::Event& event)
{
	GUIDrawEventBuilder gui_draw_event_builder(
		this->buffer_manager, this->texture_manager);
	gui_draw_event_builder.from_draw_gui(event);
	auto& gui_draw_event = gui_draw_event_builder.get_event();

	for_each(
		[&gui_draw_event](kompvter::ItemIconListener* listener)
		{
			listener->on_draw_gui(gui_draw_event);
		});
}

void kompromat::ItemIconProcessor::process_draw_item_icon(
	const paley::Event& event)
{
	ModelDrawEventBuilder model_draw_event_builder(
		this->buffer_manager, this->texture_manager);
	model_draw_event_builder.from_draw_model(event);
	auto& model_draw_event = model_draw_event_builder.get_event();

	for_each(
		[&model_draw_event](kompvter::ItemIconListener* listener)
		{
			listener->on_draw_model(model_draw_event);
		});
}

void kompromat::ItemIconProcessor::process_transfer_item_icon_texture(
	const paley::Event& event)
{
	ItemIconEventBuilder item_icon_event_builder(this->texture_manager);
	item_icon_event_builder.from_transfer_item_icon_texture(event);
	auto& item_icon_event = item_icon_event_builder.get_event();

	for_each(
		[&item_icon_event](kompvter::ItemIconListener* listener)
		{
			listener->on_transfer(item_icon_event);
		});
}

void kompromat::ItemIconProcessor::process_clear_item_icon_buffer(
	const paley::Event& event)
{
	ItemIconEventBuilder item_icon_event_builder(this->texture_manager);
	item_icon_event_builder.from_clear_item_icon_texture(event);
	auto& item_icon_event = item_icon_event_builder.get_event();

	for_each(
		[&item_icon_event](kompvter::ItemIconListener* listener)
		{
			listener->on_clear(item_icon_event);
		});
}
