// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_RESOURCE_MANAGER_HPP
#define TOUCHSTONE_GL_RESOURCE_MANAGER_HPP

#include <cstdio>
#include <memory>
#include <functional>
#include <unordered_map>
#include "touchstone/gl/types.hpp"
#include "touchstone/gl/resource.hpp"

namespace touchstone
{
	template <typename Resource>
	class ResourceManager
	{
	public:
		ResourceManager() = default;
		~ResourceManager() = default;

		void add(const Resource& resource);
		void remove(GLuint name);
		void remove(const Resource& resource);

		bool has(GLuint name) const;
		bool has(const Resource& resource) const;
		Resource& get(GLuint name);
		const Resource& get(GLuint name) const;

		void clear();

	private:
		typedef std::unordered_map<GLuint, Resource> ResourceCollection;
		ResourceCollection resources;

	public:
		typedef typename ResourceCollection::iterator iterator;
		typedef typename ResourceCollection::const_iterator const_iterator;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;
	};
}

template <typename Resource>
void touchstone::ResourceManager<Resource>::add(const Resource& resource)
{
	this->resources.emplace(resource.get_name(), resource);
}

template <typename Resource>
void touchstone::ResourceManager<Resource>::remove(GLuint name)
{
	this->resources.erase(name);
}

template <typename Resource>
void touchstone::ResourceManager<Resource>::remove(const Resource& resource)
{
	this->resources.erase(resource.get_name());
}

template <typename Resource>
bool touchstone::ResourceManager<Resource>::has(GLuint name) const
{
	return this->resources.count(name) != 0;
}

template <typename Resource>
bool touchstone::ResourceManager<Resource>::has(const Resource& resource) const
{
	return this->resources.count(resource.get_name()) != 0;
}

template <typename Resource>
Resource& touchstone::ResourceManager<Resource>::get(GLuint name)
{
	assert(has(name));
	return this->resources.find(name)->second;
}

template <typename Resource>
const Resource& touchstone::ResourceManager<Resource>::get(GLuint name) const
{
	assert(has(name));
	return this->resources.find(name)->second;
}

template <typename Resource>
void touchstone::ResourceManager<Resource>::clear()
{
	this->resources.clear();
}

template <typename Resource>
typename touchstone::ResourceManager<Resource>::iterator
touchstone::ResourceManager<Resource>::begin()
{
	return this->resources.begin();
}

template <typename Resource>
typename touchstone::ResourceManager<Resource>::const_iterator
touchstone::ResourceManager<Resource>::begin() const
{
	return this->resources.begin();
}

template <typename Resource>
typename touchstone::ResourceManager<Resource>::iterator
touchstone::ResourceManager<Resource>::end()
{
	return this->resources.end();
}

template <typename Resource>
typename touchstone::ResourceManager<Resource>::const_iterator
touchstone::ResourceManager<Resource>::end() const
{
	return this->resources.end();
}

#endif
