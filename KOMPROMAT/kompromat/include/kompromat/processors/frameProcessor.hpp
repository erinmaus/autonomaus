// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_PROCESSORS_FRAME_PROCESSOR_HPP
#define KOMPROMAT_PROCESSORS_FRAME_PROCESSOR_HPP

#include <string>
#include "kompromat/listenerProcessor.hpp"
#include "kompvter/listeners/frameListener.hpp"
#include "paley/event/event.hpp"
#include "paley/messaging/client.hpp"

namespace kompromat
{
	class FrameProcessor : public ListenerProcessor<kompvter::FrameListener>
	{
	public:
		FrameProcessor() = default;
		~FrameProcessor() = default;

		void process(
			const std::string& scope,
			int name,
			const paley::Event& event) override;

		static void request_frame(paley::Client& client);

	private:
		void process_draw_frame(const paley::Event& event);
		void process_draw_enabled(const paley::Event& event);
		void process_draw_disabled(const paley::Event& event);

		static float get_timestamp(const paley::Event& event);
	};
}

#endif
