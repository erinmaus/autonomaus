// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_ELEMENT_STREAM_HPP
#define CHOMSKY_GUI_ELEMENT_STREAM_HPP

#include <vector>
#include <stack>
#include "chomsky/gui/guiElement.hpp"
#include "chomsky/resources/textureKeyManager.hpp"
#include "kompvter/graphics/drawParameters.hpp"
#include "kompvter/graphics/texture.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"
#include "kvre/ui/flatDrawBuffer.hpp"

namespace chomsky
{
	class GUIElementStream
	{
	public:
		GUIElementStream(
			const kompvter::TextureManager& texture_manager,
			const TextureKeyManager& texture_key_manager);
		~GUIElementStream() = default;

		void update(
			const kompvter::Texture& texture,
			const kompvter::VertexIndexMapper& index_mapper,
			const kompvter::DrawParameters& draw_parameters,
			const kvre::FlatDrawBuffer& flat_buffer);
		void refresh();
		void clear();

		std::size_t size() const;
		const GUIElement& at(std::size_t index) const;

		std::size_t tell() const;
		void seek(std::size_t offset);

		void push();
		void pop(bool keep_position);
		void rewind();

		bool next(GUIElement& result);
		bool is_end_of_stream() const;

	private:
		const kompvter::TextureManager* texture_manager;
		const TextureKeyManager* texture_key_manager;

		std::vector<GUIElement> elements;
		std::size_t current_position = 0;

		std::stack<std::size_t> positions;

	public:
		typedef std::vector<GUIElement>::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
