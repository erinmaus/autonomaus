// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_LISTENERS_FRAME_LISTENER_HPP
#define KOMPVTER_LISTENERS_FRAME_LISTENER_HPP

namespace kompvter
{
	class FrameListener
	{
	public:
		FrameListener() = default;
		virtual ~FrameListener() = default;

		virtual void on_draw_frame(float timestamp);
		virtual void on_draw_enabled(float timestamp);
		virtual void on_draw_disabled(float timestamp);
	};
}

#endif
