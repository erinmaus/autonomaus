// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompromat/events/textureEventBuilder.hpp"
#include "kompromat/processors/textureProcessor.hpp"

void kompromat::TextureProcessor::process(
	const std::string& scope,
	int,
	const paley::Event& event)
{
	if (scope == "ARDEN")
	{
		if (event.get_name() == "texture_create")
		{
			process_texture_create(event);
		}
		else if (event.get_name() == "texture_delete")
		{
			process_texture_delete(event);
		}
		else if (event.get_name() == "texture_allocate")
		{
			process_texture_allocate(event);
		}
		else if (event.get_name() == "texture_update")
		{
			process_texture_update(event);
		}
		else if (event.get_name() == "texture_copy")
		{
			process_texture_copy(event);
		}
	}
}

void kompromat::TextureProcessor::process_texture_create(const paley::Event& event)
{
	TextureEventBuilder texture_event_builder;
	texture_event_builder.from_texture_create(event);
	auto& texture_event = texture_event_builder.get_event();

	for_each(
		[&texture_event](kompvter::TextureListener* listener)
		{
			listener->on_texture_create(texture_event);
		});
}

void kompromat::TextureProcessor::process_texture_delete(const paley::Event& event)
{
	TextureEventBuilder texture_event_builder;
	texture_event_builder.from_texture_delete(event);
	auto& texture_event = texture_event_builder.get_event();

	for_each(
		[&texture_event](kompvter::TextureListener* listener)
		{
			listener->on_texture_delete(texture_event);
		});
}

void kompromat::TextureProcessor::process_texture_allocate(const paley::Event& event)
{
	TextureEventBuilder texture_event_builder;
	texture_event_builder.from_texture_allocate(event);
	auto& texture_event = texture_event_builder.get_event();

	for_each(
		[&texture_event](kompvter::TextureListener* listener)
		{
			listener->on_texture_allocate(texture_event);
		});
}

void kompromat::TextureProcessor::process_texture_update(const paley::Event& event)
{
	TextureEventBuilder texture_event_builder;
	texture_event_builder.from_texture_update(event);
	auto& texture_event = texture_event_builder.get_event();

	for_each(
		[&texture_event](kompvter::TextureListener* listener)
		{
			listener->on_texture_update(texture_event);
		});
}

void kompromat::TextureProcessor::process_texture_copy(const paley::Event& event)
{
	TextureEventBuilder texture_event_builder;
	texture_event_builder.from_texture_copy(event);
	auto& texture_event = texture_event_builder.get_event();

	for_each(
		[&texture_event](kompvter::TextureListener* listener)
		{
			listener->on_texture_copy(texture_event);
		});
}
