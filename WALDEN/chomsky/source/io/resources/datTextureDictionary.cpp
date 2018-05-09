// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "chomsky/io/resources/textureDictionary.hpp"
#include "chomsky/io/resources/textureFingerprint.hpp"
#include "chomsky/resources/textureDefinition.hpp"
#include "chomsky/resources/textureDictionary.hpp"

void mantorok::DATSerializer<chomsky::TextureDictionary>::serialize(
	const chomsky::TextureDictionary& dictionary, DATWriter& writer)
{
	writer.begin_section("chomsky.TextureDictionary::entries", DATWriter::flag_clean);
	for (auto& definition: dictionary)
	{
		writer.begin_section(definition.first, DATWriter::flag_clean);

		DATSerializer<chomsky::TextureFingerprint>::serialize(
			definition.second.get_fingerprint(), writer);

		writer.write(definition.second.get_value(), "value");

		writer.begin_section("chomsky.TextureDictionary::tags", DATWriter::flag_clean);
		for (auto i = definition.second.tags_begin(); i != definition.second.tags_end(); ++i)
		{
			writer.write(*i);
		}
		writer.end_section();

		writer.begin_section("chomsky.TextureDictionary::data", DATWriter::flag_clean);
		for (auto i = definition.second.data_begin(); i != definition.second.data_end(); ++i)
		{
			writer.begin_section(i->first, DATWriter::flag_clean);
			writer.write(i->second);
			writer.end_section();
		}
		writer.end_section();

		writer.end_section();
	}
	writer.end_section();
}

void mantorok::DATDeserializer<chomsky::TextureDictionary>::deserialize(
	chomsky::TextureDictionary& dictionary, DATReader& reader)
{
	if (!reader.match_section("chomsky.TextureDictionary::entries"))
	{
		return;
	}

	DATReader::Section section;
	while (reader.try_peek_section(section))
	{
		reader.enter_section();

		chomsky::TextureFingerprint fingerprint;
		mantorok::DATDeserializer<chomsky::TextureFingerprint>::deserialize(
			fingerprint, reader);

		if (fingerprint.get_width() == 0 || fingerprint.get_height() == 0)
		{
			dictionary.add(section.name);
		}
		else
		{
			dictionary.add(section.name, fingerprint);
		}

		auto& definition = dictionary.get(section.name);

		std::string value;
		if (!reader.read(value))
		{
			break;
		}

		definition.set_value(value);

		if (reader.match_section("chomsky.TextureDictionary::tags"))
		{
			std::string tag;
			while (reader.read(tag))
			{
				definition.add_tag(tag);
			}

			reader.leave_section();
		}

		if (reader.match_section("chomsky.TextureDictionary::data"))
		{
			DATReader::Section data_section;
			while (reader.try_peek_section(data_section))
			{
				reader.enter_section();

				std::string value;
				reader.read(value);

				definition.add_data(data_section.name, value);

				reader.leave_section();
			}

			reader.leave_section();
		}

		reader.leave_section();
	}
	reader.leave_section();
}
