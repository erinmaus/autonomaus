// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <map>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/components/component.hpp"
#include "autonomaus/components/componentManager.hpp"
#include "autonomaus/components/state/brainComponent.hpp"
#include "autonomaus/core/isaac.hpp"
#include "autonomaus/graphics/gui.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/brain.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/state/memory/message.hpp"
#include "mapp/atlas/atlas.hpp"
#include "mapp/game/game.hpp"
#include "twoflower/brochure.hpp"

namespace autonomaus
{
	class BrainTest : public Isaac
	{
	public:
		BrainTest(Autonomaus& autonomaus, LibraryHandle handle);
		~BrainTest();

		void update() override;

	private:
		std::unique_ptr<Brain> brain;
		twoflower::Brochure brochure;
		mapp::Atlas atlas;
		mapp::Game game;

		bool init = false;
	};
}

#include "autonomaus/components/world/lootComponent.hpp"
#include "autonomaus/state/memory/actionBarCortex.hpp"
#include "autonomaus/state/memory/bankWindowCortex.hpp"
#include "autonomaus/state/memory/buffBarCortex.hpp"
#include "autonomaus/state/memory/dialogInputPromptCortex.hpp"
#include "autonomaus/state/memory/homeTeleportWindowCortex.hpp"
#include "autonomaus/state/memory/inventoryPanelCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/mashinaCortex.hpp"
#include "autonomaus/state/memory/makeXWindowCortex.hpp"
#include "autonomaus/state/memory/minimapPanelCortex.hpp"
#include "autonomaus/state/memory/progressWindowCortex.hpp"
#include "autonomaus/state/memory/ribbonCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/targetCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"
#include "autonomaus/state/memory/viewCortex.hpp"
#include "autonomaus/state/memory/walkerCortex.hpp"
#include "autonomaus/state/memory/worldCortex.hpp"

autonomaus::BrainTest::BrainTest(Autonomaus& autonomaus, LibraryHandle handle) :
	Isaac(autonomaus, handle),
	brain(std::make_unique<Brain>(autonomaus)),
	brochure(autonomaus.get_game_state().get_resource_manager().get_path() + "/database"),
	atlas(brochure),
	game(brochure)
{
	brain->add<MashinaCortex>();
	brain->add<ItemCortex>();
	brain->add<ViewCortex>();
	brain->add<WorldCortex>();
	brain->add<WalkerCortex>();
	brain->add<RightClickMenuCortex>();
	brain->add<ToolTipCortex>();
	brain->add<RibbonCortex>();
	brain->add<ActionBarCortex>();
	brain->add<MinimapPanelCortex>();
	brain->add<InventoryPanelCortex>();
	brain->add<DialogInputPromptCortex>();
	brain->add<MakeXWindowCortex>();
	brain->add<ProgressWindowCortex>();
	brain->add<BankWindowCortex>();
	brain->add<HomeTeleportWindowCortex>();
	brain->add<TargetCortex>();
	brain->add<BuffBarCortex>();

	get_autonomaus().add_userdata<Brain>(brain.get());
	get_autonomaus().add_userdata<twoflower::Brochure>(&brochure);
	get_autonomaus().add_userdata<mapp::Atlas>(&atlas);
	get_autonomaus().add_userdata<mapp::Game>(&game);
	get_autonomaus().get_component_manager().add<BrainComponent>();
	get_autonomaus().get_component_manager().add<LootComponent>();
}

autonomaus::BrainTest::~BrainTest()
{
	get_autonomaus().get_component_manager().remove<BrainComponent>();
	get_autonomaus().get_component_manager().remove<LootComponent>();
	get_autonomaus().remove_userdata<Brain>();
	brain.reset();

	get_autonomaus().remove_userdata<twoflower::Brochure>();
	get_autonomaus().remove_userdata<mapp::Atlas>();
	get_autonomaus().remove_userdata<mapp::Game>();
}

void autonomaus::BrainTest::update()
{
	this->brain->update();

	if (!init)
	{
		try
		{
			auto mashina = get_autonomaus().get_userdata<MashinaCortex>();
			mashina->require("kindermaus.tasks");


			Message m;
#if 1
			m.set_string("task", "Vindicta (Magic FTAA)");
			m.set_string("main_hand", "Seasinger kiba");
			m.set_string("off_hand", "Seasinger makigai");
			m.set_string("staff", "Noxious staff");
			m.set_string("spell", "Fire surge");
#endif
#if 0
			m.set_string("task", "Test (Combat, Magic FTAA Loop)");
#endif
#if 0
			m.set_string("task", "Mining (Varrock South-East Mine)");
			m.set_string("resource", "Tin ore");
#endif
#if 0
			m.set_string("task", "Ore Smelting (Lumbridge Furnace)");
			m.set_string("resource", "Tin ore", 0);
			m.set_string("resource", "Copper ore", 1);
			m.set_integer("count", 1, 0);
			m.set_integer("count", 1, 1);
			m.set_string("bar", "Bronze bar");
#endif
#if 0
			m.set_string("task", "Forging (Varrock West)");
			m.set_string("product", "Bronze dagger");
			m.set_string("bar", "Bronze bar");
#endif
			mashina->task(m);
		}
		catch(const std::exception& e)
		{
			Log::warn("Exception: {}", e.what());
		}

		init = true;
	}
}

extern "C"
autonomaus::Isaac* create_isaac_module(
	autonomaus::Autonomaus& autonomaus,
	autonomaus::Isaac::LibraryHandle handle)
{
	return new autonomaus::BrainTest(autonomaus, handle);
}

extern "C"
void destroy_isaac_module(autonomaus::Isaac* isaac)
{
	delete isaac;
}
