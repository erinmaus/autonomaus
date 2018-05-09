// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_LISTENERS_MODEL_LISTENER_HPP
#define KOMPVTER_LISTENERS_MODEL_LISTENER_HPP

namespace kompvter
{
	class ModelDrawEvent;

	class ModelListener
	{
	public:
		ModelListener() = default;
		virtual ~ModelListener() = default;

		virtual void on_draw_model(const ModelDrawEvent& event);
		virtual void on_draw_terrain(const ModelDrawEvent& event);
		virtual void on_draw_water(const ModelDrawEvent& event);
		virtual void on_draw_particle(const ModelDrawEvent& event);
	};
}

#endif
