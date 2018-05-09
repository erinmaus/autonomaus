// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/events/bufferEvent.hpp"
#include "kompvter/graphics/bufferStatus.hpp"

bool kompvter::BufferStatus::has_buffer(int name) const
{
	auto iter = this->buffers.find(name);
	return iter != this->buffers.end();
}

int kompvter::BufferStatus::get_buffer_status(int name) const
{
	auto iter = this->buffers.find(name);
	if (iter == this->buffers.end())
	{
		return status_none;
	}

	return iter->second;
}

bool kompvter::BufferStatus::touch_buffer(int name)
{
	auto iter = this->buffers.find(name);
	if (iter != this->buffers.end())
	{
		iter->second = status_clean;
		return true;
	}

	return false;
}

void kompvter::BufferStatus::touch_all()
{
	for (auto& buffer: this->buffers)
	{
		buffer.second = status_clean;
	}
}

void kompvter::BufferStatus::on_buffer_create(const BufferEvent& event)
{
	this->buffers[event.get_name()] = status_dirty;
}

void kompvter::BufferStatus::on_buffer_delete(const BufferEvent& event)
{
	this->buffers.erase(event.get_name());
}

void kompvter::BufferStatus::on_buffer_allocate(const BufferEvent& event)
{
	this->buffers[event.get_name()] = status_dirty;
}

void kompvter::BufferStatus::on_buffer_clear(const BufferEvent& event)
{
	this->buffers[event.get_name()] = status_dirty;
}

void kompvter::BufferStatus::on_buffer_update(const BufferEvent& event)
{
	this->buffers[event.get_name()] = status_dirty;
}
