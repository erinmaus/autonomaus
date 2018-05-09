// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_WORLD_NPC_CABINET_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_WORLD_NPC_CABINET_COMPONENT_HPP

#include <map>
#include <set>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "autonomaus/components/component.hpp"
#include "autonomaus/graphics/worldOverlay.hpp"
#include "autonomaus/graphics/guiOverlay.hpp"
#include "glooper/npc/npc.hpp"

namespace autonomaus
{
	class NPCCabinetComponent : public Component
	{
	public:
		explicit NPCCabinetComponent(Autonomaus& autonomaus);
		~NPCCabinetComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();

		std::string pending_profile;
		std::set<std::string> profiles;
		typedef std::pair<GUIOverlay::Tag, WorldOverlay::Tag> NPCDraw;
		std::map<glooper::NPC::ID, NPCDraw> npc_draws;
		void draw_npcs();
	};
}

#endif
