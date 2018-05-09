// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_WORLD_LOOT_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_WORLD_LOOT_COMPONENT_HPP

#include <map>
#include <set>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/components/component.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"
#include "glooper/item/loot.hpp"

namespace autonomaus
{
	class LootComponent : public Component
	{
	public:
		explicit LootComponent(Autonomaus& autonomaus);
		~LootComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();

		std::map<thoreau::TileCoordinate, GUIOverlay::Tag> loot_draws;
		void draw_loot();
	};
}

#endif
