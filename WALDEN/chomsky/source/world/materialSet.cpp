// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include <tuple>
#include "chomsky/world/materialSet.hpp"

void chomsky::MaterialSet::add(
	int tag,
	int texture_name, int texture_region,
	const glm::vec3& color)
{
	auto range = this->materials.equal_range(tag);
	auto iter = range.second;
	for (auto i = range.first; i != range.second; ++i)
	{
		auto& state = i->second;
		if (state.texture_name == texture_name && state.texture_region == texture_region)
		{
			iter = i;
			break;
		}
	}

	if (iter == range.second)
	{
		MaterialState state;
		state.texture_name = texture_name;
		state.texture_region = texture_region;
		state.weight = 1.0f;
		state.color_sum = color;

		this->materials.emplace_hint(range.second, tag, state);
	}
	else
	{
		iter->second.weight += 1.0f;
		iter->second.color_sum += color;
	}
}

bool chomsky::MaterialSet::has(int tag) const
{
	return this->materials.count(tag) != 0;
}

void chomsky::MaterialSet::remove(int tag)
{
	this->materials.erase(tag);
}

std::size_t chomsky::MaterialSet::count() const
{
	return this->materials.size();
}

std::size_t chomsky::MaterialSet::count(int tag) const
{
	return this->materials.count(tag);
}

bool chomsky::MaterialSet::empty() const
{
	return this->materials.empty();
}

chomsky::MaterialSet::const_iterator chomsky::MaterialSet::begin() const
{
	return const_iterator(this->materials.begin());
}

chomsky::MaterialSet::const_iterator chomsky::MaterialSet::begin(int tag) const
{
	return const_iterator(this->materials.lower_bound(tag));
}

chomsky::MaterialSet::const_iterator chomsky::MaterialSet::end() const
{
	return const_iterator(this->materials.end());
}

chomsky::MaterialSet::const_iterator chomsky::MaterialSet::end(int tag) const
{
	return const_iterator(this->materials.upper_bound(tag));
}

chomsky::MaterialSet::const_iterator::const_iterator(
	const MaterialDictionary::const_iterator& source) :
		MaterialDictionary::const_iterator(source)
{
	// Nothing.
}

chomsky::MaterialSet::const_iterator::value_type
chomsky::MaterialSet::const_iterator::operator *() const
{
	auto& pair = MaterialDictionary::const_iterator::operator *();
	auto& state = pair.second;
	int tag = pair.first;
	auto color = state.color_sum * (1.0f / state.weight);
	return std::make_tuple(tag, state.texture_name, state.texture_region, color, state.weight);
}
