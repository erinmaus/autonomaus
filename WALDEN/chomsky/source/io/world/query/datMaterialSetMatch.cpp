// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/io/world/query/materialSetMatch.hpp"
#include "chomsky/io/resources/textureFingerprint.hpp"
#include "chomsky/world/query/materialDefinition.hpp"
#include "chomsky/world/query/materialSetMatch.hpp"

void mantorok::DATSerializer<chomsky::MaterialSetMatch>::serialize(
	const chomsky::MaterialSetMatch& match,
	DATWriter& writer)
{
	writer.begin_section("chomsky.MaterialSetMatch::root", DATWriter::flag_clean);
	for (auto& definition: match)
	{
		writer.begin_section("chomsky.MaterialSetMatch::definition",  DATWriter::flag_clean);

		mantorok::DATSerializer<chomsky::TextureFingerprint>::serialize(
			definition.get_fingerprint(), writer);

		auto color = definition.get_color();
		writer.write<float>(color.r, "color.r");
		writer.write<float>(color.g, "color.g");
		writer.write<float>(color.b, "color.b");

		auto max_difference_threshold = definition.get_max_difference_threshold();
		writer.write<float>(max_difference_threshold.x, "max_difference_threshold.x");
		writer.write<float>(max_difference_threshold.y, "max_difference_threshold.y");
		writer.write<float>(max_difference_threshold.z, "max_difference_threshold.z");

		auto max_dissimilarity = definition.get_max_dissimilarity();
		writer.write<float>(max_dissimilarity.x, "max_dissimilarity.x");
		writer.write<float>(max_dissimilarity.y, "max_dissimilarity.y");
		writer.write<float>(max_dissimilarity.z, "max_dissimilarity.z");

		auto max_color_difference = definition.get_max_color_difference();
		writer.write<float>(max_color_difference.r, "max_color_difference.r");
		writer.write<float>(max_color_difference.g, "max_color_difference.g");
		writer.write<float>(max_color_difference.b, "max_color_difference.b");

		writer.end_section();
	}
	writer.end_section();
}

void mantorok::DATDeserializer<chomsky::MaterialSetMatch>::deserialize(
	chomsky::MaterialSetMatch& match,
	DATReader& reader)
{
	if (reader.match_section("chomsky.MaterialSetMatch::root"))
	{
		match = chomsky::MaterialSetMatch();

		DATReader::Section section;
		while (reader.try_peek_section(section) &&
			section.name == "chomsky.MaterialSetMatch::definition")
		{
			reader.enter_section();

			chomsky::TextureFingerprint fingerprint;
			mantorok::DATDeserializer<chomsky::TextureFingerprint>::deserialize(
				fingerprint, reader);

			glm::vec3 color;
			reader.read<float>(color.r);
			reader.read<float>(color.g);
			reader.read<float>(color.b);

			glm::vec3 max_difference_threshold;
			reader.read<float>(max_difference_threshold.x);
			reader.read<float>(max_difference_threshold.y);
			reader.read<float>(max_difference_threshold.z);

			glm::vec3 max_dissimilarity;
			reader.read<float>(max_dissimilarity.x);
			reader.read<float>(max_dissimilarity.y);
			reader.read<float>(max_dissimilarity.z);

			glm::vec3 max_color_difference;
			reader.read<float>(max_color_difference.r);
			reader.read<float>(max_color_difference.g);
			reader.read<float>(max_color_difference.b);

			chomsky::MaterialDefinition definition;
			definition.set_fingerprint(fingerprint);
			definition.set_color(color);
			definition.set_max_difference_threshold(max_difference_threshold);
			definition.set_max_dissimilarity(max_dissimilarity);
			definition.set_max_color_difference(max_color_difference);
			match.add(definition);

			reader.leave_section();
		}

		reader.leave_section();
	}
}
