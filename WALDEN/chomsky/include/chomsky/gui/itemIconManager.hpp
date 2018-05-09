// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_GUI_ITEM_ICON_MANAGER_HPP
#define CHOMSKY_GUI_ITEM_ICON_MANAGER_HPP

#include <map>
#include <vector>
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/itemIconSilhouette.hpp"
#include "chomsky/resources/tileSpacePointCloud.hpp"
#include "chomsky/world/worldState.hpp"
#include "chomsky/resources/textureDictionary.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/listeners/itemIconListener.hpp"
#include "kompvter/listeners/textureListener.hpp"

namespace chomsky
{
	class TextureKeyManager;

	class ItemIconManager :
		public kompvter::ItemIconListener,
		public kompvter::TextureListener
	{
	public:
		ItemIconManager(
			const kompvter::TextureManager& texture_manager,
			TextureKeyManager& texture_key_manager,
			TextureDictionary& texture_dictionary);
		~ItemIconManager() = default;

		void add_special(
			const std::string& entry,
			const ItemIconSilhouette& silhouette);
		void remove_special(const std::string& entry);
		bool has_special(const std::string& entry) const;

		bool has_item(const std::string& key);
		const ItemIconSilhouette& get_item(const std::string& key) const;

		void reset();

		void on_draw_gui(const kompvter::GUIDrawEvent& event) override;
		void on_draw_model(const kompvter::ModelDrawEvent& event) override;
		void on_transfer(const kompvter::ItemIconEvent& event) override;
		void on_clear(const kompvter::ItemIconEvent& event) override;
		void on_texture_update(const kompvter::TextureEvent& event) override;
		void on_texture_copy(const kompvter::TextureEvent& event) override;

	private:
		void flatten(int texture_name);
		void transfer(int source_name, int destination_name);
		void invalidate(
			int texture_name, int x, int y, int width, int height);

		GUIParser gui_parser;

		TextureKeyManager* texture_key_manager;
		const kompvter::TextureManager* texture_manager;
		TextureDictionary* texture_dictionary;

		std::map<std::string, ItemIconSilhouette> specials;

		std::multimap<int, std::string> current_specials;
		std::map<int, ModelVertexCloud> current_draws;
		std::map<int, ItemIconSilhouette> current_silhouettes;
		std::map<int, std::string> current_quantity;

		typedef chomsky::WorldState<
			thoreau::TileCoordinate,
			chomsky::TileSpacePointCloud>
		WorkingPointCloudMatch;
		std::map<int, WorkingPointCloudMatch> current_matches;

		struct ItemIcon
		{
			ItemIconSilhouette silhouette;
			std::string key;
			int x, y;
			int width, height;
		};
		std::multimap<int, ItemIcon> item_icons;
		std::map<std::string, ItemIcon*> item_icons_by_key;

		std::size_t current_item_id = 0;
	};
}

#endif
