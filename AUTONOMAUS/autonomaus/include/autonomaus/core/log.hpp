// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_CORE_LOG_HPP
#define AUTONOMAUS_CORE_LOG_HPP

#include <memory>
#include <utility>
#include <vector>
#include "spdlog/spdlog.h"

namespace autonomaus
{
	class Log
	{
	public:
		template <typename... Arguments>
		static void debug(Arguments&&... arguments);

		template <typename... Arguments>
		static void info(Arguments&&... arguments);

		template <typename... Arguments>
		static void warn(Arguments&&... arguments);

		template <typename... Arguments>
		static void critical(Arguments&&... arguments);

	private:
		Log();
		~Log() = default;

		std::vector<spdlog::sink_ptr> sinks;
		std::unique_ptr<spdlog::logger> logger;

		static Log instance;
	};
}

template <typename... Arguments>
void autonomaus::Log::debug(Arguments&&... arguments)
{
	instance.logger->debug(std::forward<Arguments>(arguments)...);
}

template <typename... Arguments>
void autonomaus::Log::info(Arguments&&... arguments)
{
	instance.logger->info(std::forward<Arguments>(arguments)...);
}

template <typename... Arguments>
void autonomaus::Log::warn(Arguments&&... arguments)
{
	instance.logger->warn(std::forward<Arguments>(arguments)...);
}

template <typename... Arguments>
void autonomaus::Log::critical(Arguments&&... arguments)
{
	instance.logger->critical(std::forward<Arguments>(arguments)...);
}

#endif
