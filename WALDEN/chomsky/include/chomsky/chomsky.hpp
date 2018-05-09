// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_CHOMSKY_HPP
#define CHOMSKY_CHOMSKY_HPP

namespace chomsky
{
	namespace config
	{
		// The size of a silhoeutte in world units.
		static const int SILHOUETTE_WORLD_SIZE = 512;

		// The width and height of a silhoette, in fragments.
		static const int SILHOUETTE_SIZE = 4;

		static const float SILHOUETTE_DEFAULT_UPPER_THRESHOLD_RATIO = 1.05f;
		static const float SILHOUETTE_DEFAULT_LOWER_THRESHOLD_RATIO = 0.95f;

		// Dimensions of a font atlas.
		static const int FONT_NUM_ROWS = 16;
		static const int FONT_NUM_COLUMNS = 16;

		// Size of an item icon, in pixels.
		static const int ITEM_ICON_WIDTH = 36;
		static const int ITEM_ICON_HEIGHT = 32;
	}
}

#endif