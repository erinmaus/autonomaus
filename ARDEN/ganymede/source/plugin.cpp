// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "ganymede/plugin.hpp"

const char* const ganymede::CREATE_PLUGIN_PROC_NAME = "ganymede_create_plugin";

ganymede::Plugin::Plugin(touchstone::ImmediateDispatch& dispatch)
{
	this->dispatch = &dispatch;
}

void ganymede::Plugin::before_switch_context()
{
	// Nothing.
}

void ganymede::Plugin::after_switch_context(bool valid, WindowHandle window)
{
	// Nothing.
}

void ganymede::Plugin::before_call(std::uint32_t token)
{
	// Nothing.
}

void ganymede::Plugin::after_call(const touchstone::Call* call)
{
	// Nothing.
}

bool ganymede::Plugin::can_draw()
{
	return true;
}

void ganymede::Plugin::on_flip(const touchstone::CallStream& call_stream)
{
	// Nothing.
}

touchstone::ImmediateDispatch& ganymede::Plugin::get_dispatch() const
{
	return *this->dispatch;
}
