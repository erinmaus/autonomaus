// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/events/bufferEvent.hpp"
#include "kvncer/render/bufferStore.hpp"

kvncer::BufferStore::BufferStore(kompvter::BufferManager& buffer_manager)
	: buffer_manager(buffer_manager)
{
	reset();
}

kvncer::MeshBuffer* kvncer::BufferStore::get_buffer(int name)
{
	bool is_dirty;
	{
		auto dirty = dirty_buffers.find(name);
		is_dirty = dirty != dirty_buffers.end();
	}

	if (is_dirty)
	{
		assert(buffer_manager.has_buffer(name));
		auto buffer = buffer_manager.get_buffer(name);

		auto mesh_buffer = std::make_unique<MeshBuffer>();
		mesh_buffer->set_buffer(buffer);

		this->buffers.push_back(std::move(mesh_buffer));
		this->latest_buffers[name] = this->buffers.size() - 1;

		this->dirty_buffers.erase(name);
	}

	auto iter = this->latest_buffers.find(name);
	assert(iter != this->latest_buffers.end());
	return this->buffers.at(iter->second).get();
}

void kvncer::BufferStore::reset()
{
	this->dirty_buffers.clear();
	this->buffers.clear();
	this->latest_buffers.clear();

	for (auto buffer: buffer_manager)
	{
		this->dirty_buffers.insert(buffer.first);
	}
}

void kvncer::BufferStore::on_buffer_create(const kompvter::BufferEvent& event)
{
	mark_dirty(event.get_name());
}

void kvncer::BufferStore::on_buffer_delete(const kompvter::BufferEvent& event)
{
	this->dirty_buffers.erase(event.get_name());

	auto iter = this->latest_buffers.find(event.get_name());
	if (iter != this->latest_buffers.end())
	{
		this->latest_buffers.erase(iter);
	}
}

void kvncer::BufferStore::on_buffer_allocate(const kompvter::BufferEvent& event)
{
	mark_dirty(event.get_name());
}

void kvncer::BufferStore::on_buffer_clear(const kompvter::BufferEvent& event)
{
	mark_dirty(event.get_name());
}

void kvncer::BufferStore::on_buffer_update(const kompvter::BufferEvent& event)
{
	mark_dirty(event.get_name());
}

void kvncer::BufferStore::mark_dirty(int name)
{
	this->dirty_buffers.insert(name);
}
