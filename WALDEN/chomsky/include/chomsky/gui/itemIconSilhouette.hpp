// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_ITEM_ICON_SILHOUETTE_HPP
#define CHOMSKY_GUI_ITEM_ICON_SILHOUETTE_HPP

#include <glm/glm.hpp>
#include "chomsky/io/gui/itemIconSilhouette.hpp"
#include "chomsky/resources/silhouette.hpp"
#include "chomsky/resources/modelVertexCloud.hpp"
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"

namespace chomsky
{
	class ItemIconSilhouette
	{
	public:
		friend class ItemIconManager;

		friend class mantorok::DATSerializer<ItemIconSilhouette>;
		friend class mantorok::DATDeserializer<ItemIconSilhouette>;

		static const int WIDTH = 4;
		static const int HEIGHT = 4;
		static const int DEPTH = 1;

		ItemIconSilhouette();
		~ItemIconSilhouette() = default;

		int get_width() const;
		int get_height() const;
		int get_depth() const;

		const Silhouette& get_weight() const;
		const Silhouette& get_red() const;
		const Silhouette& get_green() const;
		const Silhouette& get_blue() const;

		const TileSpacePointCloudMatch& get_point_cloud_match() const;

		void project(const ModelVertexCloud& cloud);
		void merge(const ItemIconSilhouette& other);

		std::string to_string() const;

		static bool match(
			const ItemIconSilhouette& a,
			const ItemIconSilhouette& b);

	private:
		void clear();
		void normalize_colors();

		Silhouette weight, red, green, blue;
		TileSpacePointCloudMatch point_cloud_match;
	};
}

#endif
