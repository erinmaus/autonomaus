// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_EVENT_PROCESSOR_HPP
#define KOMPROMAT_EVENT_PROCESSOR_HPP

#include <string>
#include "paley/event/event.hpp"

namespace kompromat
{
	class EventProcessor
	{
	public:
		EventProcessor() = default;
		virtual ~EventProcessor() = default;

		virtual void process(
			const std::string& scope,
			int name,
			const paley::Event& event) = 0;
	};
}

#endif
