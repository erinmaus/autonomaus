// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVNCER_RENDER_BUFFER_STORE_HPP
#define KVNCER_RENDER_BUFFER_STORE_HPP

#include <memory>
#include <set>
#include <unordered_map>
#include <vector>
#include "kompvter/graphics/bufferManager.hpp"
#include "kompvter/listeners/bufferListener.hpp"
#include "kvncer/graphics/meshBuffer.hpp"

namespace kvncer
{
	class BufferStore : public kompvter::BufferListener
	{
	public:
		BufferStore(kompvter::BufferManager& buffer_manager);
		~BufferStore() = default;

		MeshBuffer* get_buffer(int name);

		void reset();

	protected:
		void on_buffer_create(const kompvter::BufferEvent& event) override;
		void on_buffer_delete(const kompvter::BufferEvent& event) override;
		void on_buffer_allocate(const kompvter::BufferEvent& event) override;
		void on_buffer_clear(const kompvter::BufferEvent& event) override;
		void on_buffer_update(const kompvter::BufferEvent& event) override;

	private:
		void mark_dirty(int name);

		std::set<int> dirty_buffers;
		std::unordered_map<int, std::size_t> latest_buffers;

		typedef std::unique_ptr<MeshBuffer> MeshBufferPointer;
		std::vector<MeshBufferPointer> buffers;

		kompvter::BufferManager& buffer_manager;
	};
}

#endif
