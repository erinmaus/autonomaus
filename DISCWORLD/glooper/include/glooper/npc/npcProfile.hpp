// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_NPC_NPC_PROFILE_HPP
#define GLOOPER_NPC_NPC_PROFILE_HPP

#include <cstdint>
#include <memory>
#include <string>
#include <glm/glm.hpp>

namespace glooper
{
	class NPCManager;

	class NPCProfile
	{
	public:
		friend class NPCManager;
		NPCProfile() = default;
		NPCProfile(const NPCProfile& other) = default;

		bool get_is_valid() const;

		std::string get_name() const;

	private:
		NPCProfile(const std::string& name, NPCManager& manager);

		struct Scope
		{
			Scope(const std::string& name, NPCManager& manager);
			std::string name;
			NPCManager* manager = nullptr;
		};
		std::shared_ptr<Scope> scope;
	};
}

#endif
