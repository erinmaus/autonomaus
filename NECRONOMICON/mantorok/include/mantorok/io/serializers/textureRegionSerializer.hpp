// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_SERIALIZERS_TEXTURE_REGION_SERIALIZER_HPP
#define MANTOROK_IO_SERIALIZERS_TEXTURE_REGION_SERIALIZER_HPP

#include "mantorok/io/datSerializer.hpp"
#include "kompvter/graphics/textureRegion.hpp"

namespace mantorok
{
	template <>
	struct DATSerializer<kompvter::TextureRegion>
	{
		static void serialize(
			const kompvter::TextureRegion& region,
			DATWriter& writer);
	};
}

#endif
