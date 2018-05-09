// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_IO_RESOURCES_SILHOUETTE_HPP
#define CHOMSKY_IO_RESOURCES_SILHOUETTE_HPP

#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class Silhouette;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::Silhouette>
	{
		static void serialize(
			const chomsky::Silhouette& silhouette,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::Silhouette>
	{
		static void deserialize(
			chomsky::Silhouette& silhouette,
			DATReader& reader);
	};
}

#endif
