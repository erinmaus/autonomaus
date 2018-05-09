// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef DEADHAND_DEADHAND_HPP
#define DEADHAND_DEADHAND_HPP

#include <thread>
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include "ganymede/plugin.hpp"

// X11 sucks.
#define None 0L

namespace deadhand
{
	class Deadhand : public ganymede::Plugin
	{
	public:
		Deadhand(touchstone::ImmediateDispatch& dispatch);
		~Deadhand();

		void after_switch_context(bool valid, WindowHandle window) override;

	private:
		Display* display = nullptr;
		bool is_running = true;
		Window window = None;

		std::thread work_thread;
		void work();
	};
}

extern "C"
ganymede::Plugin* ganymede_create_plugin(
	touchstone::ImmediateDispatch* dispatch);

#endif
