// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cstring>
#include "chomsky/io/resources/textureFingerprint.hpp"
#include "chomsky/resources/textureFingerprint.hpp"

void mantorok::DATSerializer<chomsky::TextureFingerprint>::serialize(
	const chomsky::TextureFingerprint& fingerprint,
	DATWriter& writer)
{
	writer.begin_section("chomsky.TextureFingerprint::root", DATWriter::flag_clean);
	writer.write<std::uint32_t>(fingerprint.width, "width");
	writer.write<std::uint32_t>(fingerprint.height, "height");
	writer.write<std::uint32_t>(fingerprint.texture_width, "texture_width");
	writer.write<std::uint32_t>(fingerprint.texture_height, "texture_height");
	writer.write(
		(const std::uint8_t*)fingerprint.samples.get(),
		sizeof(glm::vec3) * fingerprint.width * fingerprint.height);
	writer.end_section();
}

void mantorok::DATDeserializer<chomsky::TextureFingerprint>::deserialize(
	chomsky::TextureFingerprint& fingerprint,
	DATReader& reader)
{
	if (reader.match_section("chomsky.TextureFingerprint::root"))
	{
		std::uint32_t width, height;
		std::uint32_t texture_width, texture_height;
		reader.read<std::uint32_t>(width);
		reader.read<std::uint32_t>(height);
		reader.read<std::uint32_t>(texture_width);
		reader.read<std::uint32_t>(texture_height);

		std::unique_ptr<std::uint8_t[]> samples_data;
		std::size_t samples_data_size;
		reader.read(samples_data, samples_data_size);

		std::size_t samples_size = width * height * sizeof(glm::vec3);
		if (samples_data_size != samples_size)
		{
			return;
		}

		if (width == 0 || height == 0 || texture_width == 0 || texture_height == 0)
		{
			fingerprint = chomsky::TextureFingerprint();
		}
		else
		{
			fingerprint = chomsky::TextureFingerprint(
				width, height, texture_width, texture_height);
			std::memcpy(
				fingerprint.samples.get(),
				samples_data.get(),
				samples_size);
		}

		reader.leave_section();
	}
}
