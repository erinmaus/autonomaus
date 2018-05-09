// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_ITEM_ICON_HPP
#define PRINCIPLE_RESOURCES_ITEM_ICON_HPP

#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/world/query/tileSpacePointCloudMatch.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kvncer/graphics/texture.hpp"
#include "principle/graphics/thumbnail.hpp"

namespace principle
{
	class ItemIcon
	{
	public:
		friend class ItemIconManager;

		ItemIcon() = default;
		ItemIcon(ItemIcon&& other) = default;
		~ItemIcon() = default;

		std::size_t get_num_draws() const;
		const kompvter::ModelDrawEvent& get_draw(std::size_t index) const;

		const chomsky::ItemIconSilhouette& get_silhouette() const;
		const chomsky::TileSpacePointCloudMatch& get_point_cloud_match() const;

		kvncer::Texture* get_texture() const;

		void to_thumbnail(Thumbnail& thumbnail) const;

	private:
		std::vector<kompvter::ModelDrawEvent> draws;
		chomsky::ItemIconSilhouette silhouette;
		chomsky::TileSpacePointCloudMatch point_cloud_match;
		std::unique_ptr<kvncer::Texture> texture;
	};
}

#endif
