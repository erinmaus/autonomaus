// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include "kompvter/events/bufferEvent.hpp"
#include "kompvter/graphics/bufferManager.hpp"

bool kompvter::BufferManager::has_buffer(int name) const
{
	auto iter = this->buffers.find(name);
	return iter != this->buffers.end();
}

kompvter::Buffer kompvter::BufferManager::get_buffer(int name) const
{
	auto iter = this->buffers.find(name);
	if (iter == this->buffers.end())
	{
		return Buffer();
	}

	return Buffer(iter->first, iter->second);
}

void kompvter::BufferManager::reset()
{
	this->buffers.clear();
}

kompvter::BufferManager::const_iterator kompvter::BufferManager::begin() const
{
	return const_iterator(this->buffers.begin());
}

kompvter::BufferManager::const_iterator kompvter::BufferManager::end() const
{
	return const_iterator(this->buffers.end());
}

void kompvter::BufferManager::on_buffer_create(const BufferEvent& event)
{
	this->buffers[event.get_name()] = BufferDataPointer(new BufferData());
}

void kompvter::BufferManager::on_buffer_delete(const BufferEvent& event)
{
	auto iter = this->buffers.find(event.get_name());
	if (iter != this->buffers.end())
	{
		this->buffers.erase(iter);
	}
}

void kompvter::BufferManager::on_buffer_allocate(const BufferEvent& event)
{
	auto iter = this->buffers.find(event.get_name());
	if (iter != this->buffers.end())
	{
		iter->second = BufferDataPointer(new BufferData(event.get_data_size()));
	}
}

void kompvter::BufferManager::on_buffer_clear(const BufferEvent& event)
{
	auto iter = this->buffers.find(event.get_name());
	if (iter != this->buffers.end())
	{
		if (iter->second.use_count() == 1)
		{
			iter->second->clear();
		}
		else
		{
			iter->second = BufferDataPointer(new BufferData(iter->second->get_data_size()));
		}
	}
}

void kompvter::BufferManager::on_buffer_update(const BufferEvent& event)
{
	auto iter = this->buffers.find(event.get_name());
	if (iter != this->buffers.end())
	{
		if (iter->second.use_count() == 1)
		{
			iter->second->update(
				event.get_offset(),
				event.get_data(), event.get_data_size());
		}
		else
		{
			auto result = BufferDataPointer(new BufferData(iter->second->get_data_size()));
			if (event.get_offset() != 0 || event.get_data_size() != iter->second->get_data_size())
			{
				result->update(0, iter->second->get_data(), iter->second->get_data_size());
			}
			result->update(event.get_offset(), event.get_data(), event.get_data_size());
			iter->second = result;
		}
	}
}

kompvter::BufferManager::const_iterator::const_iterator(const BufferDataMap::const_iterator& other) :
	BufferDataMap::const_iterator(other), current(0, Buffer())
{
	// Nothing.
}

kompvter::BufferManager::const_iterator::value_type*
kompvter::BufferManager::const_iterator::operator ->() const
{
	auto& result = operator *();
	return &result;
}

kompvter::BufferManager::const_iterator::value_type&
kompvter::BufferManager::const_iterator::operator *() const
{
	auto& result = BufferDataMap::const_iterator::operator *();
	if (result.first != current.first)
	{
		current = std::make_pair(result.first, Buffer(result.first, result.second));
	}

	return current;
}
