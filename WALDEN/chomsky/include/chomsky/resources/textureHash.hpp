// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_TEXTURE_HASH_HPP
#define CHOMSKY_RESOURCES_TEXTURE_HASH_HPP

#include <string>
#include <sodium.h>
#include "kompvter/graphics/textureRegion.hpp"

namespace chomsky
{
	class TextureHash
	{
	public:
		static const std::size_t LENGTH = 16;

		TextureHash() = default;
		TextureHash(const std::string& value);
		~TextureHash() = default;

		const std::string& to_string() const;

		bool is_valid() const;
		operator bool() const;

		static TextureHash hash(const kompvter::TextureRegion& region);

	private:
		static void update_hash(
			crypto_generichash_state* state,
			const std::uint8_t* pixels, std::size_t width, std::size_t height);
		static std::string finalize_hash(crypto_generichash_state* state);

		std::string value;
	};
}

#endif
