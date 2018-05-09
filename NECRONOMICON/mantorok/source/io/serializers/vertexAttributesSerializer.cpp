// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/serializers/vertexAttributesSerializer.hpp"
#include "mantorok/io/serializers/vertexFetcherSerializer.hpp"

void mantorok::DATSerializer<kompvter::VertexAttributes>::serialize(
	const kompvter::VertexAttributes& attributes, DATWriter& writer)
{
	writer.begin_section("kompvter.VertexAttributes::root");
	writer.begin_section("kompvter.VertexAttributes::attributes");
	for (auto& i: attributes)
	{
		writer.begin_section("kompvter.VertexAttributes::attribute");
		writer.write(i.first, "name");
		DATSerializer<kompvter::VertexFetcher>::serialize(i.second, writer);
		writer.end_section();
	}
	writer.end_section();
	writer.end_section();
}
