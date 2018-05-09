// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TOUCHSTONE_TRACE_DISCRETE_CALL_SET_HPP
#define TOUCHSTONE_TRACE_DISCRETE_CALL_SET_HPP

#include <boost/container/flat_set.hpp>
#include "touchstone/trace/discreteCall.hpp"

namespace touchstone
{
	class Call;

	class DiscreteCallSet
	{
	public:
		DiscreteCallSet() = default;
		~DiscreteCallSet() = default;

		bool has(const DiscreteCall& call) const;
		bool add(const DiscreteCall& call);
		bool remove(const DiscreteCall& call);

		std::size_t count() const;

		DiscreteCall at(std::size_t index) const;

		void clear();

	private:
		typedef boost::container::flat_set<DiscreteCall> CallSet;
		CallSet calls;

	public:
		typedef CallSet::const_iterator const_iterator;

		const_iterator begin() const;
		const_iterator end() const;
	};
}

#endif
