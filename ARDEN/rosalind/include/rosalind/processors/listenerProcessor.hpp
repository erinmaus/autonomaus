// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef ROSALIND_PROCESSORS_LISTENER_PROCESSOR_HPP
#define ROSALIND_PROCESSORS_LISTENER_PROCESSOR_HPP

#include <algorithm>
#include <vector>
#include <functional>
#include "rosalind/processors/eventProcessor.hpp"

namespace rosalind
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
		typedef typename std::vector<Listener*>::iterator iterator;
		typedef typename std::vector<Listener*>::const_iterator const_iterator;

		iterator begin();
		const_iterator begin() const;
		iterator end();
		const_iterator end() const;

		void for_each(
			const std::function<void(Listener*)>& func);

		template <typename Func, typename... A>
		void notify(
			Func func,
			A&&... arguments);

	private:
		std::vector<Listener*> listeners;
	};
}

template <typename Listener>
void rosalind::ListenerProcessor<Listener>::add(Listener* listener)
{
	remove(listener);
	this->listeners.push_back(listener);
}

template <typename Listener>
void rosalind::ListenerProcessor<Listener>::remove(Listener* listener)
{
	auto iter = std::find(
		this->listeners.begin(), this->listeners.end(), listener);
	if (iter != this->listeners.end())
	{
		this->listeners.erase(iter);
	}
}

template <typename Listener>
void rosalind::ListenerProcessor<Listener>::clear()
{
	this->listeners.clear();
}

template <typename Listener>
typename rosalind::ListenerProcessor<Listener>::iterator
rosalind::ListenerProcessor<Listener>::begin()
{
	return this->listeners.begin();
}

template <typename Listener>
typename rosalind::ListenerProcessor<Listener>::iterator
rosalind::ListenerProcessor<Listener>::end()
{
	return this->listeners.end();
}

template <typename Listener>
void rosalind::ListenerProcessor<Listener>::for_each(
	const std::function<void(Listener*)>& func)
{
	for (auto& listener: this->listeners)
	{
		func(listener);
	}
}

template <typename Listener>
		template <typename Func, typename... A>
void rosalind::ListenerProcessor<Listener>::notify(
	Func func,
	A&&... arguments)
{
	for (auto& listener: this->listeners)
	{
		func(listener, std::forward<A>(arguments)...);
	}
}

#endif
