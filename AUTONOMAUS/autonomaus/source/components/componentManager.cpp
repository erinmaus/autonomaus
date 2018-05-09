// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/components/componentManager.hpp"
#include "autonomaus/deps/imgui/imgui.h"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/state/gameStateManager.hpp"

autonomaus::ComponentManager::ComponentManager(Autonomaus& autonomaus)
{
	this->autonomaus = &autonomaus;
}

autonomaus::ComponentManager::~ComponentManager()
{
	for (auto i: this->components)
	{
		delete i;
	}
	this->components.clear();
	this->tags.clear();
	this->pending_remove.clear();
	this->pending_add.clear();
}

void autonomaus::ComponentManager::show()
{
	this->visible = true;
}

void autonomaus::ComponentManager::hide()
{
	this->visible = false;
}

void autonomaus::ComponentManager::toggle()
{
	this->visible = !this->visible;
}

void autonomaus::ComponentManager::reload()
{
	for (auto& component: this->components)
	{
		component->reload();
	}
}

void autonomaus::ComponentManager::update()
{
	for (auto component: this->pending_add)
	{
		this->components.push_back(component);
	}
	this->pending_add.clear();

	draw();

	for (auto& component: this->components)
	{
		if (this->pending_remove.count(component) == 0)
		{
			component->update();
		}
	}

	for (auto i = this->components.begin(); i != this->components.end(); /* No. */)
	{
		if (this->pending_remove.count(*i) != 0)
		{
			i = this->components.erase(i);
		}
		else
		{
			++i;
		}
	}
	this->pending_remove.clear();
}

void autonomaus::ComponentManager::draw()
{
	if (!this->visible)
	{
		return;
	}

	static const int FLAGS = ImGuiWindowFlags_NoTitleBar
		| ImGuiWindowFlags_NoResize
		| ImGuiWindowFlags_NoScrollbar
		| ImGuiWindowFlags_NoInputs
		| ImGuiWindowFlags_NoSavedSettings
		| ImGuiWindowFlags_NoFocusOnAppearing
		| ImGuiWindowFlags_NoBringToFrontOnFocus
		| ImGuiWindowFlags_MenuBar;

	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));
	if (ImGui::Begin("AUTONOMAUS", nullptr, FLAGS))
	{
		auto viewport = autonomaus->get_draw_state().get_gui_viewport();
		ImGui::SetWindowPos(ImVec2(0, 0));
		ImGui::SetWindowSize(ImVec2(viewport.z, viewport.w));


		if (ImGui::BeginMainMenuBar())
		{
			if (ImGui::BeginMenu("File"))
			{
				if (ImGui::MenuItem("Reload"))
				{
					this->autonomaus->get_game_state().reload();
				}

				if (ImGui::MenuItem("Hide", "F12"))
				{
					hide();
				}

				ImGui::EndMenu();
			}

			if (ImGui::BeginMenu("View"))
			{
				if (ImGui::MenuItem("Debug Drawing", nullptr, this->autonomaus->get_is_debug_enabled()))
				{
					if (this->autonomaus->get_is_debug_enabled())
					{
						this->autonomaus->disable_debug();
					}
					else
					{
						this->autonomaus->enable_debug();
					}
				}

				ImGui::Separator();

				auto& input_manager = this->autonomaus->get_input_manager();
				if (ImGui::MenuItem("Preview Input", nullptr, input_manager.get_is_preview_enabled()))
				{
					if (this->autonomaus->get_is_debug_enabled())
					{
						this->autonomaus->disable_debug();
					}
					else
					{
						this->autonomaus->enable_debug();
					}
				}

				auto mode = input_manager.get_preview_mode();
				if (ImGui::MenuItem("Preview Input After", nullptr, mode == InputManager::preview_after))
				{
					input_manager.set_preview_mode(InputManager::preview_after);
				}

				if (ImGui::MenuItem("Preview Input Before", nullptr, mode == InputManager::preview_before))
				{
					input_manager.set_preview_mode(InputManager::preview_before);
				}

				ImGui::EndMenu();
			}

			if (!this->components.empty() &&
				ImGui::BeginMenu("Components"))
			{
				for (auto& component: this->components)
				{
					auto name = component->get_name();

					if (ImGui::MenuItem(name.c_str()))
					{
						component->show();
					}
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}
	}
	else
	{
		this->visible = false;
	}
	ImGui::End();
	ImGui::PopStyleColor();
}
