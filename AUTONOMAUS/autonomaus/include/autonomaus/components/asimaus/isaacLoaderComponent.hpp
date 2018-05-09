// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_ASIMAUS_ISAAC_LOADER_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_ASIMAUS_ISAAC_LOADER_COMPONENT_HPP

#include <memory>
#include <map>
#include <set>
#include <string>
#include "autonomaus/components/component.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/core/isaac.hpp"

namespace autonomaus
{
	class IsaacLoaderComponent : public Component
	{
	public:
		IsaacLoaderComponent(Autonomaus& autonomaus);
		~IsaacLoaderComponent() = default;

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();

		std::map<std::string, Isaac*> loaded;

		std::set<std::string> get_choices();
		Isaac* load_choice(const std::string& name);
	};
}

#endif
