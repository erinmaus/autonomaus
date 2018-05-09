// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_STATE_MEMORY_MASHINA_CORTEX_HPP
#define AUTONOMAUS_STATE_MEMORY_MASHINA_CORTEX_HPP

#include <memory>
#include <vector>
#include "autonomaus/bindings/lua.hpp"
#include "autonomaus/state/memory/cortex.hpp"
#include "twoflower/plan.hpp"

namespace autonomaus
{
	class MashinaCortex : public Cortex
	{
	public:
		MashinaCortex(Autonomaus& autonomaus);
		~MashinaCortex();

		float get_boredom() const;
		float get_aggravation() const;

		Cortex::Result require(const std::string& module);
		Cortex::Result do_file(const std::string& module);

		enum
		{
			type_goal = 0,
			type_chore,
			type_distraction,
			type_todo,
			type_interrupt,
			type_max,
			type_min = 0
		};
		Cortex::Result execute(const Message& message);
		Cortex::Result task(const Message& message);

		Cortex::Result goal(const Message& message);
		Cortex::Result visit(const Message& message);

		Cortex::Result yield();

		void register_task(const std::string& name, sol::function& task);

		void update() override;

	private:
		lua_State* L = nullptr;
		static lua_State* create_state(Autonomaus& autonomaus);

		sol::object find_best_task(const twoflower::Action& action, int count);

		class Task;
		class Lobe;

		enum
		{
			state_start = 0,
			state_begin,
			state_yield,
			state_resume,
			state_run
		};
		static const int STATE_TABLE[];
		int state = state_begin;
		int current_lobe = type_goal;

		class GoalAction;
		class VisitAction;

		std::unordered_map<std::string, sol::protected_function> tasks;

		std::unordered_map<int, std::unique_ptr<Lobe>> lobes;
		Lobe& get_current_lobe();
		Lobe& get_lobe(int type);
	};
}

#endif
