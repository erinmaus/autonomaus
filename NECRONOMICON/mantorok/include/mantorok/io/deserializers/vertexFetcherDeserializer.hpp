// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DESERIALIZERS_VERTEX_FETCHER_DESERIALIZER_HPP
#define MANTOROK_IO_DESERIALIZERS_VERTEX_FETCHER_DESERIALIZER_HPP

#include "mantorok/io/datDeserializer.hpp"
#include "kompvter/graphics/vertexFetcher.hpp"

namespace mantorok
{
	template <>
	struct DATDeserializer<kompvter::VertexFetcher>
	{
		static void deserialize(
			kompvter::VertexFetcher& fetcher,
			DATReader& reader);
	};
}

#endif
