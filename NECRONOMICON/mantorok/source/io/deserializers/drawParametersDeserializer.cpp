// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "mantorok/io/deserializers/drawParametersDeserializer.hpp"

void mantorok::DATDeserializer<kompvter::DrawParameters>::deserialize(
	kompvter::DrawParameters& draw_parameters, DATReader& reader)
{
	if (!reader.match_section("kompvter.DrawParameters::root"))
	{
		return;
	}

	std::int32_t primitive;
	std::uint32_t offset_start, offset_stop;

	reader.read<std::int32_t>(primitive);
	reader.read<std::uint32_t>(offset_start);
	reader.read<std::uint32_t>(offset_stop);

	draw_parameters = kompvter::DrawParameters(
		primitive, offset_start, offset_stop);

	reader.leave_section();
}
