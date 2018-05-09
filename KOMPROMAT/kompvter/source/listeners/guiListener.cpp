// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/viewportEvent.hpp"
#include "kompvter/listeners/guiListener.hpp"

void kompvter::GUIListener::on_draw_gui(const GUIDrawEvent&)
{
	// Nada.
}

void kompvter::GUIListener::on_present()
{
	// Nada.
}

void kompvter::GUIListener::on_set_viewport(const ViewportEvent& event)
{
	// Nada.
}
