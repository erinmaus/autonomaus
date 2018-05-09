// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_FRAME_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_FRAME_PROCESSOR_HPP

#include "kompvter/listeners/frameListener.hpp"
#include "rosalind/processors/listenerProcessor.hpp"

namespace rosalind
{
	class FrameProcessor : public ListenerProcessor<kompvter::FrameListener>
	{
	public:
		FrameProcessor() = default;
		~FrameProcessor() = default;

		void on_frame(float timestamp);
		void enable_draw(float timestamp);
		void disable_draw(float timestamp);

	private:
		bool draw_enabled = false;
	};
}

#endif
