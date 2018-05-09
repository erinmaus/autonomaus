// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_ITEM_ICON_MANAGER_HPP
#define PRINCIPLE_RESOURCES_ITEM_ICON_MANAGER_HPP

#include <map>
#include <vector>
#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/listeners/itemIconListener.hpp"
#include "kompvter/listeners/textureListener.hpp"
#include "kvncer/graphics/shader.hpp"
#include "principle/graphics/display.hpp"
#include "principle/resources/textureManager.hpp"
#include "principle/resources/itemIcon.hpp"

namespace principle
{
	class ItemIconManager :
		public kompvter::ItemIconListener,
		public kompvter::TextureListener
	{
	public:
		ItemIconManager(TextureManager& texture_resource_manager);
		~ItemIconManager();

		std::size_t count() const;
		const ItemIcon& at(std::size_t index) const;
		bool empty() const;

		void reset();

		void on_draw_gui(const kompvter::GUIDrawEvent& event) override;
		void on_draw_model(const kompvter::ModelDrawEvent& event) override;
		void on_transfer(const kompvter::ItemIconEvent& event) override;
		void on_clear(const kompvter::ItemIconEvent& event) override;
		void on_texture_copy(const kompvter::TextureEvent& event) override;

	private:
		void transfer(
			int source_name,
			int source_x, int source_y,
			int destination_name,
			int destination_x, int destination_y,
			int width, int height);
		void finalize(int texture_name);

		TextureManager* texture_resource_manager;
		kvncer::Shader* draw_model_shader;
		kvncer::Shader* draw_gui_shader;
		kvncer::Shader* copy_shader;

		std::map<int /* texture */, int /* num_transfers */> current_transfers;

		typedef std::vector<ItemIcon> ItemIconCollection;
		ItemIconCollection item_icons;

		std::multimap<int /* texture_name */, kompvter::ModelDrawEvent> current_model_draws;
		void draw_model(const kompvter::ModelDrawEvent& event);

		std::multimap<int /* texture_name */, kompvter::GUIDrawEvent> current_gui_draws;
		void draw_gui(const kompvter::GUIDrawEvent& event);

		void draw_transfer(
			int source_name,
			kvncer::Texture* source_texture,
			int source_x, int source_y,
			int destination_name,
			kvncer::Texture* destination_texture,
			int destination_x, int destination_y,
			int width, int height);

	public:
		typedef ItemIconCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
