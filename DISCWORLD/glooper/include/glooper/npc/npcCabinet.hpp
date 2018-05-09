// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_NPC_NPC_CABINET_HPP
#define GLOOPER_NPC_NPC_CABINET_HPP

#include <map>
#include "glooper/npc/npc.hpp"

namespace glooper
{
	class NPCCabinet
	{
	public:
		NPCCabinet() = default;
		~NPCCabinet() = default;

		bool has(NPC::ID id) const;
		bool was_removed(NPC::ID id) const;

		NPC& get(NPC::ID id);
		const NPC& get(NPC::ID id) const;

		NPC& add();
		void remove(NPC::ID id);

	private:
		typedef std::map<NPC::ID, NPC> NPCMap;
		NPCMap npcs;

		NPC::ID max_id = 0;
		NPC::ID current_id = NPC::ID_BASE;

	public:
		typedef NPCMap::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
