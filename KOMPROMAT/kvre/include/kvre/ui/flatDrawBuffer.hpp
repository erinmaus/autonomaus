// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVRE_UI_FLAT_DRAW_BUFFER_HPP
#define KVRE_UI_FLAT_DRAW_BUFFER_HPP

#include <vector>
#include <glm/glm.hpp>
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/graphics/texture.hpp"
#include "kompvter/graphics/vertexAttributes.hpp"
#include "kvre/profile/profile.hpp"

namespace kvre
{
	class FlatDrawBuffer
	{
	public:
		FlatDrawBuffer(
			const kompvter::VertexAttributes& attributes,
			const Profile* profile = nullptr);
		FlatDrawBuffer(
			const kompvter::GUIDrawEvent& event,
			const Profile* profile = nullptr);
		FlatDrawBuffer(const FlatDrawBuffer& other);
		~FlatDrawBuffer() = default;

		const kompvter::VertexAttributes& get_vertex_attributes() const;
		const Profile* get_profile() const;

		std::size_t get_num_vertices() const;

		glm::vec2 get_position(std::size_t index) const;
		glm::vec4 get_color(std::size_t index) const;
		glm::vec2 get_texture_coordinate(std::size_t index) const;
		glm::vec2 get_texture_atlas_min(std::size_t index) const;
		glm::vec2 get_texture_atlas_max(std::size_t index) const;
		bool is_textured(std::size_t index) const;
		glm::vec2 compute_wrapped_texture_coordinate(std::size_t index) const;

		FlatDrawBuffer& operator =(const FlatDrawBuffer& other);

	private:
		kompvter::VertexAttributes vertex_attributes;
		const Profile* profile;

		const kompvter::VertexFetcher* positions_fetcher;
		const kompvter::VertexFetcher* colors_fetcher;
		const kompvter::VertexFetcher* texture_coordinates_fetcher;
		const kompvter::VertexFetcher* atlas_mins_fetcher;
		const kompvter::VertexFetcher* atlas_maxes_fetcher;

		mutable std::vector<glm::vec2> positions;
		mutable std::vector<glm::vec2> texture_coordinates;
		mutable std::vector<glm::vec2> texture_atlas_mins;
		mutable std::vector<glm::vec2> texture_atlas_maxes;
		mutable std::vector<glm::vec4> colors;
		mutable std::vector<int> textured_vertices;

		std::size_t min_vertex, max_vertex;

		mutable std::size_t num_vertices = 0;
	};
}

#endif
