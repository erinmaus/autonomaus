// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/state/memory/brain.hpp"
#include "autonomaus/state/memory/rightClickMenuCortex.hpp"

autonomaus::Brain::Brain(Autonomaus& autonomaus)
{
	this->autonomaus = &autonomaus;
}

autonomaus::Brain::~Brain()
{
	for (auto& i: remove_funcs)
	{
		i.second();
	}
}

void autonomaus::Brain::update()
{
	for (auto& cortex: cortexes)
	{
		cortex->update();
	}
}

autonomaus::Cortex* autonomaus::Brain::get(const std::string& name)
{
	auto iter = this->cortexes_by_name.find(name);
	if (iter != this->cortexes_by_name.end())
	{
		return iter->second;
	}

	return nullptr;
}

autonomaus::Brain::const_iterator autonomaus::Brain::begin() const
{
	return this->cortexes_by_name.begin();
}

autonomaus::Brain::const_iterator autonomaus::Brain::end() const
{
	return this->cortexes_by_name.end();
}
