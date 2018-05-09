// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/isaac.hpp"
#include "autonomaus/components/component.hpp"
#include "autonomaus/components/componentManager.hpp"
#include "autonomaus/components/state/brainComponent.hpp"
#include "autonomaus/components/world/lootComponent.hpp"
#include "autonomaus/state/memory/brain.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/state/memory/message.hpp"
#include "mysterimaus/scanComponent.hpp"

namespace autonomaus
{
	class MysterimausModule : public Isaac
	{
	public:
		MysterimausModule(Autonomaus& autonomaus, LibraryHandle handle);
		~MysterimausModule();

		void update() override;

	private:
		std::unique_ptr<Brain> brain;
	};
}

#include "autonomaus/state/memory/actionBarCortex.hpp"
#include "autonomaus/state/memory/bankWindowCortex.hpp"
#include "autonomaus/state/memory/buffBarCortex.hpp"
#include "autonomaus/state/memory/dialogInputPromptCortex.hpp"
#include "autonomaus/state/memory/homeTeleportWindowCortex.hpp"
#include "autonomaus/state/memory/inventoryPanelCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
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

autonomaus::MysterimausModule::MysterimausModule(Autonomaus& autonomaus, LibraryHandle handle) :
	Isaac(autonomaus, handle),
	brain(std::make_unique<Brain>(autonomaus))
{
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
	get_autonomaus().get_component_manager().add<BrainComponent>();
	get_autonomaus().get_component_manager().add<LootComponent>();
	get_autonomaus().get_component_manager().add<ScanComponent>();
}

autonomaus::MysterimausModule::~MysterimausModule()
{
	get_autonomaus().get_component_manager().remove<BrainComponent>();
	get_autonomaus().get_component_manager().remove<LootComponent>();
	get_autonomaus().get_component_manager().remove<ScanComponent>();
	get_autonomaus().remove_userdata<Brain>();
	brain.reset();
}

void autonomaus::MysterimausModule::update()
{
	brain->update();
}

extern "C"
autonomaus::Isaac* create_isaac_module(
	autonomaus::Autonomaus& autonomaus,
	autonomaus::Isaac::LibraryHandle handle)
{
	return new autonomaus::MysterimausModule(autonomaus, handle);
}

extern "C"
void destroy_isaac_module(autonomaus::Isaac* isaac)
{
	delete isaac;
}
