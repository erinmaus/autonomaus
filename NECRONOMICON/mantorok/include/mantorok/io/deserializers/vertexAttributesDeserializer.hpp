// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DESERIALIZERS_VERTEX_ATTRIBUTES_DESERIALIZER_HPP
#define MANTOROK_IO_DESERIALIZERS_VERTEX_ATTRIBUTES_DESERIALIZER_HPP

#include "mantorok/io/datDeserializer.hpp"
#include "kompvter/graphics/vertexAttributes.hpp"

namespace mantorok
{
	template <>
	struct DATDeserializer<kompvter::VertexAttributes>
	{
		static void deserialize(
			kompvter::VertexAttributes& attributes,
			DATReader& reader);
	};
}

#endif
