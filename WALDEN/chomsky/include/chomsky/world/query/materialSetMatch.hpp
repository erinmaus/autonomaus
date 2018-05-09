// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_QUERY_MATERIAL_SET_MATCH_HPP
#define CHOMSKY_WORLD_QUERY_MATERIAL_SET_MATCH_HPP

#include <vector>
#include "chomsky/world/query/materialDefinition.hpp"

namespace chomsky
{
	class MaterialSetMatch
	{
	public:
		MaterialSetMatch() = default;
		~MaterialSetMatch() = default;

		void add(const MaterialDefinition& definition);
		const MaterialDefinition& get(std::size_t index) const;
		void remove(std::size_t index);

		std::size_t count() const;
		bool empty() const;

	private:
		typedef std::vector<MaterialDefinition> MaterialDefinitions;
		MaterialDefinitions definitions;

	public:
		typedef MaterialDefinitions::const_iterator const_iterator;
		typedef MaterialDefinitions::iterator iterator;

		const_iterator begin() const;
		iterator begin();
		const_iterator end() const;
		iterator end();
	};
}

#endif
