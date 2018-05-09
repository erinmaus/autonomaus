// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/deserializers/vertexAttributesDeserializer.hpp"
#include "mantorok/io/deserializers/vertexFetcherDeserializer.hpp"

void mantorok::DATDeserializer<kompvter::VertexAttributes>::deserialize(
	kompvter::VertexAttributes& attributes, DATReader& reader)
{
	if (reader.match_section("kompvter.VertexAttributes::root"))
	{
		if (!reader.match_section("kompvter.VertexAttributes::attributes"))
		{
			attributes = kompvter::VertexAttributes();

			DATReader::Section section;
			while (reader.try_peek_section(section))
			{
				if (section.name != "kompvter.VertexAttributes::attribute")
				{
					break;
				}
				reader.enter_section();

				std::string name;
				reader.read(name);

				kompvter::VertexFetcher fetcher;
				DATDeserializer<kompvter::VertexFetcher>::deserialize(fetcher, reader);

				attributes.add(name, fetcher);

				reader.leave_section();
			}

			reader.leave_section();
		}

		reader.leave_section();
	}
}
