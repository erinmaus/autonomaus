// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompromat/events/modelDrawEventBuilder.hpp"
#include "kompromat/processors/modelProcessor.hpp"

kompromat::ModelProcessor::ModelProcessor(
	kompvter::BufferManager& buffer_manager,
	kompvter::TextureManager& texture_manager)
	: buffer_manager(buffer_manager), texture_manager(texture_manager)
{
	// Nada.
}

void kompromat::ModelProcessor::process(
	const std::string& scope, int name, const paley::Event& event)
{
	if (scope == "ARDEN")
	{
		if (event.get_name() == "draw_model")
		{
			process_draw_model(event);
		}
		else if (event.get_name() == "draw_terrain")
		{
			process_draw_terrain(event);
		}
		else if (event.get_name() == "draw_water")
		{
			process_draw_water(event);
		}
	}
}

void kompromat::ModelProcessor::process_draw_model(const paley::Event& event)
{
	ModelDrawEventBuilder model_draw_event_builder(
		this->buffer_manager, this->texture_manager);
	model_draw_event_builder.from_draw_model(event);
	auto& model_draw_event = model_draw_event_builder.get_event();

	for_each(
		[&model_draw_event](kompvter::ModelListener* listener)
		{
			listener->on_draw_model(model_draw_event);
		});
}

void kompromat::ModelProcessor::process_draw_terrain(const paley::Event& event)
{
	ModelDrawEventBuilder model_draw_event_builder(
		this->buffer_manager, this->texture_manager);
	model_draw_event_builder.from_draw_model(event);
	auto& model_draw_event = model_draw_event_builder.get_event();

	for_each(
		[&model_draw_event](kompvter::ModelListener* listener)
		{
			listener->on_draw_terrain(model_draw_event);
		});
}

void kompromat::ModelProcessor::process_draw_water(const paley::Event& event)
{
	ModelDrawEventBuilder model_draw_event_builder(
		this->buffer_manager, this->texture_manager);
	model_draw_event_builder.from_draw_model(event);
	auto& model_draw_event = model_draw_event_builder.get_event();

	for_each(
		[&model_draw_event](kompvter::ModelListener* listener)
		{
			listener->on_draw_water(model_draw_event);
		});
}
