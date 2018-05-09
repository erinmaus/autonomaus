// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_SERIALIZERS_DRAW_PARAMETERS_HPP
#define MANTOROK_IO_SERIALIZERS_DRAW_PARAMETERS_HPP

#include "mantorok/io/datSerializer.hpp"
#include "kompvter/graphics/drawParameters.hpp"

namespace mantorok
{
	template <>
	struct DATSerializer<kompvter::DrawParameters>
	{
		static void serialize(
			const kompvter::DrawParameters& draw_parameters,
			DATWriter& reader);
	};
}

#endif
