// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_IO_WORLD_QUERY_MATERIAL_SET_MATCH_HPP
#define CHOMSKY_IO_WORLD_QUERY_MATERIAL_SET_MATCH_HPP

#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class ActorAnimation;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::ActorAnimation>
	{
		static void serialize(
			const chomsky::ActorAnimation& animation,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::ActorAnimation>
	{
		static void deserialize(
			chomsky::ActorAnimation& animation,
			DATReader& reader);
	};
}

#endif
