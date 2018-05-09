// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "kvlc/collision/scene.hpp"
#include "kvlc/collision/query.hpp"

std::size_t kvlc::Query::count() const
{
	return this->results.size();
}

std::size_t kvlc::Query::count(Scene::Tag tag) const
{
	return this->results.count(tag);
}

bool kvlc::Query::empty() const
{
	return this->results.empty();
}

void kvlc::Query::add_result(Scene::Tag tag, const glm::vec3& point)
{
	assert(tag != Scene::TAG_NONE);
	this->results.emplace(tag, point);
}

kvlc::Query::const_iterator kvlc::Query::begin() const
{
	return this->results.begin();
}

kvlc::Query::const_iterator kvlc::Query::end() const
{
	return this->results.end();
}
