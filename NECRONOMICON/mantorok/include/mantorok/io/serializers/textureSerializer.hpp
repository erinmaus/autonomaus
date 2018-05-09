// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_SERIALIZERS_TEXTURE_SERIALIZER_HPP
#define MANTOROK_IO_SERIALIZERS_TEXTURE_SERIALIZER_HPP

#include "mantorok/io/datSerializer.hpp"
#include "kompvter/graphics/texture.hpp"

namespace mantorok
{
	template <>
	struct DATSerializer<kompvter::Texture>
	{
		static void serialize(
			const kompvter::Texture& texture,
			DATWriter& writer);
	};
}

#endif
