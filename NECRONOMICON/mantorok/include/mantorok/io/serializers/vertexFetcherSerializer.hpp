// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_SERIALIZERS_VERTEX_FETCHER_SERIALIZER_HPP
#define MANTOROK_IO_SERIALIZERS_VERTEX_FETCHER_SERIALIZER_HPP

#include "mantorok/io/datSerializer.hpp"
#include "kompvter/graphics/vertexFetcher.hpp"

namespace mantorok
{
	template <>
	struct DATSerializer<kompvter::VertexFetcher>
	{
		static void serialize(
			const kompvter::VertexFetcher& fetcher,
			DATWriter& writer);
	};
}

#endif
