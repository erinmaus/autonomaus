// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPVTER_GRAPHICS_TEXTURE_HPP
#define KOMPVTER_GRAPHICS_TEXTURE_HPP

#include <cstddef>
#include <cstdint>
#include <map>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include "kompvter/graphics/textureRegion.hpp"

namespace kompvter
{
	class Texture
	{
	public:
		friend class TextureManager;

		struct const_iterator;
		typedef int TextureRegionName;

		Texture() = default;
		Texture(
			int name,
			int width, int height,
			int pixel_format, int pixel_type, bool is_compressed);
		~Texture() = default;

		int get_name() const;
		int get_width() const;
		int get_height() const;

		int get_pixel_format() const;
		int get_pixel_type() const;
		bool get_is_compressed() const;

		TextureRegionName add_region(
			int x, int y,
			int width, int height,
			int pixel_format, int pixel_type, bool is_compressed,
			const std::uint8_t* pixels_data, std::size_t pixels_size);

		const TextureRegion* get_region_from_name(TextureRegionName name) const;
		const TextureRegion* get_region_from_pixel(int x, int y) const;
		const TextureRegion* get_region_from_texture_coordinate(float s, float t) const;

		const_iterator begin() const;
		const_iterator end() const;

	private:
		static const int HASH_SPACE_SIZE;
		struct Data;
		Texture(const std::shared_ptr<Data>& data);

		typedef std::unordered_map<TextureRegionName, TextureRegion> TextureRegionMap;

		typedef std::pair<int, int> TextureCellKey;
		typedef std::unordered_set<TextureRegionName> TextureCell;
		typedef std::map<TextureCellKey, TextureCell> TextureRegionHashSpace;
		void add_to_hash_space(TextureRegion& region);
		void remove_from_hash_space(TextureRegion& region);

		struct Data
		{
			Data() = default;
			Data(
				int name,
				int width, int height,
				int pixel_format, int pixel_type, bool is_compressed);
			Data(const Data& other) = default;
			~Data() = default;

			int name = 0;
			int width = 0, height = 0;

			int pixel_format = 0, pixel_type = 0;
			bool is_compressed = false;

			TextureRegionName current_name = 0;
			std::shared_ptr<TextureRegionMap> regions = std::make_shared<TextureRegionMap>();
			std::shared_ptr<TextureRegionHashSpace> hash_space = std::make_shared<TextureRegionHashSpace>();
		};
		typedef std::shared_ptr<Data> DataPointer;
		DataPointer data = std::make_shared<Data>();

	public:
		struct const_iterator : public TextureRegionMap::const_iterator
		{
		public:
			typedef TextureRegion value_type;

			const_iterator(TextureRegionMap::const_iterator source);
			const value_type& operator *() const;
			const value_type& operator ->() const;
		};
	};
}

#endif
