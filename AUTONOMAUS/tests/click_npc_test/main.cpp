// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/isaac.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/components/component.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/input/gestures/mousePressGesture.hpp"
#include "autonomaus/input/gestures/worldMouseMoveGesture.hpp"
#include "autonomaus/state/camera.hpp"
#include "autonomaus/state/npcProvider.hpp"
#include "glooper/npc/npc.hpp"

class ClickNPCTestComponent : public autonomaus::Component
{
public:
	ClickNPCTestComponent(autonomaus::Autonomaus& autonomaus) :
		autonomaus::Component(autonomaus)
	{
		// Nothing.
	}

	void reload()
	{
		// Nothing.
	}

	void show()
	{
		this->visible = true;
	}

	void update()
	{
		if (this->visible)
		{
			draw();
		}

		if (!this->gestures.empty())
		{
			auto current = this->gestures.front();
			if (!current->pending())
			{
				delete current;
				this->gestures.pop();

				if (!this->gestures.empty())
				{
					auto next = this->gestures.front();
					int result = next->play();
					printf("play: %d\n", result);
				}
			}
		}
	}

	std::string get_name() const
	{
		return "Click NPC Test";
	}

private:
	bool visible = false;
	void draw()
	{
		ImGui::Begin("Click NPC Test", &this->visible);
		{
			auto& npcs = get_autonomaus().get_npc_state();
			for (auto npc: *npcs)
			{
				ImGui::PushID(npc.first);
				ImGui::Text(
					"% 4d (%s.%s)",
					npc.first,
					npc.second.get_profile_name().c_str(),
					npc.second.get_profile_variant().c_str());

				if (ImGui::Button("RClick"))
				{
					click(eiznebahn::MouseButton::button_right, npc.second);
				}

				ImGui::SameLine();
				if (ImGui::Button("LClick"))
				{
					click(eiznebahn::MouseButton::button_left, npc.second);
				}

				ImGui::PopID();
			}
		}
		ImGui::End();
	}

	std::queue<autonomaus::Gesture*> gestures;
	void click(int button, glooper::NPC& npc)
	{
		auto elevation = glm::vec3(0.0f, npc.get_bounds().compute_size().y / 2.0f, 0.0f);
		auto position = npc.get_position() + elevation;
		auto movement = new autonomaus::WorldMouseMoveGesture(
			get_autonomaus().get_input_manager(),
			position,
			autonomaus::Camera(get_autonomaus().get_draw_state()));
		auto press = new autonomaus::MousePressGesture(
			get_autonomaus().get_input_manager(),
			button);

		this->gestures.push(movement);
		this->gestures.push(press);

		int result = movement->play();
		printf("play: %d\n", result);
	}
};

class ClickNPCTest : public autonomaus::Isaac
{
public:
	ClickNPCTest(autonomaus::Autonomaus& autonomaus, LibraryHandle handle) :
		autonomaus::Isaac(autonomaus, handle)
	{
		auto& component_manager = autonomaus.get_component_manager();
		component_manager.add<ClickNPCTestComponent>();
	}

	~ClickNPCTest()
	{
		auto& component_manager = get_autonomaus().get_component_manager();
		component_manager.remove<ClickNPCTestComponent>();
	}

	void update() override
	{
		// Nothing.
	}
};

extern "C"
autonomaus::Isaac* create_isaac_module(
	autonomaus::Autonomaus& autonomaus,
	autonomaus::Isaac::LibraryHandle handle)
{
	return new ClickNPCTest(autonomaus, handle);
}

extern "C"
void destroy_isaac_module(autonomaus::Isaac* isaac)
{
	delete isaac;
}
