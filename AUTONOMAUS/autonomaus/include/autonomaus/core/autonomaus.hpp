// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_CORE_AUTONOMAUS_HPP
#define AUTONOMAUS_CORE_AUTONOMAUS_HPP

#include <map>
#include "autonomaus/state/gameStateSlot.hpp"
#include "ganymede/plugin.hpp"
#include "touchstone/gl/context.hpp"
#include "eiznebahn/input/inputSpoofer.hpp"

namespace autonomaus
{
	class ComponentManager;
	class DrawState;
	class WorldDrawHook;
	class GUIDrawHook;
	class GameStateManager;
	class WorldProvider;
	class MapProvider;
	class NPCProvider;
	class LootProvider;
	class PropsProvider;
	class GUIProvider;
	class InputManager;

	class Autonomaus : private ganymede::Plugin
	{
	public:
		~Autonomaus();

		int get_current_frame() const;

		DrawState& get_draw_state();
		const DrawState& get_draw_state() const;

		WorldDrawHook& get_world_draw_hook();
		const WorldDrawHook& get_world_draw_hook() const;

		GUIDrawHook& get_gui_draw_hook();
		const GUIDrawHook& get_gui_draw_hook() const;

		GameStateManager& get_game_state();
		const GameStateManager& get_game_state() const;

		GameStateSlotView<WorldProvider>& get_world_state();
		const GameStateSlotView<WorldProvider>& get_world_state() const;
		GameStateSlotView<MapProvider>& get_map_state();
		const GameStateSlotView<MapProvider>& get_map_state() const;
		GameStateSlotView<NPCProvider>& get_npc_state();
		const GameStateSlotView<NPCProvider>& get_npc_state() const;
		GameStateSlotView<LootProvider>& get_loot_state();
		const GameStateSlotView<LootProvider>& get_loot_state() const;
		GameStateSlotView<PropsProvider>& get_props_state();
		const GameStateSlotView<PropsProvider>& get_props_state() const;
		GameStateSlotView<GUIProvider>& get_gui_state();
		const GameStateSlotView<GUIProvider>& get_gui_state() const;

		InputManager& get_input_manager();
		const InputManager& get_input_manager() const;
		const eiznebahn::InputState& get_input_state() const;
		void suppress_input();
		void restore_input();
		bool get_is_input_suppressed() const;

		ComponentManager& get_component_manager();
		const ComponentManager& get_component_manager() const;

		bool get_is_debug_enabled() const;
		void enable_debug();
		void disable_debug();

		template <typename U>
		void add_userdata(U* value);

		template <typename U>
		void remove_userdata();

		template <typename U>
		U* get_userdata() const;

		static std::string get_config_path(const std::string& filename = std::string());
		static std::string get_share_path(const std::string& filename = std::string());

		static ganymede::Plugin* create_plugin(touchstone::ImmediateDispatch* dispatch);

	private:
		explicit Autonomaus(touchstone::ImmediateDispatch& dispatch);

		struct Impl;
		std::unique_ptr<Impl> instance;

		void try_suppress_input(bool captured);

		// ganymede::Plugin implementation
		void before_switch_context() override;
		void after_switch_context(bool valid, WindowHandle window) override;
		void before_call(std::uint32_t token) override;
		void after_call(const touchstone::Call* call) override;
		void on_flip(const touchstone::CallStream& call_stream) override;

		template <typename U>
		struct UserdataTag
		{
			static const int VALUE = 0;
		};
		std::map<const int*, void*> userdata;
	};
}

template <typename U>
const int autonomaus::Autonomaus::UserdataTag<U>::VALUE;

template <typename U>
void autonomaus::Autonomaus::add_userdata(U* value)
{
	this->userdata[&UserdataTag<U>::VALUE] = value;
}

template <typename U>
void autonomaus::Autonomaus::remove_userdata()
{
	this->userdata.erase(&UserdataTag<U>::VALUE);
}

template <typename U>
U* autonomaus::Autonomaus::get_userdata() const
{
	auto iter = this->userdata.find(&UserdataTag<U>::VALUE);
	if (iter == this->userdata.end())
	{
		return nullptr;
	}

	return static_cast<U*>(iter->second);
}

extern "C"
ganymede::Plugin* ganymede_create_plugin(
	touchstone::ImmediateDispatch* dispatch);

#endif
