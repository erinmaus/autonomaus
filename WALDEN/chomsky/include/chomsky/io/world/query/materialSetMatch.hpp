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
	class MaterialSetMatch;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::MaterialSetMatch>
	{
		static void serialize(
			const chomsky::MaterialSetMatch& match,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::MaterialSetMatch>
	{
		static void deserialize(
			chomsky::MaterialSetMatch& match,
			DATReader& reader);
	};
}

#endif
