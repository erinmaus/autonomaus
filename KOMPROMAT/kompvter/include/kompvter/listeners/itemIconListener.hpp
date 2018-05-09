// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_LISTENERS_ITEM_ICON_LISTENER_HPP
#define KOMPVTER_LISTENERS_ITEM_ICON_LISTENER_HPP

namespace kompvter
{
	class ItemIconEvent;
	class GUIDrawEvent;
	class ModelDrawEvent;

	class ItemIconListener
	{
	public:
		virtual void on_draw_gui(const GUIDrawEvent& event);
		virtual void on_draw_model(const ModelDrawEvent& event);
		virtual void on_transfer(const ItemIconEvent& event);
		virtual void on_clear(const ItemIconEvent& event);
	};
}

#endif
