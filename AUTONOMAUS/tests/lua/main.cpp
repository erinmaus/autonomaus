// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/bindings/lua.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/components/world/lootComponent.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/memory/brain.hpp"
#include "autonomaus/state/memory/bankWindowCortex.hpp"
#include "autonomaus/state/memory/dialogInputPromptCortex.hpp"
#include "autonomaus/state/memory/homeTeleportWindowCortex.hpp"
#include "autonomaus/state/memory/inventoryPanelCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/mashinaCortex.hpp"
#include "autonomaus/state/memory/minimapPanelCortex.hpp"
#include "autonomaus/state/memory/ribbonCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"
#include "autonomaus/state/memory/viewCortex.hpp"
#include "autonomaus/state/memory/walkerCortex.hpp"
#include "autonomaus/state/memory/worldCortex.hpp"
#include "mapp/atlas/atlas.hpp"
#include "mapp/game/game.hpp"
#include "touchstone/gl/immediateDispatch.hpp"
#include "twoflower/brochure.hpp"

int main(int argc, const char* argv[])
{
	touchstone::ImmediateDispatch dispatch;
	autonomaus::Autonomaus* autonomaus = (autonomaus::Autonomaus*)ganymede_create_plugin(&dispatch);
	autonomaus::Brain brain(*autonomaus);
	brain.add<autonomaus::MashinaCortex>();
	brain.add<autonomaus::ItemCortex>();
	brain.add<autonomaus::ViewCortex>();
	brain.add<autonomaus::WorldCortex>();
	brain.add<autonomaus::WalkerCortex>();
	brain.add<autonomaus::RightClickMenuCortex>();
	brain.add<autonomaus::ToolTipCortex>();
	brain.add<autonomaus::RibbonCortex>();
	brain.add<autonomaus::MinimapPanelCortex>();
	brain.add<autonomaus::InventoryPanelCortex>();
	brain.add<autonomaus::DialogInputPromptCortex>();
	brain.add<autonomaus::BankWindowCortex>();
	brain.add<autonomaus::HomeTeleportWindowCortex>();

	autonomaus->add_userdata<autonomaus::Brain>(&brain);

	twoflower::Brochure brochure(autonomaus->get_game_state().get_resource_manager().get_path() + "/database");
	mapp::Atlas atlas(brochure);
	mapp::Game game(brochure);

	autonomaus->add_userdata<twoflower::Brochure>(&brochure);
	autonomaus->add_userdata<mapp::Atlas>(&atlas);
	autonomaus->add_userdata<mapp::Game>(&game);

	auto mashina = autonomaus->get_userdata<autonomaus::MashinaCortex>();

	for (int i = 1; i < argc; ++i)
	{
		mashina->do_file(argv[i]);
	}

	for (int i = 0; i < 1000; ++i)
	{
		mashina->update();
	}

	return 0;
}
