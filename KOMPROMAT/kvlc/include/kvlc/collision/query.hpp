// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KVLC_COLLISION_QUERY_HPP
#define KVLC_COLLISION_QUERY_HPP

#include <map>
#include <glm/glm.hpp>
#include "kvlc/collision/scene.hpp"

namespace kvlc
{
	class Query
	{
	public:
		friend class Scene;

		typedef std::pair<Scene::Tag, glm::vec3> Hit;

		Query() = default;
		~Query() = default;

		std::size_t count() const;
		std::size_t count(Scene::Tag tag) const;
		bool empty() const;

		void add_result(Scene::Tag tag, const glm::vec3& point);

	private:
		typedef std::multimap<Scene::Tag, glm::vec3> ResultCollection;
		ResultCollection results;

	public:
		typedef ResultCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
