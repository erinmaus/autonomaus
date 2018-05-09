// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_STATE_BRAIN_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_STATE_BRAIN_COMPONENT_HPP

#include <string>
#include "autonomaus/components/component.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "autonomaus/state/memory/message.hpp"

namespace autonomaus
{
	class BrainComponent : public Component
	{
	public:
		BrainComponent(Autonomaus& autonomaus);
		~BrainComponent();

		void reload() override;
		void show() override;
		void update() override;
		std::string get_name() const;

	private:
		bool visible = false;
		void draw();

		Message message;
		std::string cortex;
		std::string action;
		std::string field;
		std::string value;
		int index = 0;

		Message message_result;
		Cortex::Result pending_result;
		bool updating = false;
		bool was_updating = false;
	};
}

#endif
