// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_WORLD_STATE_HPP
#define CHOMSKY_WORLD_WORLD_STATE_HPP

#include <cassert>
#include <unordered_map>

namespace chomsky
{
	template <typename Coordinate, typename Object>
	class WorldState
	{
	public:
		WorldState() = default;
		~WorldState() = default;

		bool has(const Coordinate& location) const;

		const Object& get(const Coordinate& location) const;
		Object& get(const Coordinate& location);

		Object& add(const Coordinate& location);
		void remove(const Coordinate& location);

		void clear();

		bool empty() const;
		std::size_t count() const;

	private:
		typedef std::unordered_map<Coordinate, Object> ObjectMap;
		ObjectMap groups;

	public:
		typedef typename ObjectMap::iterator iterator;
		typedef typename ObjectMap::const_iterator const_iterator;

		iterator remove(const iterator& iter);
		const_iterator remove(const const_iterator& iter);

		iterator begin();
		const_iterator begin() const;

		iterator end();
		const_iterator end() const;
	};
}

template <typename Coordinate, typename Object>
bool chomsky::WorldState<Coordinate, Object>::has(
	const Coordinate& location) const
{
	return this->groups.find(location) != this->groups.end();
}

template <typename Coordinate, typename Object>
const Object& chomsky::WorldState<Coordinate, Object>::get(
	const Coordinate& location) const
{
	auto iter = this->groups.find(location);
	assert(iter != this->groups.end());

	return iter->second;
}

template <typename Coordinate, typename Object>
Object& chomsky::WorldState<Coordinate, Object>::get(const Coordinate& location)
{
	return this->groups[location];
}

template <typename Coordinate, typename Object>
Object& chomsky::WorldState<Coordinate, Object>::add(const Coordinate& location)
{
	return this->groups[location];
}

template <typename Coordinate, typename Object>
void chomsky::WorldState<Coordinate, Object>::remove(const Coordinate& location)
{
	this->groups.erase(location);
}

template <typename Coordinate, typename Object>
void chomsky::WorldState<Coordinate, Object>::clear()
{
	this->groups.clear();
}

template <typename Coordinate, typename Object>
bool chomsky::WorldState<Coordinate, Object>::empty() const
{
	return this->groups.empty();
}

template <typename Coordinate, typename Object>
std::size_t chomsky::WorldState<Coordinate, Object>::count() const
{
	return this->groups.size();
}

template <typename Coordinate, typename Object>
typename chomsky::WorldState<Coordinate, Object>::iterator
chomsky::WorldState<Coordinate, Object>::remove(const iterator& iterator)
{
	return this->groups.erase(iterator);
}

template <typename Coordinate, typename Object>
typename chomsky::WorldState<Coordinate, Object>::const_iterator
chomsky::WorldState<Coordinate, Object>::remove(const const_iterator& iterator)
{
	return this->groups.erase(iterator);
}

template <typename Coordinate, typename Object>
typename chomsky::WorldState<Coordinate, Object>::iterator
chomsky::WorldState<Coordinate, Object>::begin()
{
	return this->groups.begin();
}

template <typename Coordinate, typename Object>
typename chomsky::WorldState<Coordinate, Object>::const_iterator
chomsky::WorldState<Coordinate, Object>::begin() const
{
	return this->groups.begin();
}

template <typename Coordinate, typename Object>
typename chomsky::WorldState<Coordinate, Object>::iterator
chomsky::WorldState<Coordinate, Object>::end()
{
	return this->groups.end();
}

template <typename Coordinate, typename Object>
typename chomsky::WorldState<Coordinate, Object>::const_iterator
chomsky::WorldState<Coordinate, Object>::end() const
{
	return this->groups.end();
}

#endif
