// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_MAP_MINIMAP_MANAGER_HPP
#define GLOOPER_MAP_MINIMAP_MANAGER_HPP

#include <set>
#include <string>
#include <map>
#include <glm/glm.hpp>
#include "chomsky/gui/guiParser.hpp"
#include "chomsky/gui/guiElementStream.hpp"
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/events/minimapDrawEvent.hpp"
#include "kompvter/events/textureEvent.hpp"
#include "kompvter/events/viewportEvent.hpp"
#include "kompvter/listeners/frameListener.hpp"
#include "kompvter/listeners/guiListener.hpp"
#include "kompvter/listeners/minimapListener.hpp"
#include "kompvter/listeners/textureListener.hpp"
#include "glooper/map/minimap.hpp"
#include "glooper/resources/resourceListener.hpp"
#include "glooper/resources/resourceManager.hpp"

namespace glooper
{
	class MinimapManager :
		public ResourceListener,
		public kompvter::FrameListener,
		public kompvter::GUIListener,
		public kompvter::MinimapListener,
		public kompvter::TextureListener
	{
	public:
		MinimapManager(
			ResourceManager& resource_manager,
			Minimap& minimap,
			bool extract_walls_etc = false);
		MinimapManager(const MinimapManager& other) = delete;
		~MinimapManager();

		void on_reload_resources() override;
		void on_draw_frame(float timestamp) override;
		void on_draw_enabled(float timestamp) override;
		void on_draw_disabled(float timestamp) override;
		void on_draw_gui(const kompvter::GUIDrawEvent& event) override;
		void on_set_viewport(const kompvter::ViewportEvent& event) override;
		void on_draw_minimap(const kompvter::MinimapDrawEvent& event) override;
		void on_texture_delete(const kompvter::TextureEvent& event) override;
		void on_texture_clear(const kompvter::TextureEvent& event) override;

		MinimapManager& operator =(const MinimapManager& other) = delete;

	private:
		void load_resources();

		void invalidate(int texture);

		void transfer_minimap(const kompvter::GUIDrawEvent& event);
		void build_working_map(const kompvter::Texture& working_texture);

		bool extract_walls_etc;
		void extract_ladders(const kompvter::Texture& working_texture);
		void extract_walls(const kompvter::Texture& working_texture);

		void extract_minimap_features(const kompvter::Texture& finalized_texture);

		glm::ivec2 get_bottom_left_tile(const kompvter::Texture& working_texture);
		glm::ivec2 get_sector_bounds(const kompvter::Texture& working_texture);

		ResourceManager* resource_manager;
		Minimap* minimap;

		int previous_minimap_texture = 0;
		bool draw_enabled = false;

		struct DrawProperties
		{
			float rotation;
			glm::vec2 size;
			glm::vec2 visible_size;
			glm::vec2 scale;
			glm::vec2 center;
		};

		std::set<int> working_minimap_textures;
		std::map<int, kvlc::Bounds> working_minimap_bounds;
		std::multimap<int, kompvter::GUIDrawEvent> working_minimap_draws;
		std::set<int> dirty_working_minimap_textures;
		std::set<int> finalized_minimap_textures;
		std::map<int, chomsky::GUIElementStream> finalized_minimap_element_streams;
		std::map<int /* finalized */, DrawProperties> finalized_draw_properties;
		std::map<int /* finalized */, kompvter::Texture> minimap_transfers;
		glm::ivec4 viewport;
	};
}

#endif
