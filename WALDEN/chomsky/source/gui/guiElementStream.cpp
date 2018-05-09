// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/gui/guiElementStream.hpp"

chomsky::GUIElementStream::GUIElementStream(
	const kompvter::TextureManager& texture_manager,
	const TextureKeyManager& texture_key_manager)
{
	this->texture_manager = &texture_manager;
	this->texture_key_manager = &texture_key_manager;
}

void chomsky::GUIElementStream::update(
	const kompvter::Texture& texture,
	const kompvter::VertexIndexMapper& index_mapper,
	const kompvter::DrawParameters& draw_parameters,
	const kvre::FlatDrawBuffer& flat_buffer)
{
	bool is_multiple_of_six_start = ((draw_parameters.get_offset_start() / index_mapper.get_index_size()) % 6) == 0;
	bool is_multiple_of_six_stop = ((draw_parameters.get_offset_stop() / index_mapper.get_index_size()) % 6) == 0;
	if (!is_multiple_of_six_stop || !is_multiple_of_six_start)
	{
		return;
	}

	std::size_t start = draw_parameters.get_offset_start() / index_mapper.get_index_size() / 6 * 4;
	std::size_t stop = draw_parameters.get_offset_stop() / index_mapper.get_index_size() / 6 * 4;

	std::size_t num_elements = (stop - start) + 1;
	this->elements.reserve(this->elements.size() + num_elements);

	GUIElement::Vertex vertices[GUIElement::NUM_VERTICES];
	for (std::size_t i = start; i < stop; i += GUIElement::NUM_VERTICES)
	{
		assert(i < flat_buffer.get_num_vertices());

		float s_min = HUGE_VALF, s_max = -HUGE_VALF;
		float t_min = HUGE_VALF, t_max = -HUGE_VALF;
		bool is_textured = true;
		for (std::size_t j = 0; j < GUIElement::NUM_VERTICES; ++j)
		{
			std::size_t index = i + j;
			vertices[j].position = flat_buffer.get_position(index);
			vertices[j].atlas_min = flat_buffer.get_texture_atlas_min(index);
			vertices[j].atlas_max = flat_buffer.get_texture_atlas_max(index);
			vertices[j].texture_coordinate = flat_buffer.get_texture_coordinate(index);
			vertices[j].wrapped_texture_coordinate = flat_buffer.compute_wrapped_texture_coordinate(index);
			vertices[j].color = flat_buffer.get_color(index);

			s_min = glm::min(vertices[j].wrapped_texture_coordinate.s, s_min);
			t_min = glm::min(vertices[j].wrapped_texture_coordinate.t, t_min);
			s_max = glm::max(vertices[j].wrapped_texture_coordinate.s, s_max);
			t_max = glm::max(vertices[j].wrapped_texture_coordinate.t, t_max);

			if (!flat_buffer.is_textured(index))
			{
				vertices[j].texture_coordinate = glm::vec2(0.0f);
				is_textured = false;
			}
		}

		int region_name = 0;
		std::string key;
		if (is_textured)
		{
			int left = s_min * texture.get_width();
			int right = s_max * texture.get_width();
			int top = t_max * texture.get_height();
			int bottom = t_min * texture.get_height();
			auto region = texture.get_region_from_pixel(
				left + (right - left) / 2,
				bottom + (top - bottom) / 2);
			if (region)
			{
				region_name = region->get_name();
				key = this->texture_key_manager->get(
					texture.get_name(), region->get_name());
			}
		}

		GUIElement element(
			vertices, GUIElement::NUM_VERTICES,
			this->elements.size(),
			key, texture.get_name(), region_name, is_textured);
		this->elements.push_back(element);
	}
}

void chomsky::GUIElementStream::refresh()
{
	for (auto& element: this->elements)
	{
		if (element.get_is_textured())
		{
			auto key = this->texture_key_manager->get(
				element.get_texture_name(),
				element.get_texture_region_name());
			element.set_key(key);
		}
	}
}

void chomsky::GUIElementStream::clear()
{
	this->elements.clear();
	this->current_position = 0;
}

std::size_t chomsky::GUIElementStream::size() const
{
	return this->elements.size();
}

const chomsky::GUIElement& chomsky::GUIElementStream::at(std::size_t index) const
{
	return this->elements.at(index);
}

std::size_t chomsky::GUIElementStream::tell() const
{
	return this->current_position;
}

void chomsky::GUIElementStream::seek(std::size_t offset)
{
	this->current_position = std::min(offset, elements.size());
}

void chomsky::GUIElementStream::push()
{
	this->positions.push(this->current_position);
}

void chomsky::GUIElementStream::rewind()
{
	if (this->positions.empty())
	{
		this->current_position = 0;
	}
	else
	{
		this->current_position = this->positions.top();
	}
}

void chomsky::GUIElementStream::pop(bool keep_position)
{
	assert(!this->positions.empty());

	if (!keep_position)
	{
		this->current_position = this->positions.top();
	}
	this->positions.pop();
}

bool chomsky::GUIElementStream::next(GUIElement& result)
{
	if (this->current_position < elements.size())
	{
		result = elements[this->current_position];
		++this->current_position;

		return true;
	}

	return false;
}

bool chomsky::GUIElementStream::is_end_of_stream() const
{
	assert(this->current_position <= this->elements.size());
	return this->current_position == this->elements.size();
}

chomsky::GUIElementStream::const_iterator
chomsky::GUIElementStream::begin() const
{
	return this->elements.begin();
}

chomsky::GUIElementStream::const_iterator
chomsky::GUIElementStream::end() const
{
	return this->elements.end();
}
