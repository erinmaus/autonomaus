// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_EVENT_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_EVENT_PROCESSOR_HPP

#include "touchstone/trace/call.hpp"

namespace rosalind
{
	class EventProcessor
	{
	public:
		EventProcessor() = default;
		virtual ~EventProcessor() = default;

		virtual void start_frame();
		virtual void before_update(const touchstone::Call* call);
		virtual void after_update(const touchstone::Call* call);
	};
}

#endif
