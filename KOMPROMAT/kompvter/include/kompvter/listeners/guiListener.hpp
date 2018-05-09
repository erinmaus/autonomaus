// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_LISTENERS_GUI_LISTENER_HPP
#define KOMPVTER_LISTENERS_GUI_LISTENER_HPP

namespace kompvter
{
	class GUIDrawEvent;
	class ViewportEvent;

	class GUIListener
	{
	public:
		GUIListener() = default;
		virtual ~GUIListener() = default;

		virtual void on_draw_gui(const GUIDrawEvent& event);
		virtual void on_present();
		virtual void on_set_viewport(const ViewportEvent& event);
	};
}

#endif
