// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "celia/forwarder/bufferMap.hpp"

celia::BufferMap::Instance celia::BufferMap::instance;

void celia::BufferMap::bind(GLenum target, GLuint buffer)
{
	instance.current_buffers[target] = buffer;
}

GLuint celia::BufferMap::get_current(GLenum target)
{
	auto iter = instance.current_buffers.find(target);
	if (iter == instance.current_buffers.end())
	{
		return 0;
	}

	return iter->second;
}

void* celia::BufferMap::map(GLuint buffer, Buffer& data, bool sync_on_unmap)
{
	instance.mapped_buffers[buffer] = std::move(data);

	if (sync_on_unmap)
	{
		instance.synced_buffers.insert(buffer);
	}

	return instance.mapped_buffers[buffer].get_data();
}

void celia::BufferMap::unmap(GLuint buffer, Buffer& output)
{
	auto iter = instance.synced_buffers.find(buffer);
	if (iter != instance.synced_buffers.end())
	{
		output.push(instance.mapped_buffers[buffer].get_data_size());
		output.push_data(
			instance.mapped_buffers[buffer].get_data(),
			instance.mapped_buffers[buffer].get_data_size());

		instance.synced_buffers.erase(iter);
	}
	else
	{
		output.push((std::size_t)0);
	}

	instance.mapped_buffers.erase(instance.mapped_buffers.find(buffer));
}
