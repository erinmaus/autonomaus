// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstdlib>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/log.hpp"

autonomaus::Log autonomaus::Log::instance;

autonomaus::Log::Log()
{
#ifndef NDEBUG
#ifdef AUTONOMAUS_BUILD_POSIX
	auto console_logger = std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>();
#elif AUTONOMAUS_BUILD_WINDOWS
	auto console_logger = std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>();
#endif
	console_logger->set_level(spdlog::level::debug);
	this->sinks.push_back(console_logger);
#endif

	auto path = Autonomaus::get_config_path("logs/log");
	auto file_sink = std::make_shared<spdlog::sinks::daily_file_sink_st>(path, 23, 59);
	file_sink->set_level(spdlog::level::debug);
	this->sinks.push_back(file_sink);

	this->logger = std::make_unique<spdlog::logger>("autonomaus", this->sinks.begin(), this->sinks.end());
	this->logger->set_pattern("[%H:%M:%S %l] %v");
}
