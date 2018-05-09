// This file is a part of EIZNEBAHN.
//
// Look, but don't touch.
//
// Copyright 2017 [bk]door.maus

#ifndef EIZNEBAHN_MOUSE_MOUSE_BUTTON_HPP
#define EIZNEBAHN_MOUSE_MOUSE_BUTTON_HPP

namespace eiznebahn
{
	struct MouseButton
	{
		enum
		{
			button_none = 0,
			button_left = 1,
			button_right = 2,
			button_middle = 3,
			button_up = 4,
			button_down = 5,

			button_max,
			button_min = 1
		};

		int button;
		float delay;
	};
}

#endif
