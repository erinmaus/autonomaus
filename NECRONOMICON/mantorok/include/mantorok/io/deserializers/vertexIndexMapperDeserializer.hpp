// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DESERIALIZERS_VERTEX_INDEX_MAPPER_DESERIALIZER_HPP
#define MANTOROK_IO_DESERIALIZERS_VERTEX_INDEX_MAPPER_DESERIALIZER_HPP

#include "mantorok/io/datDeserializer.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"

namespace mantorok
{
	template <>
	struct DATDeserializer<kompvter::VertexIndexMapper>
	{
		static void deserialize(
			kompvter::VertexIndexMapper& index_mapper,
			DATReader& reader);
	};
}

#endif
