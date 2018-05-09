// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_COMPONENTS_ASIMAUS_DEBUGGER_COMPONENT_HPP
#define AUTONOMAUS_COMPONENTS_ASIMAUS_DEBUGGER_COMPONENT_HPP

#include <memory>
#include "autonomaus/components/component.hpp"
#include "autonomaus/core/autonomaus.hpp"
#include "autonomaus/mashina/mashina.hpp"

namespace autonomaus
{
	class DebuggerComponent : public Component
	{
	public:
		DebuggerComponent(Autonomaus& autonomaus);
		~DebuggerComponent() = default;

		void start(mashina::Tree& value, mashina::Executor& executor);
		void stop();

		void show() override;
		void update() override;
		void reload() override;
		std::string get_name() const override;

	private:
		bool visible = false;
		void draw();
		void draw_controls();

		bool draw_tree_view_inline = true;
		void draw_tree();
		void draw_tree_node(mashina::Tree& tree, mashina::Node& node);

		bool draw_state_view_inline = true;
		void draw_state();

		mashina::Tree* tree = nullptr;
		mashina::Executor* executor = nullptr;
		mashina::Node* previous_node = nullptr;
		mashina::Node* current_node = nullptr;
		mashina::Node* next_node = nullptr;
		bool working = false;
		std::unique_ptr<mashina::DebugContext> context;
	};
}

#endif
