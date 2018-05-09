// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_MAP_MINIMAP_HPP
#define GLOOPER_MAP_MINIMAP_HPP

#include <string>
#include <boost/container/flat_map.hpp>
#include <glm/glm.hpp>
#include "chomsky/gui/guiElementStream.hpp"
#include "thoreau/map.hpp"
#include "thoreau/tileCoordinate.hpp"
#include "thoreau/sectorCoordinate.hpp"

namespace glooper
{
	class Minimap
	{
	public:
		friend class MinimapManager;

		Minimap() = default;
		~Minimap() = default;

		bool get_is_visible() const;
		float get_rotation() const;
		const glm::vec2& get_scale() const;
		const glm::vec2& get_center() const;
		const glm::vec2& get_size() const;
		const glm::vec2& get_visible_size() const;
		const glm::vec2& get_position() const;

		glm::vec2 minimap_to_subtile(glm::vec2 position) const;
		glm::vec2 subtile_to_minimap(glm::vec2 subtile_position) const;

		glm::vec2 tile_to_minimap(const thoreau::TileCoordinate& tile_coordinate) const;
		thoreau::TileCoordinate minimap_to_tile(const glm::vec2& position) const;
		bool is_tile_visible(const thoreau::TileCoordinate& tile_coordinate) const;

		glm::vec3 minimap_to_world(const glm::vec2& position) const;
		glm::vec2 world_to_minimap(const glm::vec3& world_position) const;

		thoreau::TileCoordinate get_center_tile() const;
		thoreau::SectorCoordinate get_current_sector() const;

		void extract(thoreau::Map& map, int layer) const;

	private:
		typedef boost::container::flat_multimap<std::string, glm::vec2> FeatureCollection;
		FeatureCollection features;

		bool is_visible = false;
		float rotation = 0.0f;
		glm::vec2 scale = glm::vec2(1.0f);
		glm::vec2 center = glm::vec2(0.0f);
		glm::vec2 position = glm::vec2(0.0f);
		glm::vec2 size = glm::vec2(0.0f);
		glm::vec2 visible_size = glm::vec2(0.0f);
		glm::vec2 translation = glm::vec2(0.0f);

		thoreau::Map working_map;

	public:
		typedef FeatureCollection::const_iterator const_features_iterator;

		// Feature position is relative to minimap.
		const_features_iterator features_begin() const;
		const_features_iterator features_begin(const std::string& feature) const;
		const_features_iterator features_end() const;
		const_features_iterator features_end(const std::string& feature) const;
	};
}

#endif
