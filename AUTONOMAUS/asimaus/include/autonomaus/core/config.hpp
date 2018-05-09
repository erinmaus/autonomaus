// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_CORE_CONFIG_HPP
#define AUTONOMAUS_CORE_CONFIG_HPP

#include <yaml-cpp/yaml.h>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/log.hpp"

namespace autonomaus
{
	class Config
	{
	public:
		template <typename V>
		static V get(
			const std::string& scope,
			const std::string& node,
			const std::string& value);

	private:
		Config() = delete;
		~Config() = delete;
	};
}

template <typename V>
V autonomaus::Config::get(
	const std::string& scope,
	const std::string& node,
	const std::string& value)
{
	try
	{
		auto root = YAML::LoadFile(Autonomaus::get_config_path("mashina.yml"));
		if (root && root[scope] && root[scope][node] && root[scope][node][value])
		{
			return root[scope][node][value].as<V>();
		}
	}
	catch (...)
	{
		// Nothing.
	}

	Log::warn("Failed to get config value: {}.{}.{}", scope, node, value);
	return {};
}

#endif
