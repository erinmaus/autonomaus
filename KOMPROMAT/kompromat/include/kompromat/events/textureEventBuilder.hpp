// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_TEXTURE_UPDATE_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_TEXTURE_UPDATE_EVENT_BUILDER_HPP

#include <cstddef>
#include <string>
#include <glm/glm.hpp>
#include "kompvter/events/textureEvent.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class TextureEventBuilder
	{
	public:
		TextureEventBuilder() = default;
		virtual ~TextureEventBuilder() = default;

		const kompvter::TextureEvent& get_event() const;

		void from_texture_create(const paley::Event& e);
		void from_texture_delete(const paley::Event& e);
		void from_texture_allocate(const paley::Event& e);
		void from_texture_update(const paley::Event& e);
		void from_texture_copy(const paley::Event& e);

	private:
		void process_destination_name(const paley::EventField& value);
		void process_source_name(const paley::EventField& value);
		void process_component_format(
			const paley::EventField& root,
			const std::string& field);
		void process_component_type(
			const paley::EventField& root,
			const std::string& field);
		void process_compression_format(
			const paley::EventField& root,
			const std::string& field);
		void process_dimensions(
			const paley::EventField& root,
			const std::string& width_field,
			const std::string& height_field);
		void process_offset(
			const paley::EventField& root,
			int& result_x, int& result_y);

		kompvter::TextureEvent event;
		paley::EventField event_data;
	};
}

#endif
