// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

// X11 #define conflicts.
#ifdef AUTONOMAUS_BUILD_POSIX
#include "eiznebahn/platform/x11/x11InputSpoofer.hpp"
#else
#include "eiznebahn/platform/nullInputSpoofer.hpp"
#endif

#include "autonomaus/core/autonomaus.hpp"

#include <cstring>
#include <boost/filesystem.hpp>
#include "autonomaus/components/componentManager.hpp"
#include "autonomaus/components/asimaus/isaacLoaderComponent.hpp"
#include "autonomaus/components/gui/chatLogComponent.hpp"
#include "autonomaus/components/gui/guiContentComponent.hpp"
#include "autonomaus/components/input/collectInputSamplesComponent.hpp"
#include "autonomaus/components/input/mousePathsInspectorComponent.hpp"
#include "autonomaus/components/world/npcCabinetComponent.hpp"
#include "autonomaus/components/world/propsComponent.hpp"
#include "autonomaus/components/world/worldUpdateMetricsComponent.hpp"
#include "autonomaus/core/log.hpp"
#include "autonomaus/input/inputManager.hpp"
#include "autonomaus/state/gameStateManager.hpp"
#include "autonomaus/state/gameStateSlot.hpp"
#include "autonomaus/state/gameStateSlot.hpp"
#include "autonomaus/state/guiProvider.hpp"
#include "autonomaus/state/mapProvider.hpp"
#include "autonomaus/state/lootProvider.hpp"
#include "autonomaus/state/npcProvider.hpp"
#include "autonomaus/state/propsProvider.hpp"
#include "autonomaus/state/worldProvider.hpp"
#include "autonomaus/graphics/drawState.hpp"
#include "autonomaus/graphics/worldDrawHook.hpp"
#include "autonomaus/graphics/guiDrawHook.hpp"
#include "touchstone/gl/glTrace.hpp"
#include "touchstone/gl/calls.hpp"
#include "rosalind/common/re.hpp"

struct autonomaus::Autonomaus::Impl
{
	Impl(Autonomaus& autonomaus, touchstone::ImmediateDispatch& dispatch);

	int current_frame = 0;

	touchstone::Context context;
	DrawState draw_state;
	WorldDrawHook world_draw_hook;
	GUIDrawHook gui_draw_hook;

	GameStateManager game_state;
	GameStateSlotView<WorldProvider> world;
	GameStateSlotView<MapProvider> map;
	GameStateSlotView<NPCProvider> npc;
	GameStateSlotView<LootProvider> loot;
	GameStateSlotView<PropsProvider> props;
	GameStateSlotView<GUIProvider> gui;

	bool input_supressed = false;

	ComponentManager component_manager;

#ifdef NDEBUG
	bool debug_enabled = false;
#else
	bool debug_enabled = true;
#endif

#ifdef AUTONOMAUS_BUILD_POSIX
	typedef std::unique_ptr<eiznebahn::X11InputSpoofer> InputSpooferPointer;
#else
	typedef std::unique_ptr<eiznebahn::InputSpoofer> InputSpooferPointer;
#endif
	InputSpooferPointer input_spoofer;
	const eiznebahn::InputState* input_state = nullptr;

	std::unique_ptr<InputManager> input_manager;
};

autonomaus::Autonomaus::Autonomaus(touchstone::ImmediateDispatch& dispatch) :
	ganymede::Plugin(dispatch)
{
	this->instance = std::make_unique<Impl>(*this, dispatch);
}

autonomaus::Autonomaus::~Autonomaus()
{
	// Nothing.
}

autonomaus::Autonomaus::Impl::Impl(Autonomaus& autonomaus, touchstone::ImmediateDispatch& dispatch) :
	world_draw_hook(dispatch, draw_state),
	gui_draw_hook(dispatch, draw_state),
	component_manager(autonomaus)
{
	this->world = game_state.add<WorldProvider>();
	this->map = game_state.add<MapProvider>();
	this->npc = game_state.add<NPCProvider>(
		this->map.get_slot().get_provider(),
		this->world.get_slot().get_provider());
	this->loot = game_state.add<LootProvider>(this->world.get_slot().get_provider());
	this->props = game_state.add<PropsProvider>(this->world.get_slot().get_provider());
	this->gui = game_state.add<GUIProvider>();

	Log::info("Created game state slots.");

	this->component_manager.add<IsaacLoaderComponent>();
	this->component_manager.add<ChatLogComponent>();
	this->component_manager.add<GUIContentComponent>();
	this->component_manager.add<CollectInputSamplesComponent>();
	this->component_manager.add<MousePathsInspectorComponent>();
	this->component_manager.add<NPCCabinetComponent>();
	this->component_manager.add<PropsComponent>();
	this->component_manager.add<WorldUpdateMetricsComponent>();

	Log::info("Created default components.");

#ifdef AUTONOMAUS_BUILD_POSIX
	this->input_spoofer = InputSpooferPointer(new eiznebahn::X11InputSpoofer());
	this->input_spoofer->make_current();
	this->input_spoofer->register_keyboard_callback(
		[this](int type, int key_code)
		{
			if (type == eiznebahn::X11InputSpoofer::event_key_release &&
				key_code == eiznebahn::KeySequence::key_code_f12)
			{
				this->component_manager.toggle();
				return true;
			}

			return key_code == eiznebahn::KeySequence::key_code_f12;
		});
	this->input_state = &this->input_spoofer->get_real_input_state();
#else
	this->input_spoofer = InputSpooferPointer(new eiznebahn::NullInputSpoofer());
	this->input_state = &this->input_spoofer->get_input_state();
#endif

	this->input_manager = std::make_unique<InputManager>(autonomaus, *this->input_spoofer);
	this->input_manager->load(get_config_path());

	Log::info("Created input spoofer.");
}

void autonomaus::Autonomaus::before_switch_context()
{
	instance->world_draw_hook.invalidate();
	instance->gui_draw_hook.invalidate();
}

void autonomaus::Autonomaus::after_switch_context(bool valid, WindowHandle window)
{
	if (valid)
	{
		instance->world_draw_hook.load();
		instance->gui_draw_hook.load();

#ifdef AUTONOMAUS_BUILD_POSIX
		instance->input_spoofer->set_target_window(window);
#endif

		instance->component_manager.reload();
	}
}

void autonomaus::Autonomaus::before_call(std::uint32_t token)
{
	instance->world_draw_hook.before_call(instance->context, token);
	instance->gui_draw_hook.before_call(instance->context, token);
}

void autonomaus::Autonomaus::after_call(const touchstone::Call* call)
{
	instance->context.update(call);
	instance->world_draw_hook.after_call(instance->context, call);
	instance->gui_draw_hook.after_call(instance->context, call);
}

void autonomaus::Autonomaus::on_flip(const touchstone::CallStream& call_stream)
{
	try
	{
		++instance->current_frame;

		instance->game_state.update(call_stream);

		instance->gui_draw_hook.update_input(*instance->input_state);
		try_suppress_input(
			instance->gui_draw_hook.wants_mouse_captured() ||
			instance->gui_draw_hook.wants_keyboard_captured());

		if (instance->gui_draw_hook.is_ready())
		{
			instance->component_manager.update();
		}

		instance->input_manager->update();
		instance->input_spoofer->poll();

		instance->world_draw_hook.on_flip(instance->context);
		instance->gui_draw_hook.on_flip(instance->context);
	}
	catch (const std::exception& e)
	{
		Log::critical("I'm melting! {}", e.what());
		std::abort();
	}
}

void autonomaus::Autonomaus::try_suppress_input(bool captured)
{
#ifdef TOUCHSTONE_BUILD_POSIX
	if (captured && instance->input_spoofer->get_real_events_enabled())
	{
		instance->input_spoofer->disable_real_events();
	}
	else if (!captured && !instance->input_spoofer->get_real_events_enabled())
	{
		if (!instance->input_supressed)
		{
			instance->input_spoofer->enable_real_events();
		}
	}
#endif
}

extern "C"
ganymede::Plugin* ganymede_create_plugin(
	touchstone::ImmediateDispatch* dispatch)
{
	return autonomaus::Autonomaus::create_plugin(dispatch);
}

// Public interface

int autonomaus::Autonomaus::get_current_frame() const
{
	return instance->current_frame;
}

autonomaus::DrawState& autonomaus::Autonomaus::get_draw_state()
{
	return instance->draw_state;
}

const autonomaus::DrawState& autonomaus::Autonomaus::get_draw_state() const
{
	return instance->draw_state;
}

autonomaus::WorldDrawHook& autonomaus::Autonomaus::get_world_draw_hook()
{
	return instance->world_draw_hook;
}

const autonomaus::WorldDrawHook& autonomaus::Autonomaus::get_world_draw_hook() const
{
	return instance->world_draw_hook;
}

autonomaus::GUIDrawHook& autonomaus::Autonomaus::get_gui_draw_hook()
{
	return instance->gui_draw_hook;
}

const autonomaus::GUIDrawHook& autonomaus::Autonomaus::get_gui_draw_hook() const
{
	return instance->gui_draw_hook;
}

autonomaus::GameStateManager& autonomaus::Autonomaus::get_game_state()
{
	return instance->game_state;
}

const autonomaus::GameStateManager& autonomaus::Autonomaus::get_game_state() const
{
	return instance->game_state;
}

autonomaus::GameStateSlotView<autonomaus::WorldProvider>&
autonomaus::Autonomaus::get_world_state()
{
	return instance->world;
}

const autonomaus::GameStateSlotView<autonomaus::WorldProvider>&
autonomaus::Autonomaus::get_world_state() const
{
	return instance->world;
}
autonomaus::GameStateSlotView<autonomaus::MapProvider>&
autonomaus::Autonomaus::get_map_state()
{
	return instance->map;
}

const autonomaus::GameStateSlotView<autonomaus::MapProvider>&
autonomaus::Autonomaus::get_map_state() const
{
	return instance->map;
}

autonomaus::GameStateSlotView<autonomaus::NPCProvider>&
autonomaus::Autonomaus::get_npc_state()
{
	return instance->npc;
}

const autonomaus::GameStateSlotView<autonomaus::NPCProvider>&
autonomaus::Autonomaus::get_npc_state() const
{
	return instance->npc;
}

autonomaus::GameStateSlotView<autonomaus::LootProvider>&
autonomaus::Autonomaus::get_loot_state()
{
	return instance->loot;
}

const autonomaus::GameStateSlotView<autonomaus::LootProvider>&
autonomaus::Autonomaus::get_loot_state() const
{
	return instance->loot;
}

autonomaus::GameStateSlotView<autonomaus::PropsProvider>&
autonomaus::Autonomaus::get_props_state()
{
	return instance->props;
}

const autonomaus::GameStateSlotView<autonomaus::PropsProvider>&
autonomaus::Autonomaus::get_props_state() const
{
	return instance->props;
}

autonomaus::GameStateSlotView<autonomaus::GUIProvider>&
autonomaus::Autonomaus::get_gui_state()
{
	return instance->gui;
}

const autonomaus::GameStateSlotView<autonomaus::GUIProvider>&
autonomaus::Autonomaus::get_gui_state() const
{
	return instance->gui;
}

autonomaus::InputManager& autonomaus::Autonomaus::get_input_manager()
{
	return *instance->input_manager;
}

const autonomaus::InputManager& autonomaus::Autonomaus::get_input_manager() const
{
	return *instance->input_manager;
}

const eiznebahn::InputState& autonomaus::Autonomaus::get_input_state() const
{
	return *instance->input_state;
}

void autonomaus::Autonomaus::suppress_input()
{
#ifdef AUTONOMAUS_BUILD_POSIX
	instance->input_spoofer->disable_real_events();
	instance->input_supressed = true;
#endif
}

void autonomaus::Autonomaus::restore_input()
{
#ifdef AUTONOMAUS_BUILD_POSIX
	if (!instance->gui_draw_hook.wants_mouse_captured() &&
		!instance->gui_draw_hook.wants_keyboard_captured())
	{
		instance->input_spoofer->enable_real_events();
	}

	instance->input_supressed = false;
#endif
}

bool autonomaus::Autonomaus::get_is_input_suppressed() const
{
	return instance->input_supressed;
}

autonomaus::ComponentManager& autonomaus::Autonomaus::get_component_manager()
{
	return instance->component_manager;
}

const autonomaus::ComponentManager& autonomaus::Autonomaus::get_component_manager() const
{
	return instance->component_manager;
}

bool autonomaus::Autonomaus::get_is_debug_enabled() const
{
	return instance->debug_enabled;
}

void autonomaus::Autonomaus::enable_debug()
{
	instance->debug_enabled = true;
	instance->input_manager->enable_previews();
}

void autonomaus::Autonomaus::disable_debug()
{
	instance->debug_enabled = false;
	instance->input_manager->disable_previews();
}

std::string autonomaus::Autonomaus::get_config_path(const std::string& filename)
{
	boost::filesystem::path path;
#ifdef AUTONOMAUS_BUILD_POSIX
	const char* home = std::getenv("HOME");

	if (home != nullptr)
	{
		path = boost::filesystem::path(home);
	}

	return path.string();
#else
	// Nothing.
#endif

	path.append(".hdt");
	path.append("autonomaus");

	if (!filename.empty())
	{
		path.append(filename);
	}

	return path.string();
}

std::string autonomaus::Autonomaus::get_share_path(const std::string& filename)
{
	boost::filesystem::path path;
#ifdef AUTONOMAUS_BUILD_POSIX
	const char* home = std::getenv("HOME");

	if (home != nullptr)
	{
		path = boost::filesystem::path(home);
	}
#else
	// Nothing.
#endif
	path.append(".hdt");
	path.append("autonomaus");
	path.append("share");

	if (!filename.empty())
	{
		path.append(filename);
	}

	return path.string();
}

ganymede::Plugin* autonomaus::Autonomaus::create_plugin(
	touchstone::ImmediateDispatch* dispatch)
{
	Log::info("Creating AUTONOMAUS...");
	auto result = new autonomaus::Autonomaus(*dispatch);
	Log::info("Created AUTONOMAUS.");

	return result;
}
