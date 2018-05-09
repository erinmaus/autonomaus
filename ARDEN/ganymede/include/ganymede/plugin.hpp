// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GANYMEDE_PLUGIN_HPP
#define GANYMEDE_PLUGIN_HPP

#include "touchstone/gl/context.hpp"
#include "touchstone/gl/immediateDispatch.hpp"
#include "touchstone/trace/call.hpp"
#include "touchstone/trace/callStream.hpp"

// Some mouth-breather #define'd these on some platforms.
#ifdef Status
#undef Status
#endif

#ifdef Success
#undef Success
#endif

#ifdef Failure
#undef Failure
#endif

#ifdef None
#undef None
#endif

namespace ganymede
{
	class Plugin
	{
	public:
		typedef std::intptr_t WindowHandle;

		explicit Plugin(touchstone::ImmediateDispatch& dispatch);
		virtual ~Plugin() = default;

		virtual void before_switch_context();
		virtual void after_switch_context(bool valid, WindowHandle window);

		virtual void before_call(std::uint32_t token);
		virtual void after_call(const touchstone::Call* call);

		virtual bool can_draw();
		virtual void on_flip(const touchstone::CallStream& call_stream);

	protected:
		touchstone::ImmediateDispatch& get_dispatch() const;

	private:
		touchstone::ImmediateDispatch* dispatch;
	};

	extern const char* const CREATE_PLUGIN_PROC_NAME;
	typedef Plugin* (* CreatePluginProc)(
		touchstone::ImmediateDispatch* dispatch);
}

#endif
