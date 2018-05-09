// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_LISTENERS_MINIMAP_LISTENER_HPP
#define KOMPVTER_LISTENERS_MINIMAP_LISTENER_HPP

namespace kompvter
{
	class MinimapDrawEvent;

	class MinimapListener
	{
	public:
		MinimapListener() = default;
		virtual ~MinimapListener() = default;

		virtual void on_draw_minimap(const MinimapDrawEvent& event);
	};
}

#endif
