// This file is a part of PRINCIPLE.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef PRINCIPLE_RESOURCES_TEXTURE_MANAGER_HPP
#define PRINCIPLE_RESOURCES_TEXTURE_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include "kompvter/events/textureEvent.hpp"
#include "kompvter/listeners/textureListener.hpp"
#include "kvncer/graphics/texture.hpp"
#include "principle/graphics/display.hpp"

namespace principle
{
	class TextureManager : public kompvter::TextureListener
	{
	public:
		TextureManager() = default;
		~TextureManager() = default;

		void reset();

		void on_texture_delete(const kompvter::TextureEvent& event) override;
		void on_texture_allocate(const kompvter::TextureEvent& event) override;
		void on_texture_update(const kompvter::TextureEvent& event) override;
		void on_texture_copy(const kompvter::TextureEvent& event) override;

		kvncer::Texture* get_texture(int name) const;

	private:
		typedef std::unique_ptr<kvncer::Texture> TexturePointer;
		std::unordered_map<int, TexturePointer> textures;
	};
}

#endif
