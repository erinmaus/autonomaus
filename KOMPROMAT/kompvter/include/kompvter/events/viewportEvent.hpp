// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_EVENTS_VIEWPORT_EVENT_HPP
#define KOMPVTER_EVENTS_VIEWPORT_EVENT_HPP

namespace kompvter
{
	class ViewportEvent
	{
	public:
		ViewportEvent() = default;
		~ViewportEvent() = default;

		int get_x() const;
		void set_x(int value);

		int get_y() const;
		void set_y(int value);

		int get_width() const;
		void set_width(int value);

		int get_height() const;
		void set_height(int value);

	private:
		int x = 0, y = 0;
		int width = 0, height = 0;
	};
}

#endif
