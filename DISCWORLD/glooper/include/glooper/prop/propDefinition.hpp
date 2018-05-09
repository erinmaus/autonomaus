// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_PROP_PROP_DEFINITION_HPP
#define GLOOPER_PROP_PROP_DEFINITION_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace glooper
{
	class PropManager;

	class PropDefinition
	{
	public:
		friend class PropManager;
		PropDefinition() = default;
		PropDefinition(const PropDefinition& other) = default;

		bool get_is_valid() const;

		std::string get_name() const;

	private:
		PropDefinition(const std::string& name, PropManager& manager);

		struct Scope
		{
			Scope(const std::string& name, PropManager& manager);
			std::string name;
			PropManager* manager = nullptr;
		};
		std::shared_ptr<Scope> scope;
	};
}

#endif
