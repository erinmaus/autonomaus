// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENTS_VIEWPORT_EVENT_BUILDER_HPP
#define KOMPROMAT_EVENTS_VIEWPORT_EVENT_BUILDER_HPP

#include "kompvter/events/viewportEvent.hpp"
#include "paley/event/event.hpp"

namespace kompromat
{
	class ViewportEventBuilder
	{
	public:
		ViewportEventBuilder() = default;
		~ViewportEventBuilder() = default;

		const kompvter::ViewportEvent& get_event() const;

		void from_set_viewport(const paley::Event& event);

	private:
		kompvter::ViewportEvent event;
	};
}

#endif
