// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_IO_RESOURCES_TEXTURE_DICTIONARY_HPP
#define CHOMSKY_IO_RESOURCES_TEXTURE_DICTIONARY_HPP

#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class TextureDictionary;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::TextureDictionary>
	{
		static void serialize(
			const chomsky::TextureDictionary& dictionary,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::TextureDictionary>
	{
		static void deserialize(
			chomsky::TextureDictionary& dictionary,
			DATReader& reader);
	};
}

#endif
