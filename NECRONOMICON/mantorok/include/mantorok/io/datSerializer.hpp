// This file is a part of NECRONOMICON.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef MANTOROK_IO_DAT_SERIALIZER_HPP
#define MANTOROK_IO_DAT_SERIALIZER_HPP

#include "mantorok/io/datWriter.hpp"

namespace mantorok
{
	template <typename Type>
	struct DATSerializer
	{
		static void serialize(const Type& value, DATWriter& writer);
	};
}

#endif
