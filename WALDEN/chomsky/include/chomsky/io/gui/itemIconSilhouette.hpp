// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_IO_GUI_ITEM_ICON_SILHOUETTE_HPP
#define CHOMSKY_IO_GUI_ITEM_ICON_SILHOUETTE_HPP

#include "mantorok/io/datSerializer.hpp"
#include "mantorok/io/datDeserializer.hpp"

namespace chomsky
{
	class ItemIconSilhouette;
}

namespace mantorok
{
	template <>
	struct DATSerializer<chomsky::ItemIconSilhouette>
	{
		static void serialize(
			const chomsky::ItemIconSilhouette& silhouette,
			DATWriter& writer);
	};

	template <>
	struct DATDeserializer<chomsky::ItemIconSilhouette>
	{
		static void deserialize(
			chomsky::ItemIconSilhouette& silhouette,
			DATReader& reader);
	};
}

#endif
