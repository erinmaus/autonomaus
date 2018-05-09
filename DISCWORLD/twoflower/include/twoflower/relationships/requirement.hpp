// This file is a part of DISCWORLD.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef TWOFLOWER_RELATIONSHIPS_REQUIREMENT_HPP
#define TWOFLOWER_RELATIONSHIPS_REQUIREMENT_HPP

#include <string>
#include "twoflower/relationships/resource.hpp"
#include "twoflower/relationships/action.hpp"

namespace twoflower
{
	class Requirement
	{
	public:
		Requirement() = default;
		~Requirement() = default;

		class Builder;
		Builder builder();

		int get_id() const;
		const Resource& get_resource() const;
		const Action& get_action() const;
		float get_count() const;
		bool is_input() const;
		bool is_output() const;

	private:
		int id = 0;
		Resource resource;
		Action action;
		float count = 0.0f;
		bool input = false;
		bool output = false;
	};

	class Requirement::Builder
	{
	public:
		Builder(Requirement& requirement);

		void set_id(int value);
		void set_resource(const Resource& value);
		void set_action(const Action& value);
		void set_count(float value);
		void set_is_input(bool value);
		void set_is_output(bool value);

	private:
		Requirement* requirement;
	};
}

#endif
