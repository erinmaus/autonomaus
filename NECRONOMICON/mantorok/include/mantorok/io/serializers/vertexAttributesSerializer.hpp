// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_SERIALIZERS_VERTEX_ATTRIBUTES_SERIALIZER_HPP
#define MANTOROK_IO_SERIALIZERS_VERTEX_ATTRIBUTES_SERIALIZER_HPP

#include "mantorok/io/datSerializer.hpp"
#include "kompvter/graphics/vertexAttributes.hpp"

namespace mantorok
{
	template <>
	struct DATSerializer<kompvter::VertexAttributes>
	{
		static void serialize(
			const kompvter::VertexAttributes& attributes,
			DATWriter& writer);
	};
}

#endif
