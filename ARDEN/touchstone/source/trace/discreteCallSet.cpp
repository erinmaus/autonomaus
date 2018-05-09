// This file is a part of ARDEN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "touchstone/trace/discreteCallSet.hpp"

bool touchstone::DiscreteCallSet::has(const DiscreteCall& call) const
{
	return this->calls.count(call) > 0;
}

bool touchstone::DiscreteCallSet::add(const DiscreteCall& call)
{
	if (!call)
	{
		return false;
	}

	auto result = this->calls.insert(call);
	return result.second;
}

bool touchstone::DiscreteCallSet::remove(const DiscreteCall& call)
{
	return this->calls.erase(call) > 0;
}

std::size_t touchstone::DiscreteCallSet::count() const
{
	return this->calls.size();
}

touchstone::DiscreteCall touchstone::DiscreteCallSet::at(
	std::size_t index) const
{
	if (index < count())
	{
		return *(this->calls.begin() + index);
	}

	return DiscreteCall();
}

void touchstone::DiscreteCallSet::clear()
{
	this->calls.clear();
}

touchstone::DiscreteCallSet::const_iterator
touchstone::DiscreteCallSet::begin() const
{
	return this->calls.begin();
}

touchstone::DiscreteCallSet::const_iterator
touchstone::DiscreteCallSet::end() const
{
	return this->calls.end();
}
