// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <cassert>
#include <boost/container/flat_set.hpp>
#include "chomsky/resources/textureDictionary.hpp"

chomsky::TextureDefinition& chomsky::TextureDictionary::add(
	const std::string& key)
{
	if (!has(key))
	{
		this->definitions[key] = TextureDefinition(key, TextureFingerprint());
	}

	return get(key);
}

chomsky::TextureDefinition& chomsky::TextureDictionary::add(
	const std::string& key,
	const TextureFingerprint& fingerprint)
{
	if (!has(key))
	{
		auto result = this->definitions.emplace(key, TextureDefinition(key, fingerprint)).first;

		auto search_space_key = std::make_tuple(
			fingerprint.get_texture_width(),
			fingerprint.get_texture_height());
		this->search_space.emplace(search_space_key, &result->second);
	}

	return get(key);
}

void chomsky::TextureDictionary::remove(const std::string& key)
{
	if (has(key))
	{
		auto& definition = get(key);
		auto& fingerprint = definition.get_fingerprint();
		auto search_space_key = std::make_tuple(
			fingerprint.get_texture_width(),
			fingerprint.get_texture_height());
		auto range = this->search_space.equal_range(search_space_key);
		for (auto i = range.first; i != range.second; ++i)
		{
			if (i->second == &definition)
			{
				this->search_space.erase(i);
				break;
			}
		}
	}

	this->definitions.erase(key);
}

bool chomsky::TextureDictionary::has(const std::string& key) const
{
	return this->definitions.count(key) != 0;
}

chomsky::TextureDefinition& chomsky::TextureDictionary::get(
	const std::string& key)
{
	auto iter = this->definitions.find(key);
	assert(iter != this->definitions.end());

	return iter->second;
}

const chomsky::TextureDefinition& chomsky::TextureDictionary::get(
	const std::string& key) const
{
	auto iter = this->definitions.find(key);
	assert(iter != this->definitions.end());

	return iter->second;
}

bool chomsky::TextureDictionary::try_find(
	const TextureFingerprint& query,
	std::string& result,
	float max_difference_threshold,
	float max_dissimilarity) const
{
	typedef std::pair<float, std::string> KeyWeightPair;
	boost::container::flat_set<KeyWeightPair> candidates;

	auto search_space_key = std::make_tuple(
		query.get_texture_width(),
		query.get_texture_height());
	auto range = this->search_space.equal_range(search_space_key);
	for (auto i = range.first; i != range.second; ++i)
	{
		auto candidate_result = TextureFingerprint::compare(
			query,
			i->second->get_fingerprint(),
			glm::vec3(max_difference_threshold));
		auto length = glm::length(glm::vec3(candidate_result));

		if (length < max_dissimilarity)
		{
			candidates.emplace(length, i->second->get_key());
		}
	}

	if (candidates.empty())
	{
		return false;
	}

	result = candidates.begin()->second;
	return true;
}

std::size_t chomsky::TextureDictionary::count() const
{
	return this->definitions.size();
}

bool chomsky::TextureDictionary::empty() const
{
	return this->definitions.empty();
}

chomsky::TextureDictionary::iterator
chomsky::TextureDictionary::begin()
{
	return this->definitions.begin();
}

chomsky::TextureDictionary::const_iterator
chomsky::TextureDictionary::begin() const
{
	return this->definitions.begin();
}

chomsky::TextureDictionary::iterator
chomsky::TextureDictionary::end()
{
	return this->definitions.end();
}

chomsky::TextureDictionary::const_iterator
chomsky::TextureDictionary::end() const
{
	return this->definitions.end();
}
