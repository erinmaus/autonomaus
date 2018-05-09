// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <map>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/core/time.hpp"
#include "autonomaus/components/component.hpp"
#include "autonomaus/components/componentManager.hpp"
#include "autonomaus/components/world/lootComponent.hpp"
#include "autonomaus/core/isaac.hpp"
#include "autonomaus/graphics/gui.hpp"
#include "autonomaus/state/memory/brain.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/state/memory/message.hpp"
#include "autonomaus/state/memory/bankWindowCortex.hpp"
#include "autonomaus/state/memory/dialogInputPromptCortex.hpp"
#include "autonomaus/state/memory/homeTeleportWindowCortex.hpp"
#include "autonomaus/state/memory/inventoryPanelCortex.hpp"
#include "autonomaus/state/memory/itemCortex.hpp"
#include "autonomaus/state/memory/minimapPanelCortex.hpp"
#include "autonomaus/state/memory/ribbonCortex.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"
#include "autonomaus/state/memory/toolTipCortex.hpp"
#include "autonomaus/state/memory/viewCortex.hpp"
#include "autonomaus/state/memory/walkerCortex.hpp"
#include "autonomaus/state/memory/worldCortex.hpp"

namespace autonomaus
{
	class WalkTestComponent : public Component
	{
	public:
		WalkTestComponent(Autonomaus& autonomaus);
		~WalkTestComponent();

		void reload() override;
		void show() override;
		void update() override;
		std::string get_name() const;

	private:
		bool visible = true;
		void draw();

		std::vector<std::pair<std::string, thoreau::TileCoordinate>> waypoints;

		enum
		{
			state_start,
			state_step,
			state_wait,
			state_done
		};
		int state = state_done;

		Cortex::Result result;
		float wait_threshold;

		void walk_to_tile(const thoreau::TileCoordinate& tile_coordinate);
	};

	class WalkTest : public Isaac
	{
	public:
		WalkTest(Autonomaus& autonomaus, LibraryHandle handle);
		~WalkTest();

		void update() override;

	private:
		Brain brain;
	};
}

autonomaus::WalkTestComponent::WalkTestComponent(Autonomaus& autonomaus) :
	Component(autonomaus)
{
	waypoints.emplace_back("Lumbridge Castle", thoreau::TileCoordinate(3222, 3219, 0));
	waypoints.emplace_back("Lumbridge Potato Farm", thoreau::TileCoordinate(3261, 3319, 0));
	waypoints.emplace_back("Lumbridge Chicken House", thoreau::TileCoordinate(3227, 3293, 0));
	waypoints.emplace_back("Varrock Castle (top floor)", thoreau::TileCoordinate(3202, 3497, 3));
	waypoints.emplace_back("Varrock Castle (training yard)", thoreau::TileCoordinate(3208, 3490, 2));
	waypoints.emplace_back("Piano", thoreau::TileCoordinate(3152, 3435, 1));
}

autonomaus::WalkTestComponent::~WalkTestComponent()
{
	if (this->result.pending())
	{
		this->result.cancel();
	}
}

void autonomaus::WalkTestComponent::reload()
{
	// Nothing.
}

void autonomaus::WalkTestComponent::show()
{
	this->visible = true;
}

void autonomaus::WalkTestComponent::update()
{
	auto walker = get_autonomaus().get_userdata<WalkerCortex>();
	auto view = get_autonomaus().get_userdata<ViewCortex>();

	if (walker != nullptr && view != nullptr)
	{
		switch (this->state)
		{
			case state_start:
				if (!this->result.pending())
				{
					if (this->result.get() == Cortex::Result::Status::success)
					{
						this->result = walker->step();
						this->state = state_step;
					}
					else
					{
						Log::warn("Walking failed.");
						this->state = state_done;
					}
				}
				break;
			case state_step:
				if (!this->result.pending())
				{
					if (walker->at_destination(4.0f))
					{
						Log::info("Arrived at destination.");

						this->state = state_done;
						break;
					}

					if (this->result.get() == Cortex::Result::Status::success)
					{
						this->result = view->focus_x(glm::radians(135.0f), glm::radians(12.5f));
						this->wait_threshold = get_current_time();// + 0.6f;
					}
					else
					{
						if (walker->pending())
						{
							this->wait_threshold = get_current_time();
						}
						else
						{
							Log::warn("Walking failed.");
							this->state = state_done;
							break;
						}
					}

					this->state = state_wait;
				}
				break;
			case state_wait:
				if (get_current_time() > this->wait_threshold && !this->result.pending())
				{
					this->result = walker->step();
					this->state = state_step;
				}
				break;
			case state_done:
				// Nothing.
				break;
		}
	}

	if (this->visible)
	{
		draw();
	}
}

std::string autonomaus::WalkTestComponent::get_name() const
{
	return "Walk Text #2";
}

void autonomaus::WalkTestComponent::draw()
{
	ImGui::Begin("Walk Test #2", &this->visible);
	{
		if (this->state != state_done && ImGui::Button("Stop"))
		{
			this->result.cancel();
			this->state = state_done;
		}

		ImGui::Text("Walk to...");
		for (auto& i: this->waypoints)
		{
			ImGui::BulletText("%s", i.first.c_str());
			{
				ImGui::PushID(i.first.c_str());
				ImGui::Indent();
				if (ImGui::Button("Start"))
				{
					walk_to_tile(i.second);
				}

				ImGui::Unindent();
				ImGui::PopID();
			}
		}
	}
	ImGui::End();
}

void autonomaus::WalkTestComponent::walk_to_tile(
	const thoreau::TileCoordinate& tile_coordinate)
{
	if (this->result.pending())
	{
		this->result.cancel();
	}

	auto walker = get_autonomaus().get_userdata<WalkerCortex>();
	if (walker != nullptr)
	{
		this->result = walker->find_path(tile_coordinate);
		this->state = state_start;
	}
	else
	{
		this->state = state_done;
	}
}

autonomaus::WalkTest::WalkTest(Autonomaus& autonomaus, LibraryHandle handle) :
	Isaac(autonomaus, handle),
	brain(autonomaus)
{
	brain.add<ItemCortex>();
	brain.add<ViewCortex>();
	brain.add<WorldCortex>();
	brain.add<WalkerCortex>();
	brain.add<RightClickMenuCortex>();
	brain.add<ToolTipCortex>();
	brain.add<RibbonCortex>();
	brain.add<MinimapPanelCortex>();
	brain.add<InventoryPanelCortex>();
	brain.add<DialogInputPromptCortex>();
	brain.add<BankWindowCortex>();
	brain.add<HomeTeleportWindowCortex>();

	get_autonomaus().add_userdata<Brain>(&brain);
	get_autonomaus().get_component_manager().add<WalkTestComponent>();
	get_autonomaus().get_component_manager().add<LootComponent>();
}

autonomaus::WalkTest::~WalkTest()
{
	get_autonomaus().get_component_manager().remove<WalkTestComponent>();
	get_autonomaus().get_component_manager().remove<LootComponent>();
	get_autonomaus().remove_userdata<Brain>();
}

void autonomaus::WalkTest::update()
{
	this->brain.update();
}

extern "C"
autonomaus::Isaac* create_isaac_module(
	autonomaus::Autonomaus& autonomaus,
	autonomaus::Isaac::LibraryHandle handle)
{
	return new autonomaus::WalkTest(autonomaus, handle);
}

extern "C"
void destroy_isaac_module(autonomaus::Isaac* isaac)
{
	delete isaac;
}
