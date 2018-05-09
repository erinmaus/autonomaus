// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_IO_RESOURCES_TEXTURE_FINGERPRINT_HPP
#define CHOMSKY_IO_RESOURCES_TEXTURE_FINGERPRINT_HPP

#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class TextureFingerprint;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::TextureFingerprint>
	{
		static void serialize(
			const chomsky::TextureFingerprint& fingerprint,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::TextureFingerprint>
	{
		static void deserialize(
			chomsky::TextureFingerprint& fingerprint,
			DATReader& reader);
	};
}

#endif
