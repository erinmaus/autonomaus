// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_PROCESSORS_BUFFER_PROCESSOR_HPP
#define KOMPROMAT_PROCESSORS_BUFFER_PROCESSOR_HPP

#include <memory>
#include <unordered_map>
#include "kompromat/listenerProcessor.hpp"
#include "kompvter/listeners/bufferListener.hpp"

namespace kompromat
{
	class BufferProcessor : public ListenerProcessor<kompvter::BufferListener>
	{
	public:
		BufferProcessor() = default;
		~BufferProcessor() = default;

		void process(
			const std::string& scope,
			int name,
			const paley::Event& event) override;

	private:
		void process_buffer_create(const paley::Event& event);
		void process_buffer_delete(const paley::Event& event);
		void process_buffer_allocate(const paley::Event& event);
		void process_buffer_clear(const paley::Event& event);
		void process_buffer_update(const paley::Event& event);
	};
}

#endif
