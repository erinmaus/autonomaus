// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/serializers/drawParametersSerializer.hpp"

void mantorok::DATSerializer<kompvter::DrawParameters>::serialize(
	const kompvter::DrawParameters& draw_parameters, DATWriter& writer)
{
	writer.begin_section("kompvter.DrawParameters::root");

	writer.write<std::int32_t>(draw_parameters.get_primitive_type(), "primitive");
	writer.write<std::uint32_t>(draw_parameters.get_offset_start(), "offset_start");
	writer.write<std::uint32_t>(draw_parameters.get_offset_stop(), "offset_stop");

	writer.end_section();
}
