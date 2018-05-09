// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <cstring>
#include <sodium.h>
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/gtx/color_space_YCoCg.hpp>
#include "kompvter/events/textureEvent.hpp"
#include "chomsky/resources/textureHash.hpp"

chomsky::TextureHash::TextureHash(const std::string& value)
{
	this->value = value;
}

const std::string& chomsky::TextureHash::to_string() const
{
	return this->value;
}

bool chomsky::TextureHash::is_valid() const
{
	return !this->value.empty();
}

chomsky::TextureHash::operator bool() const
{
	return is_valid();
}

chomsky::TextureHash chomsky::TextureHash::hash(
	const kompvter::TextureRegion& region)
{
	crypto_generichash_state state;
	crypto_generichash_init(&state, nullptr, 0, LENGTH);

	auto data = std::make_unique<std::uint8_t[]>(region.get_width() * region.get_height() * 4);
	region.get_pixels(0, 0, region.get_width(), region.get_height(), data.get());

	update_hash(&state, data.get(), region.get_width(), region.get_height());

	return TextureHash(finalize_hash(&state));
}

void chomsky::TextureHash::update_hash(
	crypto_generichash_state* state,
	const std::uint8_t* pixels, std::size_t width, std::size_t height)
{
	crypto_generichash_update(state, pixels, width * height * 4);
}

std::string chomsky::TextureHash::finalize_hash(
	crypto_generichash_state* state)
{
	std::uint8_t hash_bytes[LENGTH] = { 0 };
	crypto_generichash_final(state, hash_bytes, LENGTH);

	std::string result;
	result.reserve(LENGTH * 2);
	for (std::size_t i = 0; i < LENGTH; ++i)
	{
		char value[3] = { 0 };
		std::snprintf(value, sizeof(value), "%02x", hash_bytes[i]);
		result.push_back(value[0]);
		result.push_back(value[1]);
	}

	return result;
}
