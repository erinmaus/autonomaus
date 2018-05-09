// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/guiDrawEvent.hpp"
#include "kompvter/events/itemIconEvent.hpp"
#include "kompvter/events/modelDrawEvent.hpp"
#include "kompvter/listeners/itemIconListener.hpp"

void kompvter::ItemIconListener::on_draw_gui(const GUIDrawEvent&)
{
	// Nothing.
}

void kompvter::ItemIconListener::on_draw_model(const ModelDrawEvent&)
{
	// Nothing.
}

void kompvter::ItemIconListener::on_transfer(const ItemIconEvent&)
{
	// Nothing.
}

void kompvter::ItemIconListener::on_clear(const ItemIconEvent&)
{
	// Nothing.
}
