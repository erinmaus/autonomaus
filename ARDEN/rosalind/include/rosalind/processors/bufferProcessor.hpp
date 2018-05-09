// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_BUFFER_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_BUFFER_PROCESSOR_HPP

#include "kompvter/events/bufferEvent.hpp"
#include "kompvter/listeners/bufferListener.hpp"
#include "rosalind/processors/listenerProcessor.hpp"
#include "touchstone/gl/calls.hpp"
#include "touchstone/gl/context.hpp"
#include "touchstone/trace/call.hpp"

namespace rosalind
{
	class BufferProcessor : public ListenerProcessor<kompvter::BufferListener>
	{
	public:
		BufferProcessor(const touchstone::Context* context);
		~BufferProcessor() = default;

		void before_update(const touchstone::Call* call) override;
		void after_update(const touchstone::Call* call) override;

	private:
		void process_create_buffers_after(const touchstone::call::GenBuffers* call);
		void process_buffer_data_before(const touchstone::call::BufferData* call);
		void process_buffer_data_after(const touchstone::call::BufferData* call);
		void process_buffer_data_after(const touchstone::call::BufferSubData* call);
		void process_delete_buffers_after(const touchstone::call::DeleteBuffers* call);

		const touchstone::Context* context;
	};
}

#endif
