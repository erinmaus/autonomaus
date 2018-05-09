// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_RESOURCES_TEXTURE_DICTIONARY_HPP
#define CHOMSKY_RESOURCES_TEXTURE_DICTIONARY_HPP

#include <tuple>
#include <map>
#include <string>
#include <boost/container/flat_map.hpp>
#include "chomsky/resources/textureDefinition.hpp"
#include "chomsky/resources/textureFingerprint.hpp"

namespace chomsky
{
	class TextureDictionary
	{
	public:
		TextureDictionary() = default;
		~TextureDictionary() = default;

		TextureDefinition& add(
			const std::string& key);
		TextureDefinition& add(
			const std::string& key,
			const TextureFingerprint& fingerprint);
		void remove(const std::string& key);

		bool has(const std::string& key) const;
		TextureDefinition& get(const std::string& key);
		const TextureDefinition& get(const std::string& key) const;

		bool try_find(
			const TextureFingerprint& query,
			std::string& result,
			float max_difference_threshold = HUGE_VALF,
			float max_dissimilarity = HUGE_VALF) const;

		std::size_t count() const;
		bool empty() const;

	private:
		typedef std::tuple<int /* width */, int /* height */> WidthHeightTuple;
		boost::container::flat_multimap<WidthHeightTuple, TextureDefinition*> search_space;

		typedef std::map<std::string, TextureDefinition> Definitions;
		Definitions definitions;

	public:
		typedef Definitions::const_iterator iterator;
		typedef Definitions::const_iterator const_iterator;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
	};
}

#endif
