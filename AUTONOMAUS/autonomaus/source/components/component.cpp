// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#include "autonomaus/components/component.hpp"

autonomaus::Component::Component(Autonomaus& autonomaus)
{
	this->autonomaus = &autonomaus;
}

autonomaus::Autonomaus& autonomaus::Component::get_autonomaus() const
{
	return *this->autonomaus;
}
