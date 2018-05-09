// This file is a part of KOMPROMAT.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef KOMPROMAT_LISTENER_PROCESSOR_HPP
#define KOMPROMAT_LISTENER_PROCESSOR_HPP

#include <algorithm>
#include <string>
#include <vector>
#include <functional>
#include "kompromat/eventProcessor.hpp"

namespace kompromat
{
	template <typename Listener>
	class ListenerProcessor : public EventProcessor
	{
	public:
		ListenerProcessor() = default;
		virtual ~ListenerProcessor() = default;

		void add(Listener* listener);
		void remove(Listener* listener);
		void clear();

	protected:
		typedef std::vector<Listener*> iterator;

		iterator begin();
		iterator end();

		void for_each(
			const std::function<void(Listener*)>& func);

	private:
		std::vector<Listener*> listeners;
	};
}

template <typename Listener>
void kompromat::ListenerProcessor<Listener>::add(Listener* listener)
{
	remove(listener);
	this->listeners.push_back(listener);
}

template <typename Listener>
void kompromat::ListenerProcessor<Listener>::remove(Listener* listener)
{
	auto iter = std::find(
		this->listeners.begin(), this->listeners.end(), listener);
	if (iter != this->listeners.end())
	{
		this->listeners.erase(iter);
	}
}

template <typename Listener>
void kompromat::ListenerProcessor<Listener>::clear()
{
	this->listeners.clear();
}

template <typename Listener>
typename kompromat::ListenerProcessor<Listener>::iterator
kompromat::ListenerProcessor<Listener>::begin()
{
	return this->listeners.begin();
}

template <typename Listener>
typename kompromat::ListenerProcessor<Listener>::iterator
kompromat::ListenerProcessor<Listener>::end()
{
	return this->listeners.end();
}

template <typename Listener>
void kompromat::ListenerProcessor<Listener>::for_each(
	const std::function<void(Listener*)>& func)
{
	for (auto& listener: this->listeners)
	{
		func(listener);
	}
}

#endif
