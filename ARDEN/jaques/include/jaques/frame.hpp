// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef JAQUES_FRAME_HPP
#define JAQUES_FRAME_HPP

#include <mutex>
#include <functional>
#include <memory>
#include <cstdint>
#include "ganymede/plugin.hpp"
#include "touchstone/gl/dispatch.hpp"
#include "touchstone/gl/traceBuilder.hpp"

namespace jaques
{
	class Frame
	{
	public:
		static touchstone::TraceBuilder& get_trace_builder();
		static touchstone::ImmediateDispatch& get_gl();
		static touchstone::Dispatch& get_dispatch();

		static void before_switch_context(std::uintptr_t context, std::uintptr_t window);
		static void after_switch_context(std::uintptr_t context, std::uintptr_t window);
		static void end();

		static void before_call(std::uint32_t token);
		static void after_call(const touchstone::Call* call);

		static void for_each_plugin(const std::function<void(ganymede::Plugin*)>& iter);

	private:
		static std::unique_ptr<touchstone::TraceBuilder> trace_builder_instance;
		static std::unique_ptr<touchstone::ImmediateDispatch> gl_instance;

		static std::uintptr_t current_context;

		struct PluginGuard
		{
			~PluginGuard();
			bool loaded = false;
			std::vector<ganymede::Plugin*> values;
		private:
			std::mutex mutex;
		};
		static PluginGuard plugins;
		static void load_plugins();
	};
}

#endif
