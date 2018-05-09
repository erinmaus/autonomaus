// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DAT_DESERIALIZER_HPP
#define MANTOROK_IO_DAT_DESERIALIZER_HPP

#include "mantorok/io/datReader.hpp"

namespace mantorok
{
	template <typename Type>
	struct DATDeserializer
	{
		static void deserialize(Type& value, DATReader& reader);
	};
}

#endif
