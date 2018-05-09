// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_NPC_NPC_FILE_HPP
#define GLOOPER_NPC_NPC_FILE_HPP

#include "glooper/npc/npc.hpp"
#include "glooper/npc/npcCabinet.hpp"

namespace glooper
{
	class NPCFile
	{
	public:
		NPCFile(NPCCabinet& cabinet, NPC::ID id);
		virtual ~NPCFile() = default;

		virtual bool get_is_valid() const;

		NPC::ID get_id() const;
		NPC& get_npc() const;

	private:
		NPCCabinet* cabinet;
		NPC::ID id;
	};
}

#endif
