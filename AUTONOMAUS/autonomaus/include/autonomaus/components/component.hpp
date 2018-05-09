// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_COMPONENT_HPP

#include <string>

namespace autonomaus
{
	class Autonomaus;

	class Component
	{
	public:
		explicit Component(Autonomaus& autonomaus);
		virtual ~Component() = default;

		virtual void show() = 0;
		virtual void update() = 0;
		virtual void reload() = 0;
		virtual std::string get_name() const = 0;

		Autonomaus& get_autonomaus() const;

	private:
		Autonomaus* autonomaus;
	};
}

#endif
