// This file is a part of WALDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef CHOMSKY_WORLD_DETAIL_DRAW_RESOURCE_COLLECTION_HPP
#define CHOMSKY_WORLD_DETAIL_DRAW_RESOURCE_COLLECTION_HPP

#include <unordered_map>
#include <unordered_set>

namespace chomsky
{
	class WorldUpdateManager;
	class TagManager;

	template <typename Resource>
	struct DrawResourceCollection
	{
	public:
		DrawResourceCollection() = default;
		~DrawResourceCollection();

		void add_resource(Resource* resource);
		bool has_resource(Resource* resource) const;
		void invalidate_resource(Resource* resource);

		bool has_resource_with_buffer(int buffer) const;
		Resource* get_resource_with_buffer(int buffer) const;
		void attach_buffer_to_resource(int buffer, Resource* resource);
		void invalidate_resource_with_buffer(int buffer);

		void clear_invalid();

	private:
		std::unordered_set<Resource*> resources;
		std::unordered_set<Resource*> valid;
		std::unordered_set<Resource*> invalid;
		std::unordered_map<int, Resource*> buffers;

	public:
		typedef typename std::unordered_set<Resource*>::iterator iterator;
		typedef typename std::unordered_set<Resource*>::const_iterator const_iterator;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
	};
}

template <typename Resource>
chomsky::DrawResourceCollection<Resource>::~DrawResourceCollection()
{
	for (auto resource: this->resources)
	{
		delete resource;
	}
}

template <typename Resource>
void chomsky::DrawResourceCollection<Resource>::add_resource(Resource* resource)
{
	assert(!has_resource(resource));
	this->resources.insert(resource);
	this->valid.insert(resource);
}

template <typename Resource>
bool chomsky::DrawResourceCollection<Resource>::has_resource(
	Resource *resource) const
{
	return this->resources.find(resource) != this->resources.end();
}

template <typename Resource>
void chomsky::DrawResourceCollection<Resource>::invalidate_resource(
	Resource* resource)
{
	assert(has_resource(resource));

	for (auto i = this->buffers.begin(); i != this->buffers.end(); /* Nil. */)
	{
		if (i->second == resource)
		{
			i = this->buffers.erase(i);
		}
		else
		{
			++i;
		}
	}

	this->valid.erase(resource);
	this->invalid.insert(resource);
}

template <typename Resource>
bool chomsky::DrawResourceCollection<Resource>::has_resource_with_buffer(
	int buffer) const
{
	return this->buffers.find(buffer) != this->buffers.end(); 
}

template <typename Resource>
Resource* chomsky::DrawResourceCollection<Resource>::get_resource_with_buffer(
	int buffer) const
{
	assert(has_resource_with_buffer(buffer));
	return this->buffers.find(buffer)->second;
}

template <typename Resource>
void chomsky::DrawResourceCollection<Resource>::attach_buffer_to_resource(
	int buffer, Resource* resource)
{
	assert(!has_resource_with_buffer(buffer));
	this->buffers.emplace(buffer, resource);
}

template <typename Resource>
void chomsky::DrawResourceCollection<Resource>::invalidate_resource_with_buffer(
	int buffer)
{
	if (has_resource_with_buffer(buffer))
	{
		auto resource = get_resource_with_buffer(buffer);
		invalidate_resource(resource);
	}
}

template <typename Resource>
void chomsky::DrawResourceCollection<Resource>::clear_invalid()
{
	for (auto resource: this->invalid)
	{
		this->resources.erase(resource);
		delete resource;
	}
	this->invalid.clear();
}

template <typename Resource>
typename chomsky::DrawResourceCollection<Resource>::iterator
chomsky::DrawResourceCollection<Resource>::begin()
{
	return this->resources.begin();
}

template <typename Resource>
typename chomsky::DrawResourceCollection<Resource>::const_iterator
chomsky::DrawResourceCollection<Resource>::begin() const
{
	return this->resources.begin();
}

template <typename Resource>
typename chomsky::DrawResourceCollection<Resource>::iterator
chomsky::DrawResourceCollection<Resource>::end()
{
	return this->resources.end();
}

template <typename Resource>
typename chomsky::DrawResourceCollection<Resource>::const_iterator
chomsky::DrawResourceCollection<Resource>::end() const
{
	return this->resources.end();
}

#endif
