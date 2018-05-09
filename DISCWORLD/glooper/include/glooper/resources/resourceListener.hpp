// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef GLOOPER_RESOURCES_RESOURCE_LISTENER_HPP
#define GLOOPER_RESOURCES_RESOURCE_LISTENER_HPP

namespace glooper
{
	class ResourceListener
	{
	public:
		virtual ~ResourceListener() = default;

		virtual void on_reload_resources();
	};
}

#endif
