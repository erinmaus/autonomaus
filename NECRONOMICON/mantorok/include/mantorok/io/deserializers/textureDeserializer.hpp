// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DESERIALIZERS_TEXTURE_SERIALIZER_HPP
#define MANTOROK_IO_DESERIALIZERS_TEXTURE_SERIALIZER_HPP

#include "mantorok/io/datDeserializer.hpp"
#include "kompvter/graphics/texture.hpp"

namespace mantorok
{
	template <>
	struct DATDeserializer<kompvter::Texture>
	{
		static void deserialize(
			kompvter::Texture& texture,
			DATReader& writer);
	};
}

#endif
