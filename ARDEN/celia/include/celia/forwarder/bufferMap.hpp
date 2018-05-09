// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CELIA_FORWARDER_BUFFER_MAP_HPP
#define CELIA_FORWARDER_BUFFER_MAP_HPP

#include <map>
#include <set>
#include "ganymede/ganymede.hpp"
#include "celia/common/buffer.hpp"

namespace celia
{
	class BufferMap
	{
	public:
		static void bind(GLenum target, GLuint buffer);
		static GLuint get_current(GLenum target);

		static void* map(GLuint buffer, Buffer& data, bool sync_on_unmap);
		static void unmap(GLuint buffer, Buffer& output);

	private:
		struct Instance
		{
			std::map<GLenum, GLuint> current_buffers;
			std::map<GLuint, Buffer> mapped_buffers;
			std::set<GLuint> synced_buffers;
		};

		static Instance instance;
	};
}

#endif
