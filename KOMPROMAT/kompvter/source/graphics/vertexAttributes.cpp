// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kompvter/graphics/vertexAttributes.hpp"

void kompvter::VertexAttributes::add(
	const std::string& name, const VertexFetcher& fetcher)
{
	this->attributes[name] = fetcher;
}

bool kompvter::VertexAttributes::has(const std::string& name) const
{
	return get(name) != nullptr;
}

const kompvter::VertexFetcher* kompvter::VertexAttributes::get(
	const std::string& name) const
{
	auto iter = this->attributes.find(name);
	if (iter == this->attributes.end())
	{
		return nullptr;
	}

	return &iter->second;
}

kompvter::VertexAttributes::iterator kompvter::VertexAttributes::begin() const
{
	return this->attributes.begin();
}

kompvter::VertexAttributes::iterator kompvter::VertexAttributes::end() const
{
	return this->attributes.end();
}
