// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_BRAIN_HPP
#define AUTONOMAUS_STATE_MEMORY_BRAIN_HPP

#include <functional>
#include <map>
#include <memory>
#include <type_traits>
#include <vector>
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/memory/cortex.hpp"

namespace autonomaus
{
	class Brain
	{
	public:
		Brain(Autonomaus& autonomaus);
		~Brain();

		void update();

		template <typename C>
		typename std::enable_if<std::is_base_of<Cortex, C>::value>::type* add();

		Cortex* get(const std::string& name);

	private:
		Autonomaus* autonomaus;

		typedef std::unique_ptr<Cortex> CortexPointer;
		std::vector<CortexPointer> cortexes;
		std::map<Cortex*, std::function<void()>> remove_funcs;

		typedef std::map<std::string, Cortex*> CortexMap;
		CortexMap cortexes_by_name;

	public:
		typedef CortexMap::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

template <typename C>
typename std::enable_if<std::is_base_of<autonomaus::Cortex, C>::value>::type*
autonomaus::Brain::add()
{
	auto previous = autonomaus->template get_userdata<C>();
	if (previous != nullptr)
	{
		return previous;
	}

	CortexPointer cortex = std::make_unique<C>(*this->autonomaus);
	autonomaus->template add_userdata<C>((C*)cortex.get());
	this->remove_funcs.emplace(
		cortex.get(),
		[this] { autonomaus->remove_userdata<C>(); });
	this->cortexes_by_name[cortex->get_name()] = cortex.get();
	this->cortexes.emplace_back(std::move(cortex));

	return cortex.get();
}

#endif
