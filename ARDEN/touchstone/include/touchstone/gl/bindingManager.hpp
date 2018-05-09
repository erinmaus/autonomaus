// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_GL_BINDING_MANAGER_HPP
#define TOUCHSTONE_GL_BINDING_MANAGER_HPP

#include <map>
#include "touchstone/gl/types.hpp"

namespace touchstone
{
	template <typename Target>
	class BindingManager
	{
	public:
		BindingManager() = default;
		~BindingManager() = default;

		void bind(const Target& target, GLuint name);
		void unbind(const Target& target);
		void unbind_all(GLuint name);

		bool is_resource_bound(const Target& target) const;
		GLuint get_current_name(const Target& target) const;

	private:
		typedef std::map<Target, GLuint> BindingCollection;

		BindingCollection bindings;

	public:
		typedef typename BindingCollection::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};

	struct SingletonBindingTarget
	{
		// Nothing.
	};

	template <>
	class BindingManager<SingletonBindingTarget>
	{
	public:
		BindingManager() = default;
		~BindingManager() = default;

		void bind(GLuint name);
		void unbind();
		void unbind_all(GLuint name);

		bool is_resource_bound() const;
		GLuint get_current_name() const;

	private:
		GLuint current_name = 0;
	};
}

template <typename Target>
void touchstone::BindingManager<Target>::bind(const Target& target, GLuint name)
{
	if (name == 0)
	{
		unbind(target);
	}
	else
	{
		this->bindings[target] = name;
	}
}

template <typename Target>
void touchstone::BindingManager<Target>::unbind(const Target& target)
{
	this->bindings.erase(target);
}

template <typename Target>
void touchstone::BindingManager<Target>::unbind_all(GLuint name)
{
	auto iter = this->bindings.begin();
	while (iter != this->bindings.end())
	{
		if (iter->second == name)
		{
			iter = this->bindings.erase(iter);
		}
		else
		{
			++iter;
		}
	}
}

template <typename Target>
bool touchstone::BindingManager<Target>::is_resource_bound(
	const Target& target) const
{
	return this->bindings.count(target) != 0;
}

template <typename Target>
GLuint touchstone::BindingManager<Target>::get_current_name(
	const Target& target) const
{
	if (!is_resource_bound(target))
	{
		return 0;
	}

	return this->bindings.find(target)->second;
}

template <typename Target>
typename touchstone::BindingManager<Target>::const_iterator
touchstone::BindingManager<Target>::begin() const
{
	return this->bindings.begin();
}

template <typename Target>
typename touchstone::BindingManager<Target>::const_iterator
touchstone::BindingManager<Target>::end() const
{
	return this->bindings.end();
}

#endif
