// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_SERIALIZERS_VERTEX_INDEX_MAPPER_SERIALIZER_HPP
#define MANTOROK_IO_SERIALIZERS_VERTEX_INDEX_MAPPER_SERIALIZER_HPP

#include "mantorok/io/datSerializer.hpp"
#include "kompvter/graphics/vertexIndexMapper.hpp"

namespace mantorok
{
	template <>
	struct DATSerializer<kompvter::VertexIndexMapper>
	{
		static void serialize(
			const kompvter::VertexIndexMapper& index_mapper,
			DATWriter& writer);
	};
}

#endif
