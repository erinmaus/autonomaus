// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_TEXTURE_MANAGER_HPP
#define KOMPVTER_GRAPHICS_TEXTURE_MANAGER_HPP

#include <memory>
#include <unordered_map>
#include "kompvter/graphics/texture.hpp"
#include "kompvter/listeners/textureListener.hpp"

namespace kompvter
{
	class TextureManager : public TextureListener
	{
	public:
		TextureManager() = default;
		TextureManager(const TextureManager& other) = delete;
		~TextureManager() = default;

		Texture get_texture(int name) const;
		bool has_texture(int name) const;

		void reset();

		void on_texture_create(const TextureEvent& event) override;
		void on_texture_delete(const TextureEvent& event) override;
		void on_texture_allocate(const TextureEvent& event) override;
		void on_texture_update(const TextureEvent& event) override;
		void on_texture_copy(const TextureEvent& event) override;

		TextureManager& operator = (const TextureManager& other) = delete;

	private:
		typedef std::unordered_map<int, Texture::DataPointer> TextureDataMap;
		TextureDataMap textures;

	public:
		struct const_iterator : public TextureDataMap::const_iterator
		{
		public:
			typedef std::pair<int, Texture> value_type;

			const_iterator(const TextureDataMap::const_iterator& other);
			~const_iterator() = default;

			value_type* operator ->() const;
			value_type& operator *() const;

		private:
			mutable value_type current;
		};

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
