// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DESERIALIZERS_TEXTURE_REGION_SERIALIZER_HPP
#define MANTOROK_IO_DESERIALIZERS_TEXTURE_REGION_SERIALIZER_HPP

#include "mantorok/io/datDeserializer.hpp"
#include "kompvter/graphics/textureRegion.hpp"

namespace mantorok
{
	template <>
	struct DATDeserializer<kompvter::TextureRegion>
	{
		static void deserialize(
			kompvter::TextureRegion& region,
			DATReader& reader);
	};
}

#endif
