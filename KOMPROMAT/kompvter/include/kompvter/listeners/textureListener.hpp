// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_LISTENERS_TEXTURE_LISTENER_HPP
#define KOMPVTER_LISTENERS_TEXTURE_LISTENER_HPP

namespace kompvter
{
	class TextureEvent;

	class TextureListener
	{
	public:
		TextureListener() = default;
		~TextureListener() = default;

		virtual void on_texture_create(const TextureEvent& event);
		virtual void on_texture_delete(const TextureEvent& event);
		virtual void on_texture_allocate(const TextureEvent& event);
		virtual void on_texture_update(const TextureEvent& event);
		virtual void on_texture_copy(const TextureEvent& event);
		virtual void on_texture_clear(const TextureEvent& event);
	};
}

#endif
