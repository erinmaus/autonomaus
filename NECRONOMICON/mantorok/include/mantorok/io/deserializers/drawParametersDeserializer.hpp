// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DESERIALIZERS_DRAW_PARAMETERS_DESERIALIZER_HPP
#define MANTOROK_IO_DESERIALIZERS_DRAW_PARAMETERS_DESERIALIZER_HPP

#include "mantorok/io/datDeserializer.hpp"
#include "kompvter/graphics/drawParameters.hpp"

namespace mantorok
{
	template <>
	struct DATDeserializer<kompvter::DrawParameters>
	{
		static void deserialize(
			kompvter::DrawParameters& draw_parameters,
			DATReader& reader);
	};
}

#endif
