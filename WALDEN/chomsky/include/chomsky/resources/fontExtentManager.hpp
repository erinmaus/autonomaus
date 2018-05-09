// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_FONT_EXTENT_MANAGER_HPP
#define CHOMSKY_RESOURCES_FONT_EXTENT_MANAGER_HPP

#include <cstdint>
#include <map>
#include <glm/glm.hpp>
#include "chomsky/resources/textureHash.hpp"
#include "kompvter/graphics/textureManager.hpp"
#include "kompvter/graphics/textureRegion.hpp"

namespace chomsky
{
	class FontExtentManager
	{
	public:
		FontExtentManager() = default;
		~FontExtentManager() = default;

		bool has_extents(const std::string& key) const;
		bool generate_extents(
			const std::string& key,
			const kompvter::TextureRegion& region);

		glm::vec2 get_size(
			const std::string& key, std::uint32_t character) const;
		glm::vec2 get_offset(
			const std::string& key, std::uint32_t character) const;

	private:
		struct Extent
		{
			glm::vec2 position;
			glm::vec2 size;
		};
		typedef std::map<char, Extent> CharacterExtentMap;
		std::map<std::string, CharacterExtentMap> extents;
	};
}

#endif
