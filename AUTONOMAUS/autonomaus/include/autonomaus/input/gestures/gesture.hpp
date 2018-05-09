// This file is a part of AUTONOMAUS.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef AUTONOMAUS_INPUT_GESTURES_GESTURE_HPP
#define AUTONOMAUS_INPUT_GESTURES_GESTURE_HPP

#include "autonomaus/input/inputManager.hpp"

namespace autonomaus
{
	class Gesture
	{
	public:
		Gesture(InputManager& input_manager);
		virtual ~Gesture();

		enum
		{
			success,
			error_outside_viewport,
			error_no_matches,
			error_unbalanced
		};
		virtual int play() = 0;

		bool pending() const;
		void cancel();

	protected:
		InputManager& get_input_manager() const;

		eiznebahn::InputQueue::Group get_group() const;
		void set_group(eiznebahn::InputQueue::Group value);

	private:
		InputManager* input_manager;

		bool is_group_set = false;
		eiznebahn::InputQueue::Group group = 0;
	};
}

#endif
